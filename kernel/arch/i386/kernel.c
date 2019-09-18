/**
 * kernel.c: Main kernel file
 */

#include "cr.h"
#include "gdt.h"
#include "interrupt.h"
#include "io.h"
#include "page.h"
#include "register.h"
#include "std.h"
#include "vga.h"

extern void *KERNEL_VMA;

/**
 * Kernel main method
 */
void kernel_main(void)
{
    vga_init();
    idt_init();

    kprintf("CR0: %p\n", get_cr0());
    kprintf("CR2: %p\n", get_cr2());
    kprintf("CR3: %p\n", get_cr3());
    kprintf("CR4: %p\n", get_cr4());
    kprintf("CS: %p\n", dump_cs());
    kprintf("DS: %p\n", dump_ds());
    kprintf("ES: %p\n", dump_es());
    kprintf("FS: %p\n", dump_fs());
    kprintf("GS: %p\n", dump_gs());
    kprintf("SS: %p\n\n", dump_ss());

    //page_init();

    //kprintf("CR0: %p\n", get_cr0());
    //kprintf("CR2: %p\n", get_cr2());
    //kprintf("CR3: %p\n", get_cr3());
    //kprintf("CR4: %p\n", get_cr4());
    //kprintf("CS: %p\n", dump_cs());
    //kprintf("DS: %p\n", dump_ds());
    //kprintf("ES: %p\n", dump_es());
    //kprintf("FS: %p\n", dump_fs());
    //kprintf("GS: %p\n", dump_gs());
    //kprintf("SS: %p\n\n", dump_ss());

    gdt_init();

    kprintf("CR0: %p\n", get_cr0());
    kprintf("CR2: %p\n", get_cr2());
    kprintf("CR3: %p\n", get_cr3());
    kprintf("CR4: %p\n", get_cr4());
    kprintf("CS: %p\n", dump_cs());
    kprintf("DS: %p\n", dump_ds());
    kprintf("ES: %p\n", dump_es());
    kprintf("FS: %p\n", dump_fs());
    kprintf("GS: %p\n", dump_gs());
    kprintf("SS: %p\n", dump_ss());
}
