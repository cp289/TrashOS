/**
 * page.c: Paging
 */

#include "alloc.h"
#include "asm.h"
#include "page.h"
#include "mem.h"
#include "std.h"

uintptr_t *page_dir;
uintptr_t *page_table_lookup;
uintptr_t kernel_heap_end_pma;
uintptr_t kernel_heap_end_vma;

static page_free_node_t *page_free_list = NULL;

// Static functions
static inline uintptr_t page_get_dir_idx(uintptr_t vma);
static inline uintptr_t page_get_table_idx(uintptr_t vma);
static void * page_get_table(uintptr_t vma);

// Erase page (overwrite with zeros)
void page_clear(uintptr_t vma)
{
    uint64_t *entry = (void*)vma;
    for (uintptr_t i = 0; i < PAGE_SIZE / sizeof(*entry); i++) {
        entry[i] = (uint64_t)0;
    }
}

// Map page table
void page_table_map(uintptr_t table_vma, uintptr_t vma, uintptr_t flags)
{
    const uintptr_t idx = page_get_dir_idx(vma);
    page_table_lookup[idx] = table_vma;
    page_dir[idx] = page_get_pma(table_vma) | flags;
}

// Set page directory entry directly by directory index
void page_set_dir_entry(uintptr_t idx, uintptr_t table_vma, page_entry_t entry)
{
    page_table_lookup[idx] = table_vma;
    page_dir[idx] = entry;
}

// Unmap page table (zero out page directory entry)
void page_table_unmap(uintptr_t vma)
{
    const uintptr_t idx = page_get_dir_idx(vma);
    page_table_lookup[idx] = (page_entry_t)0;
    page_dir[idx] = (page_entry_t)0;
}

// Unmap page table by directory index
void page_table_unmap_idx(uintptr_t idx)
{
    page_table_lookup[idx] = (page_entry_t)0;
    page_dir[idx] = (page_entry_t)0;
}

static inline uintptr_t page_get_dir_idx(uintptr_t vma)
{
    return vma >> 22;
}

static inline uintptr_t page_get_table_idx(uintptr_t vma)
{
    return vma >> 12 & 0x3ff;
}

static void * page_get_table(uintptr_t vma)
{
    const uintptr_t idx = page_get_dir_idx(vma);
    return (void*)page_table_lookup[idx];
}

// Remap page VMA to different PMA without modifying flags
void page_remap(uintptr_t vma, uintptr_t pma)
{
    const uintptr_t table_idx = page_get_table_idx(vma);
    page_entry_t *table = page_get_table(vma);
    table[table_idx] = (table[table_idx] & 0xfff) | pma;
    invlpg(vma);
}

// Set page table entry for VMA
void page_set_entry(uintptr_t vma, page_entry_t entry)
{
    page_entry_t *table = page_get_table(vma);
    table[page_get_table_idx(vma)] = entry;
}

// Unmap page by clearing Present flag
void page_unmap(uintptr_t vma)
{
    const uintptr_t table_idx = page_get_table_idx(vma);
    page_entry_t *table = page_get_table(vma);
    table[table_idx] = table[table_idx] & ~PAGE_PRESENT;
    invlpg(vma);
}

// Unmap page by clearing entire page table entry
void page_delete(uintptr_t vma)
{
    page_entry_t *table = page_get_table(vma);
    table[page_get_table_idx(vma)] = (page_entry_t)0;
    invlpg(vma);
}

void page_set_flags(uintptr_t vma, uintptr_t flags)
{
    const uintptr_t page_table_idx = page_get_table_idx(vma);
    page_entry_t *table = page_get_table(vma);
    table[page_table_idx] = (table[page_table_idx] >> 12 << 12) | flags;
}

// Return page table entry for vma
page_entry_t page_get_entry(uintptr_t vma)
{
    page_entry_t *table = page_get_table(vma);
    return table[page_get_table_idx(vma)];
}

// Return page table entry for vma
uintptr_t page_get_pma(uintptr_t vma)
{
    page_entry_t *table = page_get_table(vma);
    return table[page_get_table_idx(vma)] & ~(uintptr_t)0xfff;
}

// Return flags of page table entry for vma
uintptr_t page_get_flags(uintptr_t vma)
{
    return page_get_entry(vma) & 0xfff;
}

// TODO handle error conditions, e.g. no more physical pages
// Return PMA of new page from the free page list or at the end of page heap
uintptr_t page_new(void)
{
    uintptr_t pma = 0;  // PMA of new page

    // Check the free list
    if (page_free_list) {
        pma = page_free_list->pma;
        void *old_node = page_free_list;
        page_free_list = page_free_list->next;
        kfree(old_node);
    } else {
        // If nothing is free, add a new page at kernel_heap_end_pma
        pma = kernel_heap_end_pma;
        kernel_heap_end_pma += PAGE_SIZE;
    }
    return pma;
}

bool page_is_present(uintptr_t vma)
{
    return (bool)(page_get_entry(vma) & PAGE_PRESENT);
}

// Unmap page and add its PMA to the free list
void page_free(uintptr_t vma)
{
    page_free_node_t *new_node = kmalloc(sizeof(page_free_node_t));

    *new_node = (page_free_node_t){
        .pma = page_get_pma(vma),
        .next = page_free_list,
    };

    page_unmap(vma);
    page_free_list = new_node;
}

void page_init_cleanup(void)
{
    kmalloc_init(kernel_heap_end_vma);

    // Zero out and free all init pages (except for init_page_dir and
    // init_page_table_lookup)
    uintptr_t i;
    for (i = INIT_START; i < (uintptr_t)&init_page_struct; i += PAGE_SIZE) {
        page_clear(i);
        page_free(i);
    }

    // Only unmap init_page_struct, don't clear
    for (size_t j = 0; j < sizeof(init_page_struct_t); j += PAGE_SIZE) {
        page_unmap(i);
        i += PAGE_SIZE;
    }

    // Clear and unmap the rest
    for ( ; i < INIT_BSS_END; i += PAGE_SIZE) {
        page_clear(i);
        page_free(i);
    }

    // Allocate all kernel page tables
    for (i = page_get_dir_idx(KERNEL_START_VMA); i < PAGE_ENTRIES; i++) {
        if ( !(page_dir[i] & PAGE_PRESENT) ) {
            page_entry_t *table = kalloc(PAGE_GET_DEFAULT, PAGE_SIZE, PAGE_SIZE);
            page_table_lookup[i] = (uintptr_t)table;
            page_dir[i] = page_get_pma((uintptr_t)table) | PAGE_PRESENT | PAGE_WRITE;
        }
    }
}
