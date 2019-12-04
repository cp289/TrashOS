/**
 * asm.h: useful assembly routines
 */

#ifndef _KERNEL_ASM_H
#define _KERNEL_ASM_H

#include "std.h"

// Flags register
typedef union {
    struct {
        uint32_t cf         : 1; // ??
        uint32_t _reserved0 : 1; // Always set to 1
        uint32_t pf         : 1; // ??
        uint32_t _reserved1 : 1; // Always set to 0
        uint32_t af         : 1; // ??
        uint32_t _reserved2 : 1; // Always set to 0
        uint32_t zf         : 1; // ??
        uint32_t sf         : 1; // ??
        uint32_t tf         : 1; // Trap flag
        uint32_t ief        : 1; // Interrupt enable flag
        uint32_t df         : 1; // ??
        uint32_t of         : 1; // ??
        uint32_t iopl       : 2; // I/O Privelege Level
        uint32_t nt         : 1; // Nested task flag
        uint32_t _reserved3 : 1; // Always 1
        uint32_t rf         : 1; // Resume flag
        uint32_t vm         : 1; // Virtual-8086 mode
        uint32_t ac         : 1; // Alignment check / Access control
        uint32_t vif        : 1; // Virtual interrupt flag
        uint32_t vip        : 1; // Virtual interrupt pending
        uint32_t id         : 1; // Identification flag
        uint32_t _reserved4 : 10;// Always zero
    } field;
    uint32_t raw;
} flags_reg_t;

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
        : // No outputs
        : // No inputs
        : // No clobbers
    );
}

static inline void halt(void)
{
    asm ("hlt\n\t");
}

// Clear interrupt enable flag
static inline void cli(void)
{
    asm ("cli\n\t");
}

// Set interrupt enable flag
static inline void sti(void)
{
    asm ("sti\n\t");
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

static inline flags_reg_t get_flags(void)
{
    flags_reg_t out;
    asm volatile (
        "pushf\n\t"
        "pop %0\n\t"
        : "=g" (out.raw)
        : // No inputs
        : // No clobbers
    );
    return out;
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

// MSR

static inline void get_msr(void *val, uint32_t msr)
{
    uint32_t edx;
    asm (
        "movl   %[msr], %%ecx\n\t"
        "rdmsr\n\t"
        "movl   %%eax,  %[lo]\n\t"
        "movl   %%edx,  %[hi]\n\t"
        : [hi]  "=rm" (edx),
          [lo]  "=rm" (*(uint32_t*)val)
        : [msr] "rm" (msr)
        : "eax", "ecx", "edx"
    );
    *(uint64_t*)val |= ((uint64_t)edx << 32);
}

static inline void set_msr(uint64_t val, uint32_t msr)
{
    asm (
        "movl   %[msr], %%ecx\n\t"
        "movl   %[lo],  %%eax\n\t"
        "movl   %[hi],  %%edx\n\t"
        "wrmsr\n\t"
        : // No outputs
        : [msr] "rm" (msr),
          [lo]  "rm" ((uint32_t)val),
          [hi]  "rm" ((uint32_t)(val >> 32))
        : "eax", "ecx", "edx"
    );
}

#endif // _KERNEL_ASM_H
