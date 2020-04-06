/**
 * kmalloc.h: Kernel dynamic memory allocation
 */

#ifndef _KERNEL_KMALLOC_H
#define _KERNEL_KMALLOC_H

#include "std.h"

// List node for kmalloc free list
typedef struct km_node
{
    struct km_node *prev;
    struct km_node *next;
} km_node_t;

#define KM_MIN_ALLOC_SIZE   ((size_t)sizeof(km_node_t))
#define KM_CHUNK_FREE       ((size_t)1)

// km_ctxt: heap context for kmalloc
typedef struct
{
    km_node_t *free_list;   // Pointer to free list
    uintptr_t start;        // Pointer to start of context heap
    uintptr_t end;          // Pointer to end of context heap
} km_ctxt_t;

typedef struct
{
    // NOTE we use the least significant bit of size_prev to tell whether the
    // previous memory chunk is absent (hi: absent, lo: present)
    size_t size_prev;
    size_t size;
} km_chunk_desc_t;

void *
alloc(km_ctxt_t *ctxt, uintptr_t (*get_page_pma)(), size_t align, size_t bytes);
void * kalloc(uintptr_t (*get_page_pma)(), size_t alignment, size_t bytes);
void kmalloc_test(void);
km_ctxt_t km_new_heap(uintptr_t heap_start);
void kmalloc_init(uintptr_t heap_start);
void * kmalloc(size_t bytes);
void kfree(void *address);

typedef struct mem_chunk {
    size_t *size_prev;  // Pointer to size of previous chunk
    size_t *size;       // Pointer to size of current chunk
    uintptr_t addr;     // Address of chunk
} mem_chunk_t;

#endif  // _KERNEL_KMALLOC_H
