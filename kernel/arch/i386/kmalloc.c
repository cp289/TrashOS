/**
 * kmalloc.c: Kernel memory allocator
 */

#include "io.h"
#include "kmalloc.h"
#include "mem.h"
#include "page.h"

static km_ctxt_t kernel_ctxt;

// Return pointer to descriptor before memory chunk
static inline km_chunk_desc_t * km_get_desc(void *vma)
{
    return (km_chunk_desc_t*)vma - 1;
}

// Return size field for memory chunk (with flags cleared)
static inline size_t km_get_size(km_node_t *node)
{
    return km_get_desc(node)->size & ~(size_t)0x7;
}

// Return size_prev field for memory chunk (with flags cleared)
static inline size_t km_get_size_prev(km_node_t *node)
{
    return km_get_desc(node)->size_prev & ~(size_t)0x7;
}

// Return pointer to descriptor after memory chunk
static inline km_chunk_desc_t * km_get_next_desc(km_node_t *node)
{
    return (void*)((uintptr_t)node + km_get_size(node));
}

// Return size field for next memory chunk (with flags cleared)
static inline size_t km_get_size_next(km_node_t *node)
{
    return km_get_next_desc(node)->size & ~(size_t)0x7;
}

// Return pointer to descriptor before memory chunk
static inline km_chunk_desc_t * km_get_prev_desc(km_node_t *node)
{
    return (void*)((uintptr_t)node - km_get_size_prev(node) - (sizeof(km_chunk_desc_t) << 1));
}

// Return pointer to free list node given chunk descriptor pointer
static inline km_node_t * km_get_node(km_chunk_desc_t *chunk)
{
    return (void*)((uintptr_t)chunk + sizeof(km_chunk_desc_t));
}

// Unlink free list node from the list
static inline void km_node_unlink(km_ctxt_t *ctxt, km_node_t *node)
{
    if (node->prev)
        node->prev->next = node->next;
    else
        ctxt->free_list = node->next;
    if (node->next)
        node->next->prev = node->prev;
}

static inline void km_node_add(km_ctxt_t *ctxt, km_node_t *node)
{
    node->prev = (uintptr_t)0;
    node->next = ctxt->free_list;
    if (ctxt->free_list)
        ctxt->free_list->prev = node;
    ctxt->free_list = node;
}

// Clear free flags in chunk descriptors for given VMA
static inline void km_clear_free(void *vma)
{
    km_get_desc(vma)->size &= ~KM_CHUNK_FREE;
    km_get_next_desc(vma)->size_prev &= ~KM_CHUNK_FREE;
}

// Set free flags in chunk descriptors for given VMA
static inline void km_set_free(void *vma)
{
    km_get_desc(vma)->size |= KM_CHUNK_FREE;
    km_get_next_desc(vma)->size_prev |= KM_CHUNK_FREE;
}

void * alloc(km_ctxt_t *ctxt, size_t bytes)
{
    bytes = bytes ? align(bytes, KM_MIN_ALLOC_SIZE) : KM_MIN_ALLOC_SIZE;

    // Check for an appropriately sized memory chunk in the free list
    for (km_node_t *n = ctxt->free_list; n; ) {
        size_t size = km_get_size(n);
        if (size == bytes) {
            // We found a memory chunk of perfect size
            km_node_unlink(ctxt, n);
            km_clear_free(n);
            return n;
        } else if (size > bytes + sizeof(km_chunk_desc_t)) {
            // We found a memory chunk that can be split

            // Setup new chunk descriptor for leftover space
            km_chunk_desc_t *chunk = km_get_desc(n);
            km_chunk_desc_t *new_chunk = (void*)((uintptr_t)n + bytes);
            new_chunk->size_prev = bytes;
            new_chunk->size = chunk->size - bytes - sizeof(km_chunk_desc_t);
            new_chunk->size |= KM_CHUNK_FREE;
            km_get_next_desc(n)->size_prev = new_chunk->size | KM_CHUNK_FREE;

            // Adjust chunk size
            chunk->size = bytes;

            // Update free list node
            km_node_t *new_node = (void*)((uintptr_t)n + bytes + sizeof(km_chunk_desc_t));
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

    // First, allocate as many new pages as needed
    for (size_t page = align(ctxt->end, PAGE_SIZE);
         page < ctxt->end + bytes + sizeof(km_chunk_desc_t);
         page += PAGE_SIZE) {
        page_new(page, PAGE_WRITE | PAGE_PRESENT);
    }

    // Initialize chunk descriptors
    void *vma = (void*)ctxt->end;
    km_chunk_desc_t *chunk = km_get_desc(vma);
    chunk->size = bytes;

    km_chunk_desc_t *chunk_next = km_get_next_desc(vma);
    chunk_next->size_prev = bytes;
    chunk_next->size = 0;

    // Update end of the heap
    ctxt->end += bytes + sizeof(km_chunk_desc_t);

    return vma;
}

void km_free(km_ctxt_t *ctxt, void *vma)
{
    km_chunk_desc_t *chunk_next = km_get_next_desc(vma);
    km_chunk_desc_t *chunk = km_get_desc(vma);
    km_chunk_desc_t *chunk_prev = km_get_prev_desc(vma);

    // Try to combine with next memory chunk
    if (chunk_next->size & KM_CHUNK_FREE) {
        // Remove chunk_next from free list
        km_node_unlink(ctxt, km_get_node(chunk_next));

        // Update chunk size
        chunk->size += km_get_size_next(vma) + sizeof(km_chunk_desc_t);
        km_get_next_desc(vma)->size_prev = chunk->size;
    }

    // Try to combine with previous memory chunk
    if (chunk->size_prev & KM_CHUNK_FREE) {
        // Remove chunk_prev from free list
        // TODO combine these two lines
        km_node_t *node = km_get_node(chunk_prev);
        km_node_unlink(ctxt, node);
        // Update chunk_prev
        chunk_prev->size = km_get_size_prev(vma) + km_get_size(vma) + sizeof(km_chunk_desc_t);
        vma = (void*)km_get_node(chunk_prev);
        chunk = chunk_prev;
        km_get_next_desc(vma)->size_prev = chunk->size;
    }

    // Check to see if block is at the end of the heap
    if ((uintptr_t)vma + km_get_size(vma) + sizeof(km_chunk_desc_t) == ctxt->end) {
        // Free as many pages as necessary
        for (uintptr_t page = ctxt->end & ~(uintptr_t)0xfff;
             page >= (uintptr_t)vma;
             page -= PAGE_SIZE) {
            page_free(page);
        }
        ctxt->end = (uintptr_t)vma;
    } else {
        // Set free flags for block
        km_set_free(vma);
        // Add node
        km_node_add(ctxt, vma);
    }
}

// Initialize heap given heap start address
km_ctxt_t km_new_heap(uintptr_t heap_start)
{
    // We initialize a new kmalloc context by initializing the first memory
    // chunk descriptor and setting the end of the heap directly after

    uintptr_t first_chunk_addr = align(heap_start, KM_MIN_ALLOC_SIZE);
    if (!page_is_present(first_chunk_addr)) {
        page_new(first_chunk_addr, PAGE_WRITE | PAGE_PRESENT);
    }
    km_chunk_desc_t * first_chunk = (void*)first_chunk_addr;
    first_chunk->size_prev = 0;

    return (km_ctxt_t) {
        .free_list = NULL,
        .start = heap_start,
        .end = first_chunk_addr + sizeof(km_chunk_desc_t),
    };
}

void kmalloc_init(uintptr_t heap_start)
{
    kernel_ctxt = km_new_heap(heap_start);
}

void * kmalloc(size_t bytes)
{
    return alloc(&kernel_ctxt, bytes);
}

void kfree(void *vma)
{
    km_free(&kernel_ctxt, vma);
}

// Print free list
static void km_print_free_list(km_ctxt_t *ctxt)
{
    printk("print_free_list: free_list: %p, &free_list: %p\n", ctxt->free_list, &ctxt->free_list);
    km_node_t *n = ctxt->free_list;
    size_t i = 0;
    while (n) {
        printk("  [%u]: vma: %p, size: %u, prev: %p, next: %p\n",
               i++, n, km_get_size(n), n->prev, n->next);
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
    km_print_free_list(&kernel_ctxt);

    // Test freeing and partial reallocation
    printk("kmalloc_test: Test freeing and partial reallocation\n");
    void *p2_prev = p2;
    kfree(p2);
    p2 = kmalloc(8);

    printk("kmalloc_test: new_p2: %p, correct: %p\n", p2, p2_prev);
    km_print_free_list(&kernel_ctxt);

    // Test free chunk consolidation
    printk("kmalloc_test: Test free chunk consolidation\n");
    kfree(p1);
    kfree(p3);
    km_print_free_list(&kernel_ctxt);

    kfree(p2);
    km_print_free_list(&kernel_ctxt);

    // Test heap shrinking
    printk("kmalloc_test: Test heap shrinking\n");
    kfree(p4);
    km_print_free_list(&kernel_ctxt);
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
