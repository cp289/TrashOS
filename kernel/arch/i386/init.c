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

init_page_struct_t _Alignas(PAGE_SIZE) init_page_struct = {0};

static uintptr_t init_page_next_pma;     // PMA for next available page
static uintptr_t init_page_next_vma;     // VMA for next virtual page
static uintptr_t init_vga_buffer_vma;    // VMA for vga buffer

// This counter keeps track of the virtual offset of dynamically-allocated page
// tables. These values must be non-zero and are later used to update
// page_table_lookup with absolute VMAs.
static uintptr_t page_table_vma_offset = PAGE_SIZE;

// Erase page (overwrite with zeros)
static void init_page_clear(uintptr_t pma)
{
    uint64_t *entry = (void*)pma;
    for (uintptr_t i = 0; i < PAGE_SIZE / sizeof(*entry); i++) {
        entry[i] = (uint64_t)0;
    }
}

// Dynamically allocate new page on kernel heap and add entry to lookup table
static uintptr_t init_page_table_new(uintptr_t vma)
{
    const uintptr_t page_dir_idx = vma >> 22;
    uintptr_t table_pma = align(init_page_next_pma, PAGE_SIZE);
    init_page_clear(table_pma);
    init_page_next_pma = table_pma + PAGE_SIZE;
    init_page_struct.page_table_lookup[page_dir_idx] = page_table_vma_offset;
    page_table_vma_offset += PAGE_SIZE;
    return table_pma;
}

/**
 * We assume at this point that most page directory entries are initialized to
 * not present. We must therefore dynamically allocate page tables when
 * necessary. For this, we use init_page_next_pma to keep track of new physical
 * memory allocations.
 * NOTE: This routine simply maps the address without altering flags
 */
static void init_page_map(uintptr_t vma, uintptr_t pma, uintptr_t flags)
{
    const uintptr_t page_dir_idx = vma >> 22;
    const uintptr_t page_table_idx = vma >> 12 & 0x3ff;

    page_entry_t *table = (void*)(init_page_struct.page_dir[page_dir_idx] >> 12 << 12);

    // Allocate new page table if necessary
    if (!(init_page_struct.page_dir[page_dir_idx] & PAGE_PRESENT)) {
        uintptr_t new_table = init_page_table_new(vma);
        table = (void*)new_table;
        init_page_struct.page_dir[page_dir_idx] = new_table |
                                        PAGE_PUBLIC | PAGE_WRITE | PAGE_PRESENT;
        table[page_table_idx] = pma;
    } else {
        table[page_table_idx] = (table[page_table_idx] & 0xfff) | pma;
    }
    table[page_table_idx] |= flags;
}

// Map kernel into virtual address space
static void init_map_kernel(void)
{
    // We start mapping new physical pages after the kernel BSS section and the
    // page directory
    init_page_next_pma = KERNEL_END_VMA - KERNEL_START_VMA + INIT_BSS_END;

    // Identity map init section
    uintptr_t i;
    for (i = INIT_START; i < INIT_BSS_END; i += PAGE_SIZE) {
        init_page_map(i, i, PAGE_WRITE | PAGE_PRESENT);
    }

    // Map .text and .rodata (read-only)
    for (i = 0; i < RODATA_END_VMA - TEXT_START_VMA; i += PAGE_SIZE) {
        init_page_map(TEXT_START_VMA + i, INIT_BSS_END + i, PAGE_PUBLIC | PAGE_PRESENT);
    }

    // Map .data and .bss (read/write)
    for ( ; i < BSS_END_VMA - TEXT_START_VMA; i += PAGE_SIZE) {
        init_page_map(TEXT_START_VMA + i, INIT_BSS_END + i,
                      PAGE_PUBLIC | PAGE_WRITE | PAGE_PRESENT);
    }

    init_page_next_vma = BSS_END_VMA;

    // Map page directory
    init_page_map(init_page_next_vma, (uintptr_t)&init_page_struct.page_dir,
                  PAGE_WRITE | PAGE_PRESENT);

    init_page_next_vma += PAGE_SIZE;

    // Map page table lookup table
    init_page_map(init_page_next_vma, (uintptr_t)&init_page_struct.page_table_lookup,
                  PAGE_WRITE | PAGE_PRESENT);

    init_page_next_vma += PAGE_SIZE;

    // Map stack page (doesn't affect init_page_next_vma)
    init_page_map((uintptr_t)0 - PAGE_SIZE,
                  (uintptr_t)&init_page_struct.stack_page,
                  PAGE_WRITE | PAGE_PRESENT);

    // Map VGA space
    init_vga_buffer_vma = init_page_next_vma;
    for (i = 0;
         i < VGA_WIDTH_DEFAULT * VGA_HEIGHT_DEFAULT * sizeof(vga_entry_t);
         i += PAGE_SIZE) {
        init_page_map(init_page_next_vma + i, (uintptr_t)VGA_PADDR_BUFFER + i,
                      PAGE_PUBLIC | PAGE_WRITE | PAGE_PRESENT);
    }

    init_page_next_vma += i;

    /**
     * At this point, all predictable components have been mapped into kernel
     * address space, but we have not accounted for the new dynamically-
     * allocated page tables. We place these into kernel address space too. In
     * doing so, we must make sure that any additionally allocated page tables
     * are also accounted for.
     */
    const uintptr_t kernel_end_pma = KERNEL_END_VMA - KERNEL_START_VMA + INIT_BSS_END;
    const uintptr_t init_alloc_size = init_page_next_pma - kernel_end_pma;

    // Determine if more page tables are needed
    size_t n_new_tables;
    uintptr_t kernel_end_dir_aligned = align(kernel_end_pma,
                                             PAGE_ENTRIES * PAGE_SIZE);
    if (init_page_next_pma < kernel_end_dir_aligned) {
        n_new_tables = 0;
    } else {
        n_new_tables = init_page_next_pma - kernel_end_dir_aligned;
        n_new_tables /= PAGE_ENTRIES * PAGE_SIZE;
    }

    // Map all dynamically-allocated page tables into kernel space
    for (i = 0; i < init_alloc_size + n_new_tables * PAGE_SIZE;
         i += PAGE_SIZE) {
        init_page_map(init_page_next_vma + i, kernel_end_pma + i,
                      PAGE_WRITE | PAGE_PRESENT);
    }

    // Lastly, init_page_next_vma is the start of VMAs for dynamic page tables. We
    // update page_table_lookup to absolute addresses.
    for (i = 0; i < PAGE_ENTRIES; i++) {
        if(init_page_struct.page_table_lookup[i]) {
            // NOTE: Since init_page_table_offset started at PAGE_SIZE for the
            // non-zero check, we must subtract PAGE_SIZE here.
            init_page_struct.page_table_lookup[i] += init_page_next_vma - PAGE_SIZE;
        }
    }

    init_page_next_vma += init_alloc_size + n_new_tables * PAGE_SIZE;
}

void init(void)
{
    // Configure page directories/tables
    init_map_kernel();

    // Enable paging
    page_load_dir(&init_page_struct.page_dir);
    page_enable();

    // Point vga_buffer to the mapped location
    vga_map_buffer(init_vga_buffer_vma);
    vga_set_position(init_vga_get_row(), init_vga_get_col());

    // Pass on important references
    kernel_heap_end_vma = init_page_next_vma;
    kernel_heap_end_pma = init_page_next_pma;
    page_dir = (void*)BSS_END_VMA;
    page_table_lookup = (void*)(BSS_END_VMA + PAGE_SIZE);

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
