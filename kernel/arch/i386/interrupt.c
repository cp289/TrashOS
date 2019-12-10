/**
 * interrupt.c: Interrrupts
 */

#include "apic.h"
#include "asm.h"
#include "interrupt.h"
#include "io.h"

static idt_descriptor_t idt[IDT_SIZE];

static uint64_t idtr;

// TODO Is this string table necessary? Interrupt vectors numbers don't seem to
// be accessible in code.
char *interrupt_string[] = {
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

// TODO Define an handler for each of the above architecture-defined interrupts

// Handle generic interrupt
INTERRUPT handle_interrupt(interrupt_frame_t *ctxt)
{
    printk("INT: IP: %p CS: %p FLAGS: %p\n", ctxt->ip, ctxt->cs, ctxt->flags);
}

// Handle generic exception (has error code)
INTERRUPT handle_exception(interrupt_frame_t *ctxt, uintptr_t error_code)
{
    printk("EXC: IP: %p CS: %p FLAGS: %p ERROR: %u\n", ctxt->ip, ctxt->cs,
            ctxt->flags, error_code);
}

// TODO call scheduling routine
INTERRUPT handle_lapic_timer(volatile interrupt_frame_t *frame)
{
    (void)frame;
    printk("T");
    lapic_eoi();
}

INTERRUPT handle_unknown(interrupt_frame_t *frame)
{
    printk("UNKNOWN INTERRUPT! %p", frame);
}

static void idt_init_default(void)
{
    // Initialize IDT with default handlers

    // TODO Make segment selectors available as constants in a header file
    // Are segment ring levels the only way to provide security in this case?

    /**
     * TODO currently, we simply use the current (privilege 0) code selector for
     * all interrupt gates. This may not be desirable.
     */
    // TODO change DPL values to constants?
    for (int i = 0; i < IDT_SIZE; i++) {
        switch (i) {
            case 8: case 10: case 11: case 12: case 13: case 14: case 17:
                idt[i] = (idt_descriptor_t) {
                    .offset_lo = (uintptr_t)(&handle_exception) & 0xffff,
                    .offset_hi = (uintptr_t)(&handle_exception) >> 16,
                    .selector = get_cs(),
                    .type = IDT_GATE_INTERRUPT32,
                    .dpl = 0,
                    .present = 1,
                };
                break;
            default:
                idt[i] = (idt_descriptor_t) {
                    .offset_lo = (uintptr_t)(&handle_interrupt) & 0xffff,
                    .offset_hi = (uintptr_t)(&handle_interrupt) >> 16,
                    .selector = get_cs(),
                    .type = IDT_GATE_INTERRUPT32,
                    .dpl = 0,
                    .present = 1,
                };
        }
    }
}

// Setup timing interrupts
static void idt_init_lapic_timer(void)
{
    idt[IDT_VECTOR_TIMER] = (idt_descriptor_t) {
        .offset_lo = (uintptr_t)(&handle_lapic_timer) & 0xffff,
        .offset_hi = (uintptr_t)(&handle_lapic_timer) >> 16,
        .selector = get_cs(),
        .type = IDT_GATE_INTERRUPT32,
        .dpl = 0,
        .present = 1,
    };
}

// Tell processor where IDT is
static inline void idt_load(void)
{
    // Set IDTR register

    // IDTR Register:
    //  bits 47:16  IDT base address
    //  bits 15:0   IDT limit (byte size of IDT)

    idtr = ((uint64_t)(uintptr_t)idt << 16) | (sizeof idt - 1);

    asm (
        "lidt    (%0)\n\t"
        : // No outputs
        : "rm" (&idtr)
        : // No clobbers
    );
}

void idt_init(void)
{
    idt_init_default();
    idt_init_lapic_timer();
    idt_load();
}
