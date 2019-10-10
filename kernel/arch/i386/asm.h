/**
 * asm.h: useful assembly routines
 *
 * TODO we make the assumption here that register size is determined by the
 * architecture-defined pointer size. This probably works for x86 and x86_64,
 * but is there a better storage class to use (instead of `uintptr_t`)?
 */

#ifndef _KERNEL_ASM_H
#define _KERNEL_ASM_H

#include "std.h"

static inline uintptr_t get_cr0(void)
{
    uintptr_t val;
    asm (
        "movl %%cr0, %[val]\n\t"
        : [val] "=rm" (val)
        : // No inputs
        : // No clobbers
    );
    return val;
}

// NOTE: CR1 is reserved and any access throws an exception
static inline uintptr_t get_cr1(void)
{
    uintptr_t val;
    asm (
        "movl %%cr1, %[val]\n\t"
        : [val] "=rm" (val)
        : // No inputs
        : // No clobbers
    );
    return val;
}

static inline uintptr_t get_cr2(void)
{
    uintptr_t val;
    asm (
        "movl %%cr2, %[val]\n\t"
        : [val] "=rm" (val)
        : // No inputs
        : // No clobbers
    );
    return val;
}

static inline uintptr_t get_cr3(void)
{
    uintptr_t val;
    asm (
        "movl %%cr3, %[val]\n\t"
        : [val] "=rm" (val)
        : // No inputs
        : // No clobbers
    );
    return val;
}

static inline uintptr_t get_cr4(void)
{
    uintptr_t val;
    asm (
        "movl %%cr4, %[val]\n\t"
        : [val] "=rm" (val)
        : // No inputs
        : // No clobbers
    );
    return val;
}

// TODO fix this
static inline uintptr_t get_eflags(void)
{
    return 0;
}

// Segment registers

static inline uintptr_t get_cs(void)
{
    uintptr_t val;
    asm (
        "movl %%cs, %[val]\n\t"
        : [val] "=rm" (val)
        : // No inputs
        : // No clobbers
    );
    return val;
}

static inline uintptr_t get_ds(void)
{
    uintptr_t val;
    asm (
        "movl %%ds, %[val]\n\t"
        : [val] "=rm" (val)
        : // No inputs
        : // No clobbers
    );
    return val;
}

static inline uintptr_t get_es(void)
{
    uintptr_t val;
    asm (
        "movl %%es, %[val]\n\t"
        : [val] "=rm" (val)
        : // No inputs
        : // No clobbers
    );
    return val;
}

static inline uintptr_t get_fs(void)
{
    uintptr_t val;
    asm (
        "movl %%fs, %[val]\n\t"
        : [val] "=rm" (val)
        : // No inputs
        : // No clobbers
    );
    return val;
}

static inline uintptr_t get_gs(void)
{
    uintptr_t val;
    asm (
        "movl %%gs, %[val]\n\t"
        : [val] "=rm" (val)
        : // No inputs
        : // No clobbers
    );
    return val;
}

static inline uintptr_t get_ss(void)
{
    uintptr_t val;
    asm (
        "movl %%ss, %[val]\n\t"
        : [val] "=rm" (val)
        : // No inputs
        : // No clobbers
    );
    return val;
}

#endif // _KERNEL_ASM_H
