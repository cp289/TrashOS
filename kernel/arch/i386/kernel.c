/**
 * kernel.c: Main kernel file
 */

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "gdt.h"
#include "io.h"
#include "register.h"
#include "vga.h"

extern void *KERNEL_VMA;

/**
 * Kernel main method
 */
void kernel_main(void)
{
    vga_init();
    uintptr_t val = dump_cs();
    kprintf("Code segment register: %p\n", val);
    gdt_init();
    val = dump_cs();
    kprintf("Code segment register: %p\n", val);
    kprintf("lmao Code segment register: %p\n", val);
}
