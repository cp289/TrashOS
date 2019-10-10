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

// Interrupt descriptor flag constants
enum {
    IDT_TASK_GATE32 = 0x0500,   // 32-bit task gate type
    IDT_INTR_GATE16 = 0x0600,   // 16-bit interrupt gate type
    IDT_TRAP_GATE16 = 0x0700,   // 16-bit trap gate type
    IDT_INTR_GATE32 = 0x0e00,   // 32-bit interrupt gate type
    IDT_TRAP_GATE32 = 0x0f00,   // 32-bit trap gate type
    IDT_SEG_PRESENT = 0x8000,   // Segment present flag
};

// Descriptor privelege level
#define IDT_DPL(x)  ((x) << 13)

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
    uint16_t offset_1;  // Offset 15:0
    uint16_t selector;  // Code segment selector
    uint16_t flags;     // Type and attribute flags
    uint16_t offset_2;  // Offset 31:16
} idt_descriptor_t;

// Interrupt description string table
extern char *interrupt_description[];

void idt_init(void);

#endif // _KERNEL_INTERRUPT_H
