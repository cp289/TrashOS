/**
 * printk.c: printk function
 */

#include "std.h"
#include "_print.c"

ssize_t printk(const char *format, ... )
{
    va_list args;
    va_start(args, format);
    ssize_t bytes = _print(format, &args);
    va_end(args);
    return bytes;
}
