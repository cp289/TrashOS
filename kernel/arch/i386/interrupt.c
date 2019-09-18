/**
 * interrupt.c: Interrrupts
 */

#include "interrupt.h"
#include "io.h"

// Interrupt description string table
char *interrupt_description[] = {
    "Divide Error",
    "Debug Exception",
    "NMI Interrupt",
    "Breakpoint",
    "Overflow",
    "BOUND Range Exceeded",
    "Invalid Opcode (Undefined Opcode)",
    "Device Not Available (No Math Coprocessor)",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection",
    "Page Fault",
    "Intel Reserved (15)",
    "x87 FPU Floating-Point Error",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Intel Reserved (21)",
    "Intel Reserved (22)",
    "Intel Reserved (23)",
    "Intel Reserved (24)",
    "Intel Reserved (25)",
    "Intel Reserved (26)",
    "Intel Reserved (27)",
    "Intel Reserved (28)",
    "Intel Reserved (29)",
    "Intel Reserved (30)",
    "Intel Reserved (31)",
};

#define IDT_SIZE    256

static idt_descriptor_t idt[IDT_SIZE];

// Initialize new IDT gate
static void new_idt_gate(idt_descriptor_t *descr, void (*offset)(void), uint16_t flags)
{
    uintptr_t off = (uintptr_t)offset;

    descr->offset_1 = off & 0xffff;
    // TODO Make segment selectors available as constants in a header file
    // Are segment ring levels the only way to provide security in this case?
    descr->selector = 0;
    descr->flags = flags;
    descr->offset_2 = off >> 16;
}

// TODO
// Dump interrupt
static void dump_interrupt(void)
{
    int num = 0;
    kprintf("INT(%d): %s\n", num, interrupt_description[num]);
}

void idt_init(void)
{
    // Initialize IDT
    for (int i = 0; i < IDT_SIZE; i++) {
        new_idt_gate(idt + i, &dump_interrupt, IDT_INTR_GATE32 | IDT_DPL(0));
    }

    // Set IDTR register
    idtr_load(&idt, (sizeof idt) - 1);
}
