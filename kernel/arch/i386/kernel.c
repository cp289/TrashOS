/**
 * kernel.c: Main kernel file
 */

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "io.h"
#include "vga.h"

/**
 * Kernel main method
 */
void kernel_main(void)
{
    /**
     * TODO vga text mode display dimensions might be immutable. If this is the
     * case, set 25x80 dimensions as constants in vga.c and declare
     * vga_init(void).
     */
    vga_init(25, 80);

    size_t bytes = kprintf(
        "Hello World\nstring: %s\nhex: %x %X\noct: %o\nunsigned: %u\nsigned: %d %d\npointer: %p\n",
        "Test string", 0xdeadbeef, 0xdeadbeef, 01234567, 12345, -123, 123,
        &kernel_main);
    kprintf("\nPrinted %d bytes\n", bytes);
}
