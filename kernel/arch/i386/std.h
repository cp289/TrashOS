/**
 * std.h: Essential standard definitions/libraries
 */

#ifndef _KERNEL_STD_H
#define _KERNEL_STD_H

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

enum {
    BITS_PER_BYTE = 8,
};

// TODO this will only work for 32-bit systems!
typedef int32_t ssize_t;

// Register storage classes
typedef uintptr_t reg_t;
typedef uint16_t reg16_t;
typedef uint32_t reg32_t;
typedef uint64_t reg64_t;

typedef uint64_t gdt_t;

// Time storage class
typedef int64_t time_t;

#endif // _KERNEL_STD_H
