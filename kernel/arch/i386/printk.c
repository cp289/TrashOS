/**
 * printk.c: printk function
 */

#include "math.h"
#include "std.h"
/**
 * TODO ideally we put characters to a general buffer instead of relying on VGA.
 *   1) Write stream/buffer protocol (look at C++ streambuf protocol?)
 *   2) Write kfprintf(buffer, fmt, ...) and write kprintf_init to set default
 *      buffer for kprintf()
 */
#include "vga.h"

// 4-bit character table for nibbles
static const char hex[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'a', 'b', 'c', 'd', 'e', 'f'};
static const char Hex[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'A', 'B', 'C', 'D', 'E', 'F'};

/**
 * TODO support printf size modifiers and other flags
 * TODO support printing floats and doubles
 * Read format string (starting at offset *i) and va_list argument and print the
 * appropriate characters
 * RETURN
 *  number of bytes written
 */
static size_t _print_format(const char *format, size_t *i, va_list *arg)
{
    union {
        int i;
        unsigned int u;
        uintptr_t p;
        const char *s;
    } val;

    size_t bytes = 0;   // Number of bytes written
    int j;              // loop iterator

    switch (format[*i]) {
        case 'd':       // Signed integer (dec)
        case 'i':
            val.i = va_arg(*arg, int);
            if (val.i < 0) {
                val.i = -val.i;
                vga_putc('-');
                bytes += 1;
            }
            for (j = ilog10u(val.u), bytes += j + 1; j >= 0; j--) {
                unsigned int digit = val.u / iEXP10[j];
                vga_putc(hex[digit]);
                val.u -= digit * iEXP10[j];
            }
            break;
        case 'o':       // Unsigned integer (oct)
            val.u = va_arg(*arg, unsigned int);
            for (j = (ilog2u(val.u) / 3) * 3; j >= 0; j -= 3) {
                vga_putc(hex[val.u >> j & 0x7]);
                bytes++;
            }
            break;
        case 'p':       // Pointer
            val.p = va_arg(*arg, uintptr_t);
            vga_putc('0');
            vga_putc('x');
            for (j = (sizeof(val.p) << 3) - 4; j >= 0; j -= 4) {
                vga_putc(hex[val.p >> j & 0xf]);
            }
            bytes += 2 + (sizeof(val.p) << 1);
            break;
        case 's':       // String
            val.s = va_arg(*arg, const char*);
            for ( ; *val.s; val.s++) {
                vga_putc(*val.s);
                bytes++;
            }
            break;
        case 'u':       // Unsigned integer (dec)
            val.u = va_arg(*arg, unsigned int);
            for (j = ilog10u(val.u), bytes += j + 1; j >= 0; j--) {
                unsigned int digit = val.u / iEXP10[j];
                vga_putc(hex[digit]);
                val.u -= digit * iEXP10[j];
            }
            break;
        case 'x':       // Unsigned integer (hex)
            val.u = va_arg(*arg, unsigned int);
            for (j = ilog2u(val.u) & ~0x3, bytes += j >> 2; j >= 0; j -= 4) {
                vga_putc(hex[val.u >> j & 0xf]);
            }
            bytes++;
            break;
        case 'X':       // Unsigned integer (hex, upper case letters)
            val.u = va_arg(*arg, unsigned int);
            for (j = ilog2u(val.u) & ~0x3, bytes += j >> 2; j >= 0; j -= 4) {
                vga_putc(Hex[val.u >> j & 0xf]);
            }
            bytes++;
            break;
        case '%':       // Escaped % character
            vga_putc('%');
            bytes += 1;
            break;
        default:        // Unrecognized pattern
            vga_putc('%');
            vga_putc(format[*i]);
            bytes += 2;
    }
    return bytes;
}

int printk(const char *format, ... )
{
    va_list arg;            // Variable number of arguments
    va_start(arg, format);

    char c;                 // The current format string character
    int bytes = 0;          // Total number of bytes written
    bool pattern = false;   // Whether we are parsing a % pattern

    for (size_t i = 0; (c = format[i]); i++) {
        if (pattern) {
            bytes += _print_format(format, &i, &arg);
            pattern = false;
        } else {
            if (c == '%') {
                pattern = true;
            } else {
                vga_putc(c);
                bytes++;
            }
        }
    }

    va_end(arg);
    return bytes;
}
