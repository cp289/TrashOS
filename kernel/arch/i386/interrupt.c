/**
 * interrupt.c: Interrrupts
 */

#include "apic.h"
#include "asm.h"
#include "gdt.h"
#include "interrupt.h"
#include "io.h"
#include "proc.h"

static idt_descriptor_t idt[IDT_SIZE];

static uint64_t idtr;

// TODO delete this
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

// Handle generic interrupt
INTERRUPT handle_interrupt(interrupt_frame_t *ctxt)
{
    printk("INT: IP: %p CS: %p FLAGS: %p\n", ctxt->ip, ctxt->cs, ctxt->flags);
    halt();
}

// Handle generic exception (has error code)
INTERRUPT handle_exception(interrupt_frame_t *ctxt, uintptr_t error_code)
{
    printk("EXC: IP: %p CS: %p FLAGS: %p ERROR: %u\n", ctxt->ip, ctxt->cs,
            ctxt->flags, error_code);
    halt();
}

// Interrupt 0
INTERRUPT handle_divide_error(interrupt_frame_t *frame)
{
    printk("Divide Error Exception: ip: %p", frame->ip);
    halt();
}

// Interrupt 1
INTERRUPT handle_debug(interrupt_frame_t *frame)
{
    printk("Debug Exception: ip: %p\n", frame->ip);
    halt();
}

// Interrupt 2
INTERRUPT handle_nmi_interrupt(interrupt_frame_t *frame)
{
    printk("NMI Interrupt: ip: %p\n", frame->ip);
    halt();
}

// Interrupt 3
INTERRUPT handle_breakpoint(interrupt_frame_t *frame)
{
    printk("Breakpoint Exception: ip: %p\n", frame->ip);
    halt();
}

// Interrupt 4
INTERRUPT handle_overflow(interrupt_frame_t *frame)
{
    printk("Overflow Exception: ip: %p\n", frame->ip);
    halt();
}

// Interrupt 5
INTERRUPT handle_bound_range_exceeded(interrupt_frame_t *frame)
{
    printk("BOUND Range Exceeded Exception: ip: %p\n", frame->ip);
    halt();
}

// Interrupt 6
INTERRUPT handle_invalid_opcode(interrupt_frame_t *frame)
{
    printk("Invalid Opcode Exception: ip: %p\n", frame->ip);
    halt();
}

// Interrupt 7
INTERRUPT handle_device_not_available(interrupt_frame_t *frame)
{
    printk("Device Not Available Exception: ip: %p\n", frame->ip);
    halt();
}

// Interrupt 8
INTERRUPT handle_double_fault(interrupt_frame_t *frame, uintptr_t error)
{
    // NOTE error is always zero
    printk("Double Fault Exception: ip: %p, error: %p\n", frame->ip, error);
    halt();
}

// Interrupt 9
INTERRUPT handle_coprocessor_segment_overrun(interrupt_frame_t *frame)
{
    printk("Coprocessor Segment Overrun: ip: %p\n", frame->ip);
    halt();
}

// Interrupt 10
INTERRUPT handle_invalid_tss(interrupt_frame_t *frame, uintptr_t error)
{
    printk("Invalid TSS Exception: ip: %p, error: %p\n", frame->ip, error);
    halt();
}

// Interrupt 11
INTERRUPT handle_segment_not_present(interrupt_frame_t *frame, uintptr_t error)
{
    printk("Segment Not present: ip: %p, error: %p\n", frame->ip, error);
    halt();
}

// Interrupt 12
INTERRUPT handle_stack_fault(interrupt_frame_t *frame, uintptr_t error)
{
    printk("Stack Fault Exception: ip: %p, error: %p\n", frame->ip, error);
    halt();
}

// Interrupt 13
INTERRUPT handle_general_protection(interrupt_frame_t *frame, uintptr_t error)
{
    printk("General Protection Exception: ip: %p, error: %p\n", frame->ip, error);
    halt();
}

// Interrupt 14
INTERRUPT handle_page_fault(interrupt_frame_t *ctxt, uintptr_t error_raw)
{
    error_code_page_t error = { .raw = error_raw };

    reg_t cr2 = get_cr2();

    if (error.user_mode) {
        printk("USER PAGE FAULT (%p): ip: %p, error: %p\n", cr2, ctxt->ip, error);
    } else {
        if (!error.violation) {
            printk("SUPERVISOR PAGE ABSENT (%p): ip: %p, error: %p, sp: %p\n",
                   cr2, ctxt->ip, error, get_sp());
        } else {
            printk("UNKNOWN PAGE FAULT (%p): ip: %p, error: %p\n", cr2,
                   ctxt->ip, error);
        }
    }
    halt();
}

INTERRUPT handle_unknown(interrupt_frame_t *frame)
{
    printk("UNKNOWN INTERRUPT! %p", frame);
    halt();
}

static inline idt_descriptor_t
idt_descriptor(void (*handler)(), reg_t sel, int type, int dpl)
{
    return (idt_descriptor_t) {
        .offset_lo = (uintptr_t)(handler) & 0xffff,
        .offset_hi = (uintptr_t)(handler) >> 16,
        .selector = sel,
        .type = type,
        .dpl = dpl,
        .present = 1,
    };
}

void int_handle_proc_switch(void);  // See int_asm.s for implementation

static void idt_init_vectors(void)
{
    // Initialize IDT with default handlers
    for (int i = 0; i < IDT_SIZE; i++) {
        switch (i) {
        case 0:
            idt[i] = idt_descriptor(&handle_divide_error, GDT_SEL_CODE_PL0,
                                    IDT_GATE_INTERRUPT32, RING0);
            break;
        case 1:
            idt[i] = idt_descriptor(&handle_debug, GDT_SEL_CODE_PL0,
                                    IDT_GATE_INTERRUPT32, RING0);
            break;
        case 2:
            idt[i] = idt_descriptor(&handle_nmi_interrupt, GDT_SEL_CODE_PL0,
                                    IDT_GATE_INTERRUPT32, RING0);
            break;
        case 3:
            idt[i] = idt_descriptor(&handle_breakpoint, GDT_SEL_CODE_PL0,
                                    IDT_GATE_INTERRUPT32, RING0);
            break;
        case 4:
            idt[i] = idt_descriptor(&handle_overflow, GDT_SEL_CODE_PL0,
                                    IDT_GATE_INTERRUPT32, RING0);
            break;
        case 5:
            idt[i] = idt_descriptor(&handle_bound_range_exceeded,
                                    GDT_SEL_CODE_PL0, IDT_GATE_INTERRUPT32,
                                    RING0);
            break;
        case 6:
            idt[i] = idt_descriptor(&handle_invalid_opcode, GDT_SEL_CODE_PL0,
                                    IDT_GATE_INTERRUPT32, RING0);
            break;
        case 7:
            idt[i] = idt_descriptor(&handle_device_not_available,
                                    GDT_SEL_CODE_PL0, IDT_GATE_INTERRUPT32,
                                    RING0);
            break;
        case 8:
            idt[i] = idt_descriptor(&handle_double_fault, GDT_SEL_CODE_PL0,
                                    IDT_GATE_INTERRUPT32, RING0);
            break;
        case 9:
            idt[i] = idt_descriptor(&handle_coprocessor_segment_overrun,
                                    GDT_SEL_CODE_PL0, IDT_GATE_INTERRUPT32,
                                    RING0);
            break;
        case 10:
            idt[i] = idt_descriptor(&handle_invalid_tss, GDT_SEL_CODE_PL0,
                                    IDT_GATE_INTERRUPT32, RING0);
            break;
        case 11:
            idt[i] = idt_descriptor(&handle_segment_not_present,
                                    GDT_SEL_CODE_PL0, IDT_GATE_INTERRUPT32,
                                    RING0);
            break;
        case 12:
            idt[i] = idt_descriptor(&handle_stack_fault, GDT_SEL_CODE_PL0,
                                    IDT_GATE_INTERRUPT32, RING0);
            break;
        case 13:
            idt[i] = idt_descriptor(&handle_general_protection,
                                    GDT_SEL_CODE_PL0, IDT_GATE_INTERRUPT32,
                                    RING0);
            break;
        case 14:
            idt[i] = idt_descriptor(&handle_page_fault, GDT_SEL_CODE_PL0,
                                    IDT_GATE_INTERRUPT32, RING0);
            break;
        case 17:
            idt[i] = idt_descriptor(&handle_exception, GDT_SEL_CODE_PL0,
                                    IDT_GATE_INTERRUPT32, RING0);
            break;
        case IDT_VECTOR_TIMER:
            idt[i] = idt_descriptor(&int_handle_proc_switch, GDT_SEL_CODE_PL0,
                                    IDT_GATE_INTERRUPT32, RING0);
            break;
        default:
            idt[i] = idt_descriptor(&handle_unknown, GDT_SEL_CODE_PL0,
                                    IDT_GATE_INTERRUPT32, RING0);
        }
    }
}

// Tell processor where IDT is
static inline void idt_load(void)
{
    // IDTR Register:
    //  bits 47:16  IDT base address
    //  bits 15:0   IDT limit (byte size of IDT)

    // Set IDTR register
    idtr = ((uint64_t)(uintptr_t)idt << 16) | (sizeof idt - 1);

    // Load IDTR register
    asm (
        "lidt    (%0)\n\t"
        : // No outputs
        : "rm" (&idtr)
        : // No clobbers
    );
}

void idt_init(void)
{
    idt_init_vectors();
    idt_load();
}
