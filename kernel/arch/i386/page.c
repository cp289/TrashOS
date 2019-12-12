/**
 * page.c: Paging
 */

#include "page.h"
#include "mem.h"
#include "std.h"

uintptr_t *page_dir;
page_table_t *page_table;

// Map virtual memory address (vma) to page at physical memory address (pma)
void page_map(uintptr_t vma, uintptr_t pma)
{
    uintptr_t entry = (page_table[vma >> 22][vma >> 12 & 0x3ff] & 0xffff) | pma;
    page_table[vma >> 22][vma >> 12 & 0x3ff] = entry;
}

// Return page table entry for vma
uintptr_t page_get_entry(uintptr_t vma)
{
    return page_table[vma >> 22][vma >> 12 & 0x3ff];
}

// Return flags of page table entry for vma
uintptr_t page_get_flags(uintptr_t vma)
{
    return page_get_entry(vma) & 0xfff;
}

// Configure flag bits in page table entry for vma
void page_set_flags(uintptr_t vma, uintptr_t flags)
{
    uintptr_t entry = page_table[vma >> 22][vma >> 12 & 0x3ff];
    entry &= ~(uintptr_t)0 >> 12 << 12;
    entry |= flags;
    page_table[vma >> 22][vma >> 12 & 0x3ff] = entry;
}
