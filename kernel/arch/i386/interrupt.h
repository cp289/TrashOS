/**
 * interrupt.h: Interrupt handling
 *
 * For documentation on interrupts, see Intel SDM, vol. 3, sections 6.11 and 6.12
 */

#ifndef _KERNEL_INTERRUPT_H
#define _KERNEL_INTERRUPT_H

#include "std.h"

enum {
    IDT_SIZE = 256,
};

// IDT vector constants
enum {
    IDT_VECTOR_TIMER = 65,
};

// Interrupt descriptor type constants
enum {
    IDT_GATE_TASK           = 0x28, // (32-bit) task gate type
    IDT_GATE_INTERRUPT16    = 0x30, // 16-bit interrupt gate type
    IDT_GATE_INTERRUPT32    = 0x70, // 32-bit interrupt gate type
    IDT_GATE_TRAP16         = 0x38, // 16-bit trap gate type
    IDT_GATE_TRAP32         = 0x78, // 32-bit trap gate type
};

/**
 * NOTE: According to the Intel Software Developer's Manual, vol. 3, section
 * 6.12.1, the stack has EFLAGS, CS, EIP, and (sometimes) an Error Code pushed
 * to the stack. In the case no error code is pushed, we call the interrupt
 * handler, whereas if an error code is pushed, we call the exception handler.
 *
 * We use GCC's `interrupt` function attributes to declare interrupt handlers
 * without the need for assembly.
 */

// Function declaration for interrupt and exception handlers
#define INTERRUPT __attribute__((interrupt)) void

typedef struct
{
    uintptr_t ip;
    uintptr_t cs;
    uintptr_t flags;
} interrupt_frame_t;

typedef struct
{
    uint64_t offset_lo  : 16;   // Offset 15:0
    uint64_t selector   : 16;   // Code segment selector
    uint64_t _reserved0 : 5;
    uint64_t type       : 8;    // Gate type
    uint64_t dpl        : 2;    // Destination privelege level
    uint64_t present    : 1;    // Segment present flag
    uint64_t offset_hi  : 16;   // Offset 31:16
} idt_descriptor_t;

// Interrupt description string table
extern char *interrupt_description[];

void idt_init(void);

#endif // _KERNEL_INTERRUPT_H
