/**
 * page.c: Paging
 */

#include "page.h"
#include "mem.h"
#include "std.h"

uintptr_t *page_dir;
uintptr_t *page_table_lookup;
uintptr_t kernel_brk;
uintptr_t kernel_vbrk;

// Erase page (overwrite with zeros)
void page_clear(uintptr_t vma)
{
    uint64_t *entry = (void*)vma;
    for (uintptr_t i = 0; i < PAGE_SIZE / sizeof(*entry); i++) {
        entry[i] = (uint64_t)0;
    }
}

// Dynamically allocate new page table
static uintptr_t new_page_table(void)
{
    uintptr_t table = align(kernel_brk, PAGE_SIZE);
    page_clear(table);
    kernel_brk = table + PAGE_SIZE;
    return table;
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
    return (void*)page_table_lookup[page_get_dir_idx(vma)];
}

// Map virtual memory address (vma) to page at physical memory address (pma)
void page_map(uintptr_t vma, uintptr_t pma)
{
    const uintptr_t page_dir_idx = page_get_dir_idx(vma);
    const uintptr_t page_table_idx = page_get_table_idx(vma);

    page_entry_t *table = page_get_table(vma);

    // Allocate new page table if necessary
    if (!(page_dir[page_dir_idx] & PAGE_PRESENT)) {
        uintptr_t new_table = new_page_table();
        table = (void*)new_table;
        page_dir[page_dir_idx] = new_table | PAGE_WRITE | PAGE_PRESENT;
        table[page_table_idx] = pma;
    } else {
        table[page_table_idx] = (table[page_table_idx] & 0xfff) | pma;
    }
}

// Unmap page by zeroing its page table entry
void page_unmap(uintptr_t vma)
{
    page_entry_t *table = page_get_table(vma);
    table[page_get_table_idx(vma)] = (page_entry_t)0;
}

void page_set_flags(uintptr_t vma, uintptr_t flags)
{
    const uintptr_t page_table_idx = vma >> 12 & 0x3ff;
    page_entry_t *table = page_get_table(vma);
    table[page_table_idx] = (table[page_table_idx] >> 12 << 12) | flags;
}

// Return page table entry for vma
uintptr_t page_get_entry(uintptr_t vma)
{
    page_entry_t *table = page_get_table(vma);
    return table[page_get_table_idx(vma)];
}

// Return flags of page table entry for vma
uintptr_t page_get_flags(uintptr_t vma)
{
    return page_get_entry(vma) & 0xfff;
}

void page_init_cleanup(void)
{
    // Zero out and unmap all init pages (except for init_page_dir)
    uintptr_t i;
    for (i = INIT_START; i < (uintptr_t)&init_page_dir; i += PAGE_SIZE) {
        page_clear(i);
        page_unmap(i);
    }
    page_unmap(i);  // Only unmap init_page_dir, don't clear
    for (i += PAGE_SIZE; i < INIT_BSS_END; i += PAGE_SIZE) {
        page_clear(i);
        page_unmap(i);
    }
}
