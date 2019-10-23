/**
 * interrupt.c: Interrrupts
 */

#include "asm.h"
#include "interrupt.h"
#include "io.h"

static idt_descriptor_t idt[IDT_SIZE];

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

// Handle an interrupt
INTERRUPT handle_interrupt(interrupt_frame_t *ctxt)
{
    kprintf("INT: IP: %p CS: %p FLAGS: %p\n", ctxt->ip, ctxt->cs, ctxt->flags);
}

// Handle an exception
INTERRUPT handle_exception(interrupt_frame_t *ctxt, uintptr_t error_code)
{
    kprintf("EXC: IP: %p CS: %p FLAGS: %p ERROR: %u\n", ctxt->ip, ctxt->cs,
            ctxt->flags, error_code);
}

// Initialize new IDT gate
static void new_idt_gate(idt_descriptor_t *descr, void (*offset)(), uint16_t flags)
{
    uintptr_t off = (uintptr_t)offset;

    descr->offset_1 = off & 0xffff;
    // TODO Make segment selectors available as constants in a header file
    // Are segment ring levels the only way to provide security in this case?

    /**
     * TODO currently, we simply use the current (privilege 0) code selector for
     * all interrupt gates. This may not be desirable.
     */
    descr->selector = get_cs();
    descr->flags = flags;
    descr->offset_2 = off >> 16;
}

void idt_init(void)
{
    // Initialize IDT
    for (int i = 0; i < IDT_SIZE; i++) {
        switch (i) {
            case 8: case 10: case 11: case 12: case 13: case 14: case 17:
                new_idt_gate(idt + i, &handle_exception,
                        IDT_INTR_GATE32 | IDT_SEG_PRESENT | IDT_DPL(0));
                break;
            default:
                new_idt_gate(idt + i, &handle_interrupt,
                        IDT_INTR_GATE32 | IDT_SEG_PRESENT | IDT_DPL(0));
        }
    }

    // Set IDTR register

    // IDTR Register:
    //  bits 47:16  IDT base address
    //  bits 15:0   IDT limit (byte size of IDT)

    // TODO Here we define variables directly in the data section. We should
    // instead define this as an (extern?) uint64_t variable in C. Outside of
    // cleaner code, it is unclear if there is any performance benefit to doing
    // so, perhaps more effective Link Time Optimization?
    asm (
        ".section .data\n"

        ".align 8\n"
        "idtr:\n\t"
        ".word 0\n\t"
        ".long 0\n"

        ".section .text\n"

        "movl    %[idtr_addr], idtr+2\n\t"
        "movw    %[idtr_size], idtr\n\t"
        "lidt    idtr\n\t"
        : // No outputs
        :   [idtr_addr] "rm" (&idt),
            [idtr_size] "rm" ((uint16_t)(sizeof idt - 1))
        : // No clobbers
    );
}
