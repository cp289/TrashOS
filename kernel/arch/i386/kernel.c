/**
 * kernel.c: Main kernel file
 */

#include "alloc.h"
#include "apic.h"
#include "asm.h"
#include "cpuid.h"
#include "gdt.h"
#include "interrupt.h"
#include "io.h"
#include "page.h"
#include "proc.h"
#include "std.h"
#include "vga.h"

// Run unit tests
// TODO generate / return return error codes
static void kernel_test(void)
{
    kmalloc_test();
}

/**
 * Kernel main method
 */
void kernel_main(void)
{
    (void)kernel_test;
    /**
     * NOTE: Interrupt gates require referencing memory descriptors, so the GDT
     * must be configured and loaded before the IDT
     */
    vga_clear();
    gdt_init();
    idt_init();
    page_init_cleanup();
    apic_init();
    proc_init();
    proc_loop();

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
