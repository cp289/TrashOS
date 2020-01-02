/**
 * page.h: Paging
 */

#ifndef _KERNEL_PAGE_H
#define _KERNEL_PAGE_H

#include "std.h"

/**
 * Page directory entry: the topmost paging structure
 *
 * bits 31:12   Address of Page Table (4K aligned)
 * bits 11: 9   Available for OS use
 * bit      8   Ignored: whether page is ignored
 * bit      7   Page size:
 *                  0:  4KiB page size
 *                  1:  4MiB page size (also no page table involved)
 * bit      6   0?
 * bit      5   Accessed: whether page was read/written to (not cleared by CPU)
 * bit      4   Cache Disabled: whether caching is disabled for this page
 * bit      3   Write-through:
 *                  0:  write-back caching enabled
 *                  1:  write-through caching enabled
 * bit      2   User/supervisor:
 *                  0:  Page may only be accessed by supervisor
 *                  1:  Page may be accessed by all privilege levels
 * bit      1   Read/write:
 *                  0:  Page is read-only
 *                  1:  Page is read/write
 * bit      0   Present: whether the page is in physical memory (or whether to
 *              not cause a page fault)
 */

/**
 * Page table entry:
 *
 * bits 31:12   Address of Page Table (4K aligned)
 * bits 11: 9   Available for OS use
 * bit      8   Global:
 *                  1:  Prevents TLB from updating address in cache if CR3 is
 *                      reset (requires page global enable bit to be set in CR4)
 * bit      7   0?
 * bit      6   Dirty:
 *                  0:  Page has not been written to (can be set by OS)
 *                  1:  Page has been written to (set by CPU)
 * bit      5   Accessed: whether page was read/written to (not cleared by CPU)
 * bit      4   Cache Disabled: whether caching is disabled for this page
 * bit      3   Write-through:
 *                  0:  write-back caching enabled
 *                  1:  write-through caching enabled
 * bit      2   User/supervisor:
 *                  0:  Page may only be accessed by supervisor
 *                  1:  Page may be accessed by all privilege levels
 * bit      1   Read/write:
 *                  0:  Page is read-only
 *                  1:  Page is read/write
 * bit      0   Present: whether the page is in physical memory (or whether to
 *              not cause a page fault)
 */

// Constants
#define PAGE_SIZE           4096
#define PAGE_ENTRIES        1024

// Flag bits for paging
#define PAGE_IGNORE         ((uintptr_t)1 << 8) // Only for Page Directory
#define PAGE_GLOBAL         ((uintptr_t)1 << 8) // Only for Page Table
#define PAGE_LARGE          ((uintptr_t)1 << 7) // Only for Page Directory
#define PAGE_DIRTY          ((uintptr_t)1 << 6) // Only for Page Table
#define PAGE_ACCESSED       ((uintptr_t)1 << 5)
#define PAGE_DISABLE_CACHE  ((uintptr_t)1 << 4)
#define PAGE_WRITE_THROUGH  ((uintptr_t)1 << 3)
#define PAGE_PUBLIC         ((uintptr_t)1 << 2)
#define PAGE_WRITE          ((uintptr_t)1 << 1)
#define PAGE_PRESENT        ((uintptr_t)1 << 0)

// Type for page entries
typedef uintptr_t page_entry_t;

extern page_entry_t init_page_dir[PAGE_ENTRIES];
extern page_entry_t *page_dir;
extern uintptr_t *page_table_lookup;

static inline void page_load_dir(void *page_dir)
{
    asm (
        "movl   %0,  %%cr3\n\t"
        : // No outputs
        : "r" (page_dir)
        : // No clobbers
    );
}

static inline void page_enable(void)
{
    asm (
        "movl   %%cr0,  %%eax\n\t"
        "orl    %0,     %%eax\n\t"
        "movl   %%eax,  %%cr0\n\t"
        : // No outputs
        : "i" (0x80000001)
        : "eax"
    );
}

void page_clear(uintptr_t pma);
void page_init_cleanup(void);
uintptr_t page_get_entry(uintptr_t vma);
uintptr_t page_get_flags(uintptr_t vma);
void page_map(uintptr_t vma, uintptr_t pma);
void page_set_flags(uintptr_t vma, uintptr_t flags);
void page_unmap(uintptr_t vma);

#endif // _KERNEL_PAGE_H
