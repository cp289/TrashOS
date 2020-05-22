/**
 * init_printk.c: printk function (init section debugging)
 */

#include "std.h"
#define VGA_PUTC init_vga_putc
#include "_print.c"

ssize_t init_printk(const char *format, ... )
{
    va_list args;
    va_start(args, format);
    ssize_t bytes = _print(format, &args);
    va_end(args);
    return bytes;
}
