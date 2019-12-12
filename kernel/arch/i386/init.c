/**
 * init.c: This file is loaded to the text_init section and simply configures
 * a higher-half kernel virtual address space
 */

#include "page.h"
#include "std.h"
#include "mem.h"

page_dir_t _Alignas(PAGE_SIZE) init_page_dir;
page_table_t _Alignas(PAGE_SIZE) init_page_table[PAGE_ENTRIES];

// Map page at physical memory address (pma) to virtual memory address (vma)
static void init_page_map(uintptr_t vma, uintptr_t pma)
{
    uintptr_t entry = (init_page_table[vma >> 22][vma >> 12 & 0x3ff] & 0xffff) | pma;
    init_page_table[vma >> 22][vma >> 12 & 0x3ff] = entry;
}

static void init_page_set_flags(uintptr_t vma, uintptr_t flags)
{
    uintptr_t entry = init_page_table[vma >> 22][vma >> 12 & 0x3ff];
    entry &= ~(uintptr_t)0 >> 12 << 12;
    entry |= flags;
    init_page_table[vma >> 22][vma >> 12 & 0x3ff] = entry;
}

void init(void)
{
    // Initialize all page directory entries to writable and present
    for (int i = 0; i < PAGE_ENTRIES; i++) {
        uintptr_t entry = (uintptr_t)&init_page_table[i] | PAGE_WRITE | PAGE_PRESENT;
        init_page_dir[i] = entry;
    }

    // Intialize everything to empty, inaccessible pages
    for (int j = 0; j < PAGE_ENTRIES; j++)
        for (int i = 0; i < PAGE_ENTRIES; i++)
            init_page_table[j][i] = PAGE_WRITE;

    // Identity map first MB and init_text section
    for (uintptr_t i = 0; i < KERNEL_LMA + INIT_SIZE; i += PAGE_SIZE) {
        init_page_map(i, i);
        init_page_set_flags(i, PAGE_WRITE | PAGE_PRESENT);
    }

    // Map kernel
    for (uintptr_t i = 0; i < KERNEL_SIZE; i += PAGE_SIZE) {
        const uintptr_t pma = KERNEL_LMA + INIT_SIZE + i;
        const uintptr_t vma = KERNEL_VMA + i;
        init_page_map(vma, pma);
        init_page_set_flags(vma, PAGE_WRITE | PAGE_PRESENT);
    }

    // Enable paging
    page_load_dir(&init_page_dir);
    page_enable();
}
