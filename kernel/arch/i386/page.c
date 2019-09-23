/**
 * page.c: Paging
 */

#include "page.h"
#include "std.h"

//static page_t new_page(void *address, uint32_t flags)
//{
//    page_t entry = (uintptr_t)address;
//    entry |= flags & 0xfff;
//    return entry;
//}

static page_dir_t _Alignas(PAGE_SIZE) kernel_page_dir;
static page_table_t _Alignas(PAGE_SIZE) kernel_page_table;

void page_init(void)
{
    // Initialize all page directory entries to writable and not present
    for (int i = 0; i < 1024; i++) {
        kernel_page_dir[i] = PAGE_WRITE;
    }

    kernel_page_dir[0] |= (uintptr_t)&kernel_page_table | PAGE_PRESENT;

    for (int i = 0; i < 1024; i++) {
        kernel_page_table[0] = (i << 12) | PAGE_WRITE | PAGE_PRESENT;
    }

    page_load_dir(&kernel_page_dir);
    page_enable();
}