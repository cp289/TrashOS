/**
 * mem.h: Memory constants
 */

#ifndef _MEM_H
#define _MEM_H

#include "std.h"

// Linker script variables
extern char _KERNEL_LMA;
extern char _KERNEL_VMA;    // This can be thought of as _KERNEL_START
extern char _KERNEL_END;
extern char _KERNEL_SIZE;
extern char _INIT_START;
extern char _INIT_END;
extern char _INIT_SIZE;
extern char _INIT_BSS_START;
extern char _INIT_BSS_END;
extern char _INIT_BSS_SIZE;
extern char _TEXT_START;
extern char _TEXT_END;
extern char _TEXT_SIZE;
extern char _RODATA_START;
extern char _RODATA_END;
extern char _RODATA_SIZE;
extern char _DATA_START;
extern char _DATA_END;
extern char _DATA_SIZE;
extern char _BSS_START;
extern char _BSS_END;
extern char _BSS_SIZE;

#define KERNEL_LMA      ((uintptr_t)&_KERNEL_LMA)
#define KERNEL_VMA      ((uintptr_t)&_KERNEL_VMA)
#define KERNEL_END      ((uintptr_t)&_KERNEL_END)
#define KERNEL_SIZE     ((uintptr_t)&_KERNEL_SIZE)
#define INIT_START      ((uintptr_t)&_INIT_START)
#define INIT_END        ((uintptr_t)&_INIT_END)
#define INIT_SIZE       ((uintptr_t)&_INIT_SIZE)
#define INIT_BSS_START  ((uintptr_t)&_INIT_BSS_START)
#define INIT_BSS_END    ((uintptr_t)&_INIT_BSS_END)
#define INIT_BSS_SIZE   ((uintptr_t)&_INIT_BSS_SIZE)
#define TEXT_START      ((uintptr_t)&_TEXT_START)
#define TEXT_END        ((uintptr_t)&_TEXT_END)
#define TEXT_SIZE       ((uintptr_t)&_TEXT_SIZE)
#define RODATA_START    ((uintptr_t)&_RODATA_START)
#define RODATA_END      ((uintptr_t)&_RODATA_END)
#define RODATA_SIZE     ((uintptr_t)&_RODATA_SIZE)
#define DATA_START      ((uintptr_t)&_DATA_START)
#define DATA_END        ((uintptr_t)&_DATA_END)
#define DATA_SIZE       ((uintptr_t)&_DATA_SIZE)
#define BSS_START       ((uintptr_t)&_BSS_START)
#define BSS_END         ((uintptr_t)&_BSS_END)
#define BSS_SIZE        ((uintptr_t)&_BSS_SIZE)

// Init stack location
extern char _INIT_STACK_TOP;    // From boot.s
#define INIT_STACK_TOP  ((uintptr_t)&_INIT_STACK_TOP)

// Align address to byte boundary
static inline uintptr_t align(uintptr_t addr, int byte)
{
    return (addr & ~((uintptr_t)byte - 1)) + byte * ((addr & (byte - 1)) != 0);
}

extern uintptr_t kernel_brk;        // End of kernel heap (copied after paging)

#endif // _MEM_H
