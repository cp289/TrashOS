/**
 * alloc.c: Kernel memory allocator
 */

#include "asm.h"
#include "io.h"
#include "alloc.h"
#include "mem.h"
#include "page.h"

static alloc_ctxt_t kernel_ctxt;

// Return pointer to descriptor before memory chunk
static inline alloc_chunk_desc_t * alloc_get_desc(void *vma)
{
    return (alloc_chunk_desc_t*)vma - 1;
}

// Return size field for memory chunk (with flags cleared)
static inline size_t alloc_get_size(alloc_node_t *node)
{
    return alloc_get_desc(node)->size & ~(size_t)0x7;
}

// Return size_prev field for memory chunk (with flags cleared)
static inline size_t alloc_get_size_prev(alloc_node_t *node)
{
    return alloc_get_desc(node)->size_prev & ~(size_t)0x7;
}

// Return pointer to descriptor after memory chunk
static inline alloc_chunk_desc_t * alloc_get_next_desc(alloc_node_t *node)
{
    return (void*)((uintptr_t)node + alloc_get_size(node));
}

// Return size field for next memory chunk (with flags cleared)
static inline size_t alloc_get_size_next(alloc_node_t *node)
{
    return alloc_get_next_desc(node)->size & ~(size_t)0x7;
}

// Return pointer to descriptor before memory chunk
static inline alloc_chunk_desc_t * alloc_get_prev_desc(alloc_node_t *node)
{
    return (void*)((uintptr_t)node - alloc_get_size_prev(node)
                   - (sizeof(alloc_chunk_desc_t) << 1));
}

// Return pointer to free list node given chunk descriptor pointer
static inline alloc_node_t * alloc_get_node(alloc_chunk_desc_t *chunk)
{
    return (void*)((uintptr_t)chunk + sizeof(alloc_chunk_desc_t));
}

// Unlink free list node from the list
static inline void alloc_node_unlink(alloc_ctxt_t *ctxt, alloc_node_t *node)
{
    if (node->prev)
        node->prev->next = node->next;
    else
        ctxt->free_list = node->next;
    if (node->next)
        node->next->prev = node->prev;
}

static inline void alloc_node_add(alloc_ctxt_t *ctxt, alloc_node_t *node)
{
    node->prev = (uintptr_t)0;
    node->next = ctxt->free_list;
    if (ctxt->free_list)
        ctxt->free_list->prev = node;
    ctxt->free_list = node;
}

// Clear free flags in chunk descriptors for given VMA
static inline void alloc_clear_free(void *vma)
{
    alloc_get_desc(vma)->size &= ~KM_CHUNK_FREE;
    alloc_get_next_desc(vma)->size_prev &= ~KM_CHUNK_FREE;
}

// Set free flags in chunk descriptors for given VMA
static inline void alloc_set_free(void *vma)
{
    alloc_get_desc(vma)->size |= KM_CHUNK_FREE;
    alloc_get_next_desc(vma)->size_prev |= KM_CHUNK_FREE;
}

void *
alloc(alloc_ctxt_t *ctxt, uintptr_t (*get_page_pma)(), size_t alignment, size_t bytes)
{
    bytes = bytes ? align(bytes, KM_MIN_ALLOC_SIZE) : KM_MIN_ALLOC_SIZE;

    // TODO We temporary avoid the free list for aligned allocations
    if (alignment != KM_MIN_ALLOC_SIZE)
        goto extend_heap;

    // Check for an appropriately sized memory chunk in the free list
    for (alloc_node_t *n = ctxt->free_list; n; ) {
        size_t size = alloc_get_size(n);
        if (size == bytes) {
            // We found a memory chunk of perfect size
            alloc_node_unlink(ctxt, n);
            alloc_clear_free(n);
            return n;
        } else if (size > bytes + sizeof(alloc_chunk_desc_t)) {
            // We found a memory chunk that can be split

            // Setup new chunk descriptor for leftover space
            alloc_chunk_desc_t *chunk = alloc_get_desc(n);
            alloc_chunk_desc_t *new_chunk = (void*)((uintptr_t)n + bytes);
            new_chunk->size_prev = bytes;
            new_chunk->size = chunk->size - bytes - sizeof(alloc_chunk_desc_t);
            new_chunk->size |= KM_CHUNK_FREE;
            alloc_get_next_desc(n)->size_prev = new_chunk->size | KM_CHUNK_FREE;

            // Adjust chunk size
            chunk->size = bytes;

            // Update free list node
            alloc_node_t *new_node = (void*)((uintptr_t)n + bytes
                                     + sizeof(alloc_chunk_desc_t));
            *new_node = *n;
            if (new_node->prev)
                new_node->prev->next = new_node;
            else
                ctxt->free_list = new_node;
            if (new_node->next)
                new_node->next->prev = new_node;
            return n;
        }
        n = n->next;
    }

    // There is nothing suitable in the free list, so we extend the heap
    uintptr_t free_block_size;

extend_heap:

    // First, we enforce alignment request

    // Add unused space to free list if necessary
    free_block_size = align(ctxt->end, alignment) - ctxt->end;
    if (free_block_size) {
        if (free_block_size <= sizeof(alloc_chunk_desc_t)) {
            printk("alloc: fatal: free block below min size: %u\n", free_block_size);
            die();
            // TODO extend previous memory chunk size to cover unused region
        } else {
            // Push unused region to the free list
            // TODO write test case
            alloc_chunk_desc_t *free_chunk = alloc_get_desc((void*)ctxt->end);
            free_chunk->size = free_block_size - sizeof(alloc_chunk_desc_t);
            free_chunk->size |= KM_CHUNK_FREE;
            alloc_chunk_desc_t *free_chunk_next = alloc_get_next_desc((void*)ctxt->end);
            free_chunk_next->size_prev = free_chunk->size;
            alloc_node_add(ctxt, (void*)ctxt->end);
        }
        ctxt->end += free_block_size;
    }

    // Update end of the heap
    // WARNING ctxt->end must be updated here before calling get_page_pma() to
    // avoid a potential nested invocation of alloc()
    uintptr_t old_ctxt_end = ctxt->end;
    ctxt->end += bytes + sizeof(alloc_chunk_desc_t);

    // Next, allocate/map as many new physical pages as needed
    for (size_t page_vma = align(old_ctxt_end, PAGE_SIZE);
         page_vma < old_ctxt_end + bytes + sizeof(alloc_chunk_desc_t);
         page_vma += PAGE_SIZE) {
        uintptr_t page_pma = get_page_pma();
        page_set_entry(page_vma, page_pma | PAGE_WRITE | PAGE_PRESENT);
    }

    // Initialize chunk descriptors
    void *vma = (void*)old_ctxt_end;
    alloc_chunk_desc_t *chunk = alloc_get_desc(vma);
    chunk->size = bytes;

    alloc_chunk_desc_t *chunk_next = alloc_get_next_desc(vma);
    chunk_next->size_prev = bytes;
    chunk_next->size = 0;

    return vma;
}

void * kalloc(uintptr_t (*get_page_pma)(), size_t alignment, size_t bytes)
{
    return alloc(&kernel_ctxt, get_page_pma, alignment, bytes);
}

void free(alloc_ctxt_t *ctxt, void *vma)
{
    alloc_chunk_desc_t *chunk_next = alloc_get_next_desc(vma);
    alloc_chunk_desc_t *chunk = alloc_get_desc(vma);
    alloc_chunk_desc_t *chunk_prev = alloc_get_prev_desc(vma);

    // Try to combine with next memory chunk
    if (chunk_next->size & KM_CHUNK_FREE) {
        // Remove chunk_next from free list
        alloc_node_unlink(ctxt, alloc_get_node(chunk_next));

        // Update chunk size
        chunk->size += alloc_get_size_next(vma) + sizeof(alloc_chunk_desc_t);
        alloc_get_next_desc(vma)->size_prev = chunk->size;
    }

    // Try to combine with previous memory chunk
    if (chunk->size_prev & KM_CHUNK_FREE) {
        // Remove chunk_prev from free list
        // TODO combine these two lines
        alloc_node_t *node = alloc_get_node(chunk_prev);
        alloc_node_unlink(ctxt, node);
        // Update chunk_prev
        chunk_prev->size = alloc_get_size_prev(vma) + alloc_get_size(vma)
                           + sizeof(alloc_chunk_desc_t);
        vma = (void*)alloc_get_node(chunk_prev);
        chunk = chunk_prev;
        alloc_get_next_desc(vma)->size_prev = chunk->size;
    }

    // Check to see if block is at the end of the heap
    if ((uintptr_t)vma + alloc_get_size(vma) + sizeof(alloc_chunk_desc_t) == ctxt->end) {
        // Free as many pages as necessary
        for (uintptr_t page = ctxt->end & ~(uintptr_t)0xfff;
             page >= (uintptr_t)vma;
             page -= PAGE_SIZE) {
            page_free(page);
        }
        ctxt->end = (uintptr_t)vma;
    } else {
        // Set free flags for block
        alloc_set_free(vma);
        // Add node
        alloc_node_add(ctxt, vma);
    }
}

// Initialize heap given heap start address
alloc_ctxt_t alloc_new_heap(uintptr_t heap_start)
{
    // We initialize a new kmalloc context by initializing the first memory
    // chunk descriptor and setting the end of the heap directly after

    uintptr_t first_chunk_addr = align(heap_start, KM_MIN_ALLOC_SIZE);
    if (!page_is_present(first_chunk_addr)) {
        page_set_entry(first_chunk_addr, page_new() | PAGE_WRITE | PAGE_PRESENT);
    }
    alloc_chunk_desc_t * first_chunk = (void*)first_chunk_addr;
    first_chunk->size_prev = 0;

    return (alloc_ctxt_t) {
        .free_list = NULL,
        .start = heap_start,
        .end = first_chunk_addr + sizeof(alloc_chunk_desc_t),
    };
}

void kmalloc_init(uintptr_t heap_start)
{
    kernel_ctxt = alloc_new_heap(heap_start);
}

void * kmalloc(size_t bytes)
{
    return alloc(&kernel_ctxt, &page_new, KM_MIN_ALLOC_SIZE, bytes);
}

void * kmalloc_aligned(size_t bytes, size_t align)
{
    return alloc(&kernel_ctxt, &page_new, align, bytes);
}

void kfree(void *vma)
{
    free(&kernel_ctxt, vma);
}

// Print free list
static void alloc_print_free_list(alloc_ctxt_t *ctxt)
{
    printk("print_free_list: free_list: %p, &free_list: %p\n",
           ctxt->free_list, &ctxt->free_list);
    alloc_node_t *n = ctxt->free_list;
    size_t i = 0;
    while (n) {
        printk("  [%u]: vma: %p, size: %u, prev: %p, next: %p\n",
               i++, n, alloc_get_size(n), n->prev, n->next);
        n = n->next;
    }
    printk("  [END]\n");
}

void kmalloc_test(void)
{
    void *p1, *p2, *p3, *p4;
    uintptr_t heap_start = kernel_ctxt.end;

    // Test allocation
    printk("kmalloc_test: Test allocation\n");
    p1 = kmalloc(1);
    p2 = kmalloc(23);
    p3 = kmalloc(12);
    p4 = kmalloc(24);

    printk("kmalloc_test: end: %p, correct: %p\n", kernel_ctxt.end, heap_start + 104);
    alloc_print_free_list(&kernel_ctxt);

    // Test freeing and partial reallocation
    printk("kmalloc_test: Test freeing and partial reallocation\n");
    void *p2_prev = p2;
    kfree(p2);
    p2 = kmalloc(8);

    printk("kmalloc_test: new_p2: %p, correct: %p\n", p2, p2_prev);
    alloc_print_free_list(&kernel_ctxt);

    // Test free chunk consolidation
    printk("kmalloc_test: Test free chunk consolidation\n");
    kfree(p1);
    kfree(p3);
    alloc_print_free_list(&kernel_ctxt);

    kfree(p2);
    alloc_print_free_list(&kernel_ctxt);

    // Test heap shrinking
    printk("kmalloc_test: Test heap shrinking\n");
    kfree(p4);
    alloc_print_free_list(&kernel_ctxt);
    printk("kmalloc_test: heap start: %p, heap end: %p\n", heap_start, kernel_ctxt.end);

    // Test large allocation
    printk("kmalloc_test: Test large allocation\n");
    char *char_array = kmalloc(4 * PAGE_SIZE);
    kfree(char_array);

    // Uncomment the below to make sure that writing past the kernel heap
    // results in a page fault

    //printk("kmalloc_test: Attempting to write to freed pages\n");
    //for (size_t i = 0; i < 4 * PAGE_SIZE; i++) {
    //    char_array[i] = 'a';
    //}
}
