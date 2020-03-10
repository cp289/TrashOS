/**
 * kernel.c: Main kernel file
 */

#include "apic.h"
#include "asm.h"
#include "cpuid.h"
#include "gdt.h"
#include "interrupt.h"
#include "io.h"
#include "kmalloc.h"
#include "page.h"
#include "std.h"
#include "vga.h"

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
    idt_init();
    page_init_cleanup();
    // TODO remap the LAPIC to fit somewhere in kernel memory space
    //apic_init();

    // TODO call a scheduling routine that does something like this
    //sti();
    //while (1)
    //    halt();

    char id_str[3 * sizeof(reg_t) + 1];
    size_t max_size;

    cpuid_id_string(&max_size, id_str);
    printk("kernel_main: CPUID: %s %d\n", id_str, max_size);

    cpuid_version_t ver;
    cpuid_version(&ver);
    printk("kernel_main: APIC ID: %d\n", ver.initial_apic_id);

    printk("kernel_main: CR0: %p\n", get_cr0());
    printk("kernel_main: CR2: %p\n", get_cr2());
    printk("kernel_main: CR3: %p\n", get_cr3());
    printk("kernel_main: CR4: %p\n", get_cr4());
    printk("kernel_main: CS:  %p\n", get_cs());
    printk("kernel_main: DS:  %p\n", get_ds());
    printk("kernel_main: ES:  %p\n", get_es());
    printk("kernel_main: FS:  %p\n", get_fs());
    printk("kernel_main: GS:  %p\n", get_gs());
    printk("kernel_main: SS:  %p\n", get_ss());

    printk("kernel_main: EXIT\n");

    die();
}
