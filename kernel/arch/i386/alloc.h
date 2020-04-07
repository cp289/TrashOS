/**
 * kmalloc.h: Kernel dynamic memory allocation
 */

#ifndef _KERNEL_KMALLOC_H
#define _KERNEL_KMALLOC_H

#include "std.h"

// List node for kmalloc free list
typedef struct alloc_node
{
    struct alloc_node *prev;
    struct alloc_node *next;
} alloc_node_t;

#define KM_MIN_ALLOC_SIZE   ((size_t)sizeof(alloc_node_t))
#define KM_CHUNK_FREE       ((size_t)1)

// alloc_ctxt: heap context for kmalloc
typedef struct
{
    alloc_node_t *free_list;// Pointer to free list
    uintptr_t start;        // Pointer to start of context heap
    uintptr_t end;          // Pointer to end of context heap
} alloc_ctxt_t;

typedef struct
{
    // NOTE we use the least significant bit of size_prev to tell whether the
    // previous memory chunk is absent (hi: absent, lo: present)
    size_t size_prev;
    size_t size;
} alloc_chunk_desc_t;

void *
alloc(alloc_ctxt_t *ctxt, uintptr_t (*get_page_pma)(), size_t align, size_t bytes);
void free(alloc_ctxt_t *ctxt, void *vma);
void * kalloc(uintptr_t (*get_page_pma)(), size_t alignment, size_t bytes);
void kmalloc_test(void);
alloc_ctxt_t alloc_new_heap(uintptr_t heap_start);
void kmalloc_init(uintptr_t heap_start);
void * kmalloc(size_t bytes);
void kfree(void *address);

#endif  // _KERNEL_KMALLOC_H
