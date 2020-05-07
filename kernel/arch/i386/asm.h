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
    };
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
    asm volatile (
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
    asm volatile (
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
    asm volatile (
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
    asm volatile (
        "movl %%cr3, %[val]\n\t"
        : [val] "=rm" (val)
        : // No inputs
        : // No clobbers
    );
    return val;
}

static inline void set_cr3(reg_t val)
{
    asm (
        "movl %[val], %%cr3\n\t"
        : // No outputs
        : [val] "r" (val)
        : // No clobbers
    );
}

static inline reg_t get_cr4(void)
{
    reg_t val;
    asm volatile (
        "movl %%cr4, %[val]\n\t"
        : [val] "=rm" (val)
        : // No inputs
        : // No clobbers
    );
    return val;
}

// Invalidate TLB entry
static inline void invlpg(uintptr_t vma)
{
    asm (
        "invlpg %[vma]\n\t"
        : // No outputs
        : [vma] "m" (vma)
        : // No clobbers
    );
}

// Get stack pointer
static inline reg_t get_sp(void)
{
    reg_t val;
    asm volatile (
        "movl %%esp, %[val]\n\t"
        : [val] "=rm" (val)
        : // No inputs
        : // No clobbers
    );
    return val;
}

// Set stack pointer
static inline void set_sp(uintptr_t val)
{
    asm (
        "movl %[val], %%esp\n\t"
        : // No outputs
        : [val] "rm" (val)
        : // No clobbers
    );
}

static inline void set_flags(flags_reg_t flags)
{
    asm (
        "pushl %0\n\t"
        "popf\n\t"
        : // No outputs
        : "r" (flags.raw)
        : // No clobbers
    );
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

static inline uint16_t get_cs(void)
{
    uint16_t val;
    asm volatile (
        "movw %%cs, %[val]\n\t"
        : [val] "=rm" (val)
        : // No inputs
        : // No clobbers
    );
    return val;
}

static inline uint16_t get_ds(void)
{
    uint16_t val;
    asm volatile (
        "movw %%ds, %[val]\n\t"
        : [val] "=rm" (val)
        : // No inputs
        : // No clobbers
    );
    return val;
}

static inline uint16_t get_es(void)
{
    uint16_t val;
    asm volatile (
        "movw %%es, %[val]\n\t"
        : [val] "=rm" (val)
        : // No inputs
        : // No clobbers
    );
    return val;
}

static inline uint16_t get_fs(void)
{
    uint16_t val;
    asm volatile (
        "movw %%fs, %[val]\n\t"
        : [val] "=rm" (val)
        : // No inputs
        : // No clobbers
    );
    return val;
}

static inline uint16_t get_gs(void)
{
    uint16_t val;
    asm volatile (
        "movw %%gs, %[val]\n\t"
        : [val] "=rm" (val)
        : // No inputs
        : // No clobbers
    );
    return val;
}

static inline uint16_t get_ss(void)
{
    uint16_t val;
    asm volatile (
        "movw %%ss, %[val]\n\t"
        : [val] "=rm" (val)
        : // No inputs
        : // No clobbers
    );
    return val;
}

static inline void set_cs(uint16_t cs)
{
    asm volatile (
        "pushl  %[val]\n\t"
        "pushl  $set_cs_%=\n\t"
        "lret\n\t"
        "set_cs_%=:\n\t"
        : // No outputs
        : [val] "rm" ((uint32_t)cs)
        : // No clobbers
    );
}

static inline void set_ds(uint16_t ds)
{
    asm (
        "movw %[val], %%ds\n\t"
        : // No outputs
        : [val] "rm" (ds)
        : // No clobbers
    );
}

static inline void set_es(uint16_t es)
{
    asm (
        "movw %[val], %%es\n\t"
        : // No outputs
        : [val] "rm" (es)
        : // No clobbers
    );
}

static inline void set_fs(uint16_t fs)
{
    asm (
        "movw %[val], %%fs\n\t"
        : // No outputs
        : [val] "rm" (fs)
        : // No clobbers
    );
}

static inline void set_gs(uint16_t gs)
{
    asm (
        "movw %[val], %%gs\n\t"
        : // No outputs
        : [val] "rm" (gs)
        : // No clobbers
    );
}

static inline void set_ss(uint16_t ss)
{
    asm (
        "movw %[val], %%ss\n\t"
        : // No outputs
        : [val] "rm" (ss)
        : // No clobbers
    );
}

static inline void set_tr(uint16_t tr)
{
    asm (
        "ltr %0\n\t"
        : // No outputs
        : "r" (tr)
        : // No clobbers
    );
}

static inline void load_gdt(uint64_t *selector)
{
    asm (
        "lgdt    (%0)\n\t"
        : // No ouptuts
        : "r" (selector)
        : // No clobbers
    );
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
