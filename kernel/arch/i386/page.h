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

// Flag bits for paging
#define PAGE_IGNORE         (1 << 8)    // Only for Page Directory
#define PAGE_GLOBAL         (1 << 8)    // Only for Page Table
#define PAGE_LARGE          (1 << 7)    // Only for Page Directory
#define PAGE_DIRTY          (1 << 6)    // Only for Page Table
#define PAGE_ACCESSED       (1 << 5)
#define PAGE_DISABLE_CACHE  (1 << 4)
#define PAGE_WRITE_THROUGH  (1 << 3)
#define PAGE_PUBLIC         (1 << 2)
#define PAGE_WRITE          (1 << 1)
#define PAGE_PRESENT        (1 << 0)

// Type for page entries
typedef uintptr_t page_table_t[1024];
typedef uintptr_t page_dir_t[1024];

void page_init(void);
void page_load_dir(void *page_dir);
void page_enable(void);

#endif // _KERNEL_PAGE_H
