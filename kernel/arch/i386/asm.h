/**
 * asm.h: useful assembly routines
 */

#ifndef _KERNEL_ASM_H
#define _KERNEL_ASM_H

#include "std.h"

static inline void die(void)
{
    // Place system into infinite loop
    asm (
        // Disable interrupts
        "cli\n"
        // Wait for next interrupt (there are none; this locks the system)
    "1:  hlt\n\t"
        // Jump to halt if non-maskable interrupt or system management mode occurs
        "jmp 1b\n\t"
        "hlt\n\t"
        : // No outputs
        : // No inputs
        : // No clobbers
    );
}

static inline reg_t get_cr0(void)
{
    reg_t val;
    asm (
        "movl %%cr0, %[val]\n\t"
        : [val] "=rm" (val)
        : // No inputs
        : // No clobbers
    );
    return val;
}

// NOTE: CR1 is reserved and any access throws an exception
static inline reg_t get_cr1(void)
{
    reg_t val;
    asm (
        "movl %%cr1, %[val]\n\t"
        : [val] "=rm" (val)
        : // No inputs
        : // No clobbers
    );
    return val;
}

static inline reg_t get_cr2(void)
{
    reg_t val;
    asm (
        "movl %%cr2, %[val]\n\t"
        : [val] "=rm" (val)
        : // No inputs
        : // No clobbers
    );
    return val;
}

static inline reg_t get_cr3(void)
{
    reg_t val;
    asm (
        "movl %%cr3, %[val]\n\t"
        : [val] "=rm" (val)
        : // No inputs
        : // No clobbers
    );
    return val;
}

static inline reg_t get_cr4(void)
{
    reg_t val;
    asm (
        "movl %%cr4, %[val]\n\t"
        : [val] "=rm" (val)
        : // No inputs
        : // No clobbers
    );
    return val;
}

// TODO fix this
static inline reg_t get_eflags(void)
{
    return 0;
}

// Segment registers

static inline reg_t get_cs(void)
{
    reg_t val;
    asm (
        "movl %%cs, %[val]\n\t"
        : [val] "=rm" (val)
        : // No inputs
        : // No clobbers
    );
    return val;
}

static inline reg_t get_ds(void)
{
    reg_t val;
    asm (
        "movl %%ds, %[val]\n\t"
        : [val] "=rm" (val)
        : // No inputs
        : // No clobbers
    );
    return val;
}

static inline reg_t get_es(void)
{
    reg_t val;
    asm (
        "movl %%es, %[val]\n\t"
        : [val] "=rm" (val)
        : // No inputs
        : // No clobbers
    );
    return val;
}

static inline reg_t get_fs(void)
{
    reg_t val;
    asm (
        "movl %%fs, %[val]\n\t"
        : [val] "=rm" (val)
        : // No inputs
        : // No clobbers
    );
    return val;
}

static inline reg_t get_gs(void)
{
    reg_t val;
    asm (
        "movl %%gs, %[val]\n\t"
        : [val] "=rm" (val)
        : // No inputs
        : // No clobbers
    );
    return val;
}

static inline reg_t get_ss(void)
{
    reg_t val;
    asm (
        "movl %%ss, %[val]\n\t"
        : [val] "=rm" (val)
        : // No inputs
        : // No clobbers
    );
    return val;
}

#endif // _KERNEL_ASM_H
