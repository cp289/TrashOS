/**
 * _print.c: general printing routines
 *
 * This file is included in other units needing printing capability
 */

#include "std.h"
#include "math.h"
#include "vga.h"

#ifndef VGA_PUTC
#define VGA_PUTC vga_putc
#endif

/**
 * TODO ideally we put characters to a general buffer instead of relying on VGA.
 *   1) Write stream/buffer protocol (look at C++ streambuf protocol?)
 *   2) Write kfprintf(buffer, fmt, ...) and write kprintf_init to set default
 *      buffer for kprintf()
 */

// 4-bit character table for nibbles
static const char hex[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'a', 'b', 'c', 'd', 'e', 'f'};
static const char Hex[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'A', 'B', 'C', 'D', 'E', 'F'};

enum {
    FORMAT_INVALID = 0,
    FORMAT_NONE,
    // Intermediate states
    FORMAT_SHORT,
    FORMAT_SHORT_SHORT,
    FORMAT_LONG,
    FORMAT_LONG_LONG,
    // Signed int formats
    FORMAT_SHORT_SHORT_INT,
    FORMAT_SHORT_INT,
    FORMAT_INT,
    FORMAT_LONG_INT,
    FORMAT_LONG_LONG_INT,
    // Unsigned int formats
    FORMAT_SHORT_SHORT_UINT,
    FORMAT_SHORT_UINT,
    FORMAT_UINT,
    FORMAT_LONG_UINT,
    FORMAT_LONG_LONG_UINT,
    // Pointer formats
    FORMAT_SHORT_SHORT_PTR,
    FORMAT_SHORT_PTR,
    FORMAT_PTR,
    FORMAT_LONG_PTR,
    FORMAT_LONG_LONG_PTR,
    // Oct formats
    FORMAT_SHORT_SHORT_OCT,
    FORMAT_SHORT_OCT,
    FORMAT_OCT,
    FORMAT_LONG_OCT,
    FORMAT_LONG_LONG_OCT,
    // Hex formats
    FORMAT_SHORT_SHORT_HEX,
    FORMAT_SHORT_HEX,
    FORMAT_HEX,
    FORMAT_LONG_HEX,
    FORMAT_LONG_LONG_HEX,
    FORMAT_SHORT_SHORT_BIG_HEX,
    FORMAT_SHORT_BIG_HEX,
    FORMAT_BIG_HEX,
    FORMAT_LONG_BIG_HEX,
    FORMAT_LONG_LONG_BIG_HEX,
    // String formats
    FORMAT_STR,
    FORMAT_ESCAPED_PERCENT,
};

static int _parse_format(const char *format, size_t i)
{
    int format_state = FORMAT_NONE;
    bool reading_format = true;
    while (reading_format) {
        switch (format_state)
        {
        case FORMAT_NONE:
            switch (format[i])
            {
            case 'h':
                format_state = FORMAT_SHORT;
                break;
            case 'l':
                format_state = FORMAT_LONG;
                break;
            case 'd':
            case 'i':
                format_state = FORMAT_INT;
                reading_format = false;
                break;
            case 'u':
                format_state = FORMAT_UINT;
                reading_format = false;
                break;
            case 'p':
                format_state = FORMAT_PTR;
                reading_format = false;
                break;
            case 'o':
                format_state = FORMAT_OCT;
                reading_format = false;
                break;
            case 'x':
                format_state = FORMAT_HEX;
                reading_format = false;
                break;
            case 'X':
                format_state = FORMAT_BIG_HEX;
                reading_format = false;
                break;
            case 's':
                format_state = FORMAT_STR;
                reading_format = false;
                break;
            case '%':
                format_state = FORMAT_ESCAPED_PERCENT;
                reading_format = false;
                break;
            default:
                format_state = FORMAT_INVALID;
                reading_format = false;
            }
            break;

        case FORMAT_SHORT:
            switch (format[i])
            {
            case 'h':
                format_state = FORMAT_SHORT_SHORT;
                break;
            case 'd':
            case 'i':
                format_state = FORMAT_SHORT_INT;
                reading_format = false;
                break;
            case 'u':
                format_state = FORMAT_SHORT_UINT;
                reading_format = false;
                break;
            case 'p':
                format_state = FORMAT_SHORT_PTR;
                reading_format = false;
                break;
            case 'o':
                format_state = FORMAT_SHORT_OCT;
                reading_format = false;
                break;
            case 'x':
                format_state = FORMAT_SHORT_HEX;
                reading_format = false;
                break;
            case 'X':
                format_state = FORMAT_SHORT_BIG_HEX;
                reading_format = false;
                break;
            default:
                format_state = FORMAT_INVALID;
                reading_format = false;
            }
            break;

        case FORMAT_SHORT_SHORT:
            switch (format[i])
            {
            case 'd':
            case 'i':
                format_state = FORMAT_SHORT_SHORT_INT;
                break;
            case 'u':
                format_state = FORMAT_SHORT_SHORT_UINT;
                break;
            case 'p':
                format_state = FORMAT_SHORT_SHORT_PTR;
                break;
            case 'o':
                format_state = FORMAT_SHORT_SHORT_OCT;
                reading_format = false;
                break;
            case 'x':
                format_state = FORMAT_SHORT_SHORT_HEX;
                break;
            case 'X':
                format_state = FORMAT_SHORT_SHORT_BIG_HEX;
                reading_format = false;
                break;
            default:
                format_state = FORMAT_INVALID;
            }
            reading_format = false;
            break;

        case FORMAT_LONG:
            switch (format[i])
            {
            case 'l':
                format_state = FORMAT_LONG_LONG;
                break;
            case 'd':
            case 'i':
                format_state = FORMAT_LONG_INT;
                reading_format = false;
                break;
            case 'u':
                format_state = FORMAT_LONG_UINT;
                reading_format = false;
                break;
            case 'p':
                format_state = FORMAT_LONG_PTR;
                reading_format = false;
                break;
            case 'o':
                format_state = FORMAT_LONG_OCT;
                reading_format = false;
                break;
            case 'x':
                format_state = FORMAT_LONG_HEX;
                reading_format = false;
                break;
            case 'X':
                format_state = FORMAT_LONG_BIG_HEX;
                reading_format = false;
                break;
            default:
                format_state = FORMAT_INVALID;
                reading_format = false;
            }
            break;

        case FORMAT_LONG_LONG:
            switch (format[i])
            {
            case 'd':
            case 'i':
                format_state = FORMAT_LONG_LONG_INT;
                break;
            case 'u':
                format_state = FORMAT_LONG_LONG_UINT;
                break;
            case 'p':
                format_state = FORMAT_LONG_LONG_PTR;
                break;
            case 'o':
                format_state = FORMAT_LONG_LONG_OCT;
                break;
            case 'x':
                format_state = FORMAT_LONG_LONG_HEX;
                break;
            case 'X':
                format_state = FORMAT_LONG_LONG_BIG_HEX;
                reading_format = false;
                break;
            default:
                format_state = FORMAT_INVALID;
            }
            reading_format = false;
            break;
        }
        i++;
    }
    return format_state;
}

static ssize_t _print_dec_unsigned(void *num_addr, size_t num_size)
{
    ssize_t bytes = 0;
    uint64_t num = (uint64_t)0;
    for (size_t i = 0; i < num_size; i++) {
        num |= (uint64_t)((uint8_t*)num_addr)[i] << i * BITS_PER_BYTE;
    }

    int j = ilog10u(num);
    for (bytes += j + 1; j >= 0; j--) {
        int digit = div64(num, EXP10[j]);
        VGA_PUTC(hex[digit]);
        num -= digit * EXP10[j];
    }
    return bytes;
}

static ssize_t _print_oct(void *num_addr, size_t num_size)
{
    ssize_t bytes = 0;
    uint64_t num = (uint64_t)0;
    for (size_t i = 0; i < num_size; i++) {
        num |= (uint64_t)((uint8_t*)num_addr)[i] << i * BITS_PER_BYTE;
    }

    for (int j = (ilog2u(num) / 3) * 3; j >= 0; j -= 3) {
        VGA_PUTC(hex[num >> j & 0x7]);
        bytes++;
    }
    return bytes;
}

static ssize_t _print_hex(void *num_addr, size_t num_size)
{
    ssize_t bytes = 0;
    uint64_t num = (uint64_t)0;
    for (size_t i = 0; i < num_size; i++) {
        num |= (uint64_t)((uint8_t*)num_addr)[i] << i * BITS_PER_BYTE;
    }

    int j = ilog2u(num) & ~0x3;
    for (bytes += j >> 2; j >= 0; j -= 4) {
        VGA_PUTC(hex[num >> j & 0xf]);
    }
    bytes++;

    return bytes;
}

static ssize_t _print_Hex(void *num_addr, size_t num_size)
{
    ssize_t bytes = 0;
    uint64_t num = (uint64_t)0;
    for (size_t i = 0; i < num_size; i++) {
        num |= (uint64_t)((uint8_t*)num_addr)[i] << i * BITS_PER_BYTE;
    }

    int j = ilog2u(num) & ~0x3;
    for (bytes += j >> 2; j >= 0; j -= 4) {
        VGA_PUTC(Hex[num >> j & 0xf]);
    }
    bytes++;

    return bytes;
}

static ssize_t _print_ptr(void *num_addr, size_t num_size)
{
    ssize_t bytes = 0;
    uint64_t num = (uint64_t)0;
    for (size_t i = 0; i < num_size; i++) {
        num |= (uint64_t)((uint8_t*)num_addr)[i] << i * BITS_PER_BYTE;
    }

    VGA_PUTC('0');
    VGA_PUTC('x');
    for (int j = num_size * BITS_PER_BYTE - 4; j >= 0; j -= 4) {
        VGA_PUTC(hex[num >> j & 0xf]);
    }
    bytes += 2 + (sizeof(num) << 1);

    return bytes;
}

static ssize_t _print_dec_signed(void *num_addr, size_t num_size)
{
    ssize_t bytes = 0;
    int64_t num = (int64_t)0;
    for (size_t i = 0; i < num_size; i++) {
        num |= (uint64_t)((uint8_t*)num_addr)[i] << i * BITS_PER_BYTE;
    }

    // Handle negative case
    if (num & (1 << (num_size * BITS_PER_BYTE - 1))) {
        // Sign extend
        num |= num_size == sizeof(int64_t) ? (int64_t)0 :
            (~(uint64_t)0 << (num_size * BITS_PER_BYTE));
        // Flip sign
        num = -num;
        VGA_PUTC('-');
        bytes++;
    }

    int j = ilog10u((uint64_t)num);
    for (bytes += j + 1; j >= 0; j--) {
        VGA_PUTC(hex[div64(num, EXP10[j])]);
        num -= div64(num, EXP10[j]) * EXP10[j];
    }

    return bytes;
}

/**
 * TODO support printing floats and doubles
 * TODO handle error conditions and return negative values
 * Read format string (starting at offset *i) and va_list argument and print the
 * appropriate characters
 * RETURN
 *  number of bytes written
 */
static ssize_t _print_format(const char *format, size_t *i, va_list *arg)
{
    union {
        int8_t      hhi;
        uint8_t     hhu;
        int16_t     hi;
        uint16_t    hu;
        int32_t     i;
        uint32_t    u;
        int64_t     li;
        uint64_t    lu;
        int64_t     lli;
        uint64_t    llu;
        const char *s;
    } val;

    ssize_t bytes = 0;  // Number of bytes written

    switch (_parse_format(format, *i))
    {
        case FORMAT_SHORT_SHORT_INT:
            val.hhi = (int8_t)va_arg(*arg, int);
            bytes += _print_dec_signed(&val.hhi, sizeof(val.hhi));
            *i += 2;
            break;

        case FORMAT_SHORT_INT:
            val.hi = (int16_t)va_arg(*arg, int);
            bytes += _print_dec_signed(&val.hi, sizeof(val.hi));
            *i += 1;
            break;

        case FORMAT_INT:
            val.i = va_arg(*arg, int32_t);
            bytes += _print_dec_signed(&val.i, sizeof(val.i));
            break;

        case FORMAT_LONG_INT:
            val.li = va_arg(*arg, int64_t);
            bytes += _print_dec_signed(&val.li, sizeof(val.li));
            *i += 1;
            break;

        case FORMAT_LONG_LONG_INT:
            val.lli = va_arg(*arg, int64_t);
            bytes += _print_dec_signed(&val.lli, sizeof(val.lli));
            *i += 2;
            break;

        case FORMAT_SHORT_SHORT_UINT:
            val.hhu = (uint8_t)va_arg(*arg, int);
            bytes += _print_dec_unsigned(&val.hhu, sizeof(val.hhu));
            *i += 2;
            break;

        case FORMAT_SHORT_UINT:
            val.hu = (uint16_t)va_arg(*arg, int);
            bytes += _print_dec_unsigned(&val.hu, sizeof(val.hu));
            *i += 1;
            break;

        case FORMAT_UINT:
            val.u = va_arg(*arg, unsigned int);
            bytes += _print_dec_unsigned(&val.u, sizeof(val.u));
            break;

        case FORMAT_LONG_UINT:
            val.lu = va_arg(*arg, uint64_t);
            bytes += _print_dec_unsigned(&val.lu, sizeof(val.lu));
            *i += 1;
            break;

        case FORMAT_LONG_LONG_UINT:
            val.llu = va_arg(*arg, uint64_t);
            bytes += _print_dec_unsigned(&val.llu, sizeof(val.llu));
            *i += 2;
            break;

        case FORMAT_SHORT_SHORT_OCT:
            val.hhu = (uint8_t)va_arg(*arg, unsigned int);
            bytes += _print_oct(&val.hhu, sizeof(val.hhu));
            *i += 2;
            break;

        case FORMAT_SHORT_OCT:
            val.hu = (uint16_t)va_arg(*arg, unsigned int);
            bytes += _print_oct(&val.hu, sizeof(val.hu));
            *i += 1;
            break;

        case FORMAT_OCT:
            val.u = va_arg(*arg, unsigned int);
            bytes += _print_oct(&val.u, sizeof(val.u));
            break;

        case FORMAT_LONG_OCT:
            val.lu = va_arg(*arg, uint64_t);
            bytes += _print_oct(&val.lu, sizeof(val.lu));
            *i += 1;
            break;

        case FORMAT_LONG_LONG_OCT:
            val.llu = va_arg(*arg, uint64_t);
            bytes += _print_oct(&val.llu, sizeof(val.llu));
            *i += 2;
            break;

        case FORMAT_SHORT_SHORT_HEX:
            val.hhu = (uint8_t)va_arg(*arg, unsigned int);
            bytes += _print_hex(&val.hhu, sizeof(val.hhu));
            *i += 2;
            break;

        case FORMAT_SHORT_HEX:
            val.hu = (uint16_t)va_arg(*arg, unsigned int);
            bytes += _print_hex(&val.hu, sizeof(val.hu));
            *i += 1;
            break;

        case FORMAT_HEX:
            val.u = va_arg(*arg, unsigned int);
            bytes += _print_hex(&val.u, sizeof(val.u));
            break;

        case FORMAT_LONG_HEX:
            val.lu = va_arg(*arg, uint64_t);
            bytes += _print_hex(&val.lu, sizeof(val.lu));
            *i += 1;
            break;

        case FORMAT_LONG_LONG_HEX:
            val.llu = va_arg(*arg, uint64_t);
            bytes += _print_hex(&val.llu, sizeof(val.llu));
            *i += 2;
            break;

        case FORMAT_SHORT_SHORT_BIG_HEX:
            val.hhu = (uint8_t)va_arg(*arg, unsigned int);
            bytes += _print_Hex(&val.hhu, sizeof(val.hhu));
            *i += 2;
            break;

        case FORMAT_SHORT_BIG_HEX:
            val.hu = (uint16_t)va_arg(*arg, unsigned int);
            bytes += _print_Hex(&val.hu, sizeof(val.hu));
            *i += 1;
            break;

        case FORMAT_BIG_HEX:
            val.u = va_arg(*arg, unsigned int);
            bytes += _print_Hex(&val.u, sizeof(val.u));
            break;

        case FORMAT_LONG_BIG_HEX:
            val.lu = va_arg(*arg, uint64_t);
            bytes += _print_Hex(&val.lu, sizeof(val.lu));
            *i += 1;
            break;

        case FORMAT_LONG_LONG_BIG_HEX:
            val.llu = va_arg(*arg, uint64_t);
            bytes += _print_Hex(&val.llu, sizeof(val.llu));
            *i += 2;
            break;

        case FORMAT_PTR:
            val.u = va_arg(*arg, uintptr_t);
            bytes += _print_ptr(&val.u, sizeof(val.u));
            break;

        case FORMAT_LONG_PTR:
            val.llu = va_arg(*arg, uint64_t);
            bytes += _print_ptr(&val.llu, sizeof(val.llu));
            *i += 1;
            break;

        case FORMAT_STR:
            val.s = va_arg(*arg, const char*);
            for ( ; *val.s; val.s++) {
                VGA_PUTC(*val.s);
                bytes++;
            }
            break;

        case FORMAT_ESCAPED_PERCENT:
            VGA_PUTC('%');
            bytes++;
            break;
        default:
            VGA_PUTC('?');
            VGA_PUTC('?');
            VGA_PUTC('?');
            bytes += 3;
    }
    return bytes;
}

static ssize_t _print(const char *format, va_list *arg)
{
    char c;                 // The current character
    ssize_t bytes = 0;      // Total number of bytes written
    bool pattern = false;   // Whether we are parsing a % pattern

    for (size_t i = 0; (c = format[i]); i++) {
        if (pattern) {
            bytes += _print_format(format, &i, arg);
            pattern = false;
        } else {
            if (c == '%') {
                pattern = true;
            } else {
                VGA_PUTC(c);
                bytes++;
            }
        }
    }

    return bytes;
}
