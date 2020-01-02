/**
 * init.c: This file is loaded to the text_init section and simply configures
 * a higher-half kernel virtual address space
 */

#include "io.h"
#include "page.h"
#include "mem.h"
#include "vga.h"

extern void kernel_main(void);      // Kernel entry point

// Below we make static versions of essential paging routines and data
// structures for the init section. We later discard the generated code and
// remap important data into kernel address space.

page_entry_t _Alignas(PAGE_SIZE) init_page_dir[PAGE_ENTRIES];
// This lookup table hosts the virtual addresses for each page table
static uintptr_t *init_page_table_lookup;

static uintptr_t init_kernel_brk;
static uintptr_t init_brk;               // See description for init_page_map()
static uintptr_t init_vga_buffer_vma;    // Mapped address for vga buffer
static uintptr_t init_page_table_offset; // Offset for new page table

// Erase page (overwrite with zeros)
static void init_page_clear(uintptr_t pma)
{
    uint64_t *entry = (void*)pma;
    for (uintptr_t i = 0; i < PAGE_SIZE / sizeof(*entry); i++) {
        entry[i] = (uint64_t)0;
    }
}

// Dynamically allocate new page on kernel heap
static uintptr_t init_page_new(void)
{
    uintptr_t table = align(init_brk, PAGE_SIZE);
    init_page_clear(table);
    init_brk = table + PAGE_SIZE;
    return table;
}

/**
 * We assume at this point that most page directory entries are initialized to
 * not present. We must therefore dynamically allocate page tables when
 * necessary. For this, we use init_brk to keep track of new physical memory
 * allocations.
 * NOTE: This routine simply maps the address without altering flags
 */
static void init_page_map(uintptr_t vma, uintptr_t pma, uintptr_t flags)
{
    const uintptr_t page_dir_idx = vma >> 22;
    const uintptr_t page_table_idx = vma >> 12 & 0x3ff;

    page_entry_t *table = (void*)(init_page_dir[page_dir_idx] >> 12 << 12);

    // Allocate new page table if necessary
    if (!(init_page_dir[page_dir_idx] & PAGE_PRESENT)) {
        uintptr_t new_table = init_page_new();
        table = (void*)new_table;
        init_page_dir[page_dir_idx] = new_table | PAGE_WRITE | PAGE_PRESENT;
        init_page_table_lookup[page_dir_idx] = init_page_table_offset;
        init_page_table_offset += PAGE_SIZE;
        table[page_table_idx] = pma;
    } else {
        table[page_table_idx] = (table[page_table_idx] & 0xfff) | pma;
    }
    table[page_table_idx] |= flags;
}

// Map kernel into virtual address space
static void init_map_kernel(void)
{
    init_brk = KERNEL_END - KERNEL_VMA + INIT_BSS_END;

    // We start placing new page tables after the kernel BSS section and the
    // page directory
    init_page_table_offset = BSS_END + PAGE_SIZE;

    // Initialize all page directory entries to writable
    for (int i = 0; i < PAGE_ENTRIES; i++) {
        init_page_dir[i] = PAGE_WRITE;
    }

    // Allocate and initialize init_page_table_lookup
    init_page_table_lookup = (void*)init_page_new();

    // Identity map init section
    uintptr_t i;
    for (i = INIT_START; i < INIT_BSS_END; i += PAGE_SIZE) {
        init_page_map(i, i, PAGE_WRITE | PAGE_PRESENT);
    }

    // Map .text and .rodata (read-only)
    for (i = 0; i < RODATA_END - TEXT_START; i += PAGE_SIZE) {
        init_page_map(TEXT_START + i, INIT_BSS_END + i, PAGE_PRESENT);
    }

    // Map .data and .bss (read/write)
    for ( ; i < BSS_END - TEXT_START; i += PAGE_SIZE) {
        init_page_map(TEXT_START + i, INIT_BSS_END + i,
                      PAGE_WRITE | PAGE_PRESENT);
    }

    init_kernel_brk = BSS_END;

    // Map page directory
    init_page_map(init_kernel_brk, (uintptr_t)&init_page_dir,
                  PAGE_WRITE | PAGE_PRESENT);

    init_kernel_brk += PAGE_SIZE;

    /**
     * At this point, all predictable components have been mapped into kernel
     * address space, but we have not accounted for the new dynamically-
     * allocated page tables. We place these into kernel address space too. In
     * doing so, we must make sure that any additionally allocated page tables
     * are also accounted for.
     */
    uintptr_t kernel_end_pma = KERNEL_END - KERNEL_VMA + INIT_BSS_END;
    uintptr_t init_alloc_size = init_brk - kernel_end_pma;

    // Determine if more page tables are needed
    size_t n_new_tables;
    uintptr_t kernel_end_dir_aligned = align(kernel_end_pma,
                                             PAGE_ENTRIES * PAGE_SIZE);
    if (init_brk < kernel_end_dir_aligned) {
        n_new_tables = 0;
    } else {
        n_new_tables = init_brk - kernel_end_dir_aligned;
        n_new_tables /= PAGE_ENTRIES * PAGE_SIZE;
    }

    // Map all dynamically-allocated page tables into kernel space
    for (i = 0; i < init_alloc_size + n_new_tables * PAGE_SIZE;
         i += PAGE_SIZE) {
        init_page_map(init_kernel_brk + i, kernel_end_pma + i,
                      PAGE_WRITE | PAGE_PRESENT);
    }

    init_kernel_brk += i;

    // Map VGA space
    init_vga_buffer_vma = init_kernel_brk;
    for (i = 0;
         i < VGA_WIDTH_DEFAULT * VGA_HEIGHT_DEFAULT * sizeof(vga_entry_t);
         i += PAGE_SIZE) {
        init_page_map(init_kernel_brk + i, (uintptr_t)VGA_PADDR_BUFFER + i,
                      PAGE_WRITE | PAGE_PRESENT);
    }

    init_kernel_brk += i;

    // Lastly, we map a page for the kernel stack
    init_page_map(-(intptr_t)PAGE_SIZE, init_brk, PAGE_WRITE | PAGE_PRESENT);
}

void init(void)
{
    // Configure page directories/tables
    init_map_kernel();

    // Enable paging
    page_load_dir(&init_page_dir);
    page_enable();

    // Point vga_buffer to the mapped location
    vga_map_buffer(init_vga_buffer_vma);
    vga_set_position(init_vga_get_row(), init_vga_get_col());

    // Pass on important references
    kernel_brk = init_kernel_brk;
    page_dir = (void*)&init_page_dir;
    page_table_lookup = (void*)(BSS_END + PAGE_SIZE);

    // Switch to new stack and start kernel
    asm (
        "movl %[sp], %%esp\n\t"
        "movl %[sp], %%ebp\n\t"
        "call kernel_main\n\t"
        : // No outputs
        : [sp] "rm" (-(intptr_t)8)
        : // No (important) clobbers
    );
}
