/**
 * std.h: Essential standard definitions/libraries
 */

#ifndef _KERNEL_STD_H
#define _KERNEL_STD_H

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * TODO we make the assumption here that register size is determined by the
 * architecture-defined pointer size. This probably works for x86 and x86_64,
 * but is there a better storage class to use (instead of `uintptr_t`)?
 */
typedef uintptr_t reg_t;

#endif // _KERNEL_STD_H
