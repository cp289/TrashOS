/**
 * mem.h: Memory constants
 */

#ifndef _MEM_H
#define _MEM_H

#include "std.h"

// Linker script variables
extern char _KERNEL_LMA;
extern char _KERNEL_VMA;
extern char _KERNEL_SIZE;
extern char _INIT_SIZE;

static const uintptr_t KERNEL_LMA = (uintptr_t)&_KERNEL_LMA;
static const uintptr_t KERNEL_VMA = (uintptr_t)&_KERNEL_VMA;
static const uintptr_t KERNEL_SIZE = (uintptr_t)&_KERNEL_SIZE;
static const uintptr_t INIT_SIZE = (uintptr_t)&_INIT_SIZE;

#endif // _MEM_H
