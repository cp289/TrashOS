/**
 * kernel.c: Main kernel file
 */

#include "asm.h"
#include "cpuid.h"
#include "gdt.h"
#include "interrupt.h"
#include "io.h"
#include "page.h"
#include "std.h"
#include "vga.h"

// Linker script variable
extern void *KERNEL_VMA;

/**
 * Kernel main method
 */
void kernel_main(void)
{
    /**
     * NOTE: Interrupt gates require referencing memory descriptors, so the GDT
     * must be configured and loaded before the IDT
     */
    gdt_init();
    vga_init();
    idt_init();
    page_init();

    //volatile int x = 0;
    //kprintf("Division by zero: %d\n", 2/x);

    char id_str[3 * sizeof(reg_t) + 1];
    cpuid_id_string(id_str);

    kprintf("CPUID: %s\n", id_str);

    kprintf("CR0: %p\n", get_cr0());
    kprintf("CR2: %p\n", get_cr2());
    kprintf("CR3: %p\n", get_cr3());
    kprintf("CR4: %p\n", get_cr4());
    kprintf("CS:  %p\n", get_cs());
    kprintf("DS:  %p\n", get_ds());
    kprintf("ES:  %p\n", get_es());
    kprintf("FS:  %p\n", get_fs());
    kprintf("GS:  %p\n", get_gs());
    kprintf("SS:  %p\n", get_ss());
}
