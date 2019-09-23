/**
 * interrupt.h: Interrupt handling
 *
 * For documentation on interrupts, see Intel SDM, vol. 3, sections 6.11 and 6.12
 */

#ifndef _KERNEL_INTERRUPT_H
#define _KERNEL_INTERRUPT_H

#include "std.h"

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

typedef struct
{
    uint16_t offset_1;  // Offset 15:0
    uint16_t selector;  // Code segment selector
    uint16_t flags;     // Type and attribute flags
    uint16_t offset_2;  // Offset 31:16
} idt_descriptor_t;

extern char *interrupt_description[];

void catch_interrupt(void);
void idtr_load(void *base, uint16_t limit);
void idt_init(void);

#endif // _KERNEL_INTERRUPT_H
