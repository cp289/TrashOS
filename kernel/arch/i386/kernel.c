/**
 * kernel.c: Main kernel file
 */

#include "apic.h"
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
    vga_init();
    // TODO uncomment
    //page_init();
    gdt_init();
    idt_init();
    apic_init();

    // TODO call a scheduline routine that does something like this
    sti();
    while (1)
        halt();

    char id_str[3 * sizeof(reg_t) + 1];
    size_t max_size;

    cpuid_id_string(&max_size, id_str);
    kprintf("kernel_main: CPUID: %s %d\n", id_str, max_size);

    cpuid_version_t ver;
    cpuid_version(&ver);
    kprintf("kernel_main: APIC ID: %d\n", ver.initial_apic_id);

    kprintf("kernel_main: CR0: %p\n", get_cr0());
    kprintf("kernel_main: CR2: %p\n", get_cr2());
    kprintf("kernel_main: CR3: %p\n", get_cr3());
    kprintf("kernel_main: CR4: %p\n", get_cr4());
    kprintf("kernel_main: CS:  %p\n", get_cs());
    kprintf("kernel_main: DS:  %p\n", get_ds());
    kprintf("kernel_main: ES:  %p\n", get_es());
    kprintf("kernel_main: FS:  %p\n", get_fs());
    kprintf("kernel_main: GS:  %p\n", get_gs());
    kprintf("kernel_main: SS:  %p\n", get_ss());

    kprintf("kernel_main: EXIT\n");

    die();
}
