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

// Possible causes for page faults
// NOTE: some of the below causes are impossible
enum {
    SUPERVISOR_NOT_PRESENT = 0,
    SUPERVISOR_VIOLATION,
    SUPERVISOR_WRITE_NOT_PRESENT,
    SUPERVISOR_WRITE_VIOLATION,
    USER_NOT_PRESENT,
    USER_VIOLATION,
    USER_WRITE_NOT_PRESENT,
    USER_WRITE_VIOLATION,
    RESERVED_SUPERVISOR_NOT_PRESENT,
    RESERVED_SUPERVISOR_VIOLATION,
    RESERVED_SUPERVISOR_WRITE_NOT_PRESENT,
    RESERVED_SUPERVISOR_WRITE_VIOLATION,
    RESERVED_USER_NOT_PRESENT,
    RESERVED_USER_VIOLATION,
    RESERVED_USER_WRITE_NOT_PRESENT,
    RESERVED_USER_WRITE_VIOLATION,
    FETCH_SUPERVISOR_NOT_PRESENT,
    FETCH_SUPERVISOR_VIOLATION,
    FETCH_SUPERVISOR_WRITE_NOT_PRESENT,
    FETCH_SUPERVISOR_WRITE_VIOLATION,
    FETCH_USER_NOT_PRESENT,
    FETCH_USER_VIOLATION,
    FETCH_USER_WRITE_NOT_PRESENT,
    FETCH_USER_WRITE_VIOLATION,
    FETCH_RESERVED_SUPERVISOR_NOT_PRESENT,
    FETCH_RESERVED_SUPERVISOR_VIOLATION,
    FETCH_RESERVED_SUPERVISOR_WRITE_NOT_PRESENT,
    FETCH_RESERVED_SUPERVISOR_WRITE_VIOLATION,
    FETCH_RESERVED_USER_NOT_PRESENT,
    FETCH_RESERVED_USER_VIOLATION,
    FETCH_RESERVED_USER_WRITE_NOT_PRESENT,
    FETCH_RESERVED_USER_WRITE_VIOLATION,
    PROTECTION_SUPERVISOR_NOT_PRESENT,
    PROTECTION_SUPERVISOR_VIOLATION,
    PROTECTION_SUPERVISOR_WRITE_NOT_PRESENT,
    PROTECTION_SUPERVISOR_WRITE_VIOLATION,
    PROTECTION_USER_NOT_PRESENT,
    PROTECTION_USER_VIOLATION,
    PROTECTION_USER_WRITE_NOT_PRESENT,
    PROTECTION_USER_WRITE_VIOLATION,
    PROTECTION_RESERVED_SUPERVISOR_NOT_PRESENT,
    PROTECTION_RESERVED_SUPERVISOR_VIOLATION,
    PROTECTION_RESERVED_SUPERVISOR_WRITE_NOT_PRESENT,
    PROTECTION_RESERVED_SUPERVISOR_WRITE_VIOLATION,
    PROTECTION_RESERVED_USER_NOT_PRESENT,
    PROTECTION_RESERVED_USER_VIOLATION,
    PROTECTION_RESERVED_USER_WRITE_NOT_PRESENT,
    PROTECTION_RESERVED_USER_WRITE_VIOLATION,
    PROTECTION_FETCH_SUPERVISOR_NOT_PRESENT,
    PROTECTION_FETCH_SUPERVISOR_VIOLATION,
    PROTECTION_FETCH_SUPERVISOR_WRITE_NOT_PRESENT,
    PROTECTION_FETCH_SUPERVISOR_WRITE_VIOLATION,
    PROTECTION_FETCH_USER_NOT_PRESENT,
    PROTECTION_FETCH_USER_VIOLATION,
    PROTECTION_FETCH_USER_WRITE_NOT_PRESENT,
    PROTECTION_FETCH_USER_WRITE_VIOLATION,
    PROTECTION_FETCH_RESERVED_SUPERVISOR_NOT_PRESENT,
    PROTECTION_FETCH_RESERVED_SUPERVISOR_VIOLATION,
    PROTECTION_FETCH_RESERVED_SUPERVISOR_WRITE_NOT_PRESENT,
    PROTECTION_FETCH_RESERVED_SUPERVISOR_WRITE_VIOLATION,
    PROTECTION_FETCH_RESERVED_USER_NOT_PRESENT,
    PROTECTION_FETCH_RESERVED_USER_VIOLATION,
    PROTECTION_FETCH_RESERVED_USER_WRITE_NOT_PRESENT,
    PROTECTION_FETCH_RESERVED_USER_WRITE_VIOLATION,
};

// TODO move to header file
typedef union
{
    uintptr_t raw;
    struct {
        uintptr_t violation     : 1; // 0: non-present page, 1: protection violation
        uintptr_t write         : 1; // Caused by a write
        uintptr_t user_mode     : 1; // Caused in user_mode
        uintptr_t reserved      : 1; // Caused by RSVD flag
        uintptr_t instruction   : 1; // Caused by instruction fetch
        uintptr_t protection    : 1; // Caused by protection-key violation
        uintptr_t _reserved0    : 9;
        uintptr_t sgx           : 1; // Caused by violation of SGX-specific access control
        uintptr_t _reserved1    : 16;
    } bits;
} error_code_page_t;

// Handle page fault exception
INTERRUPT handle_page_fault(interrupt_frame_t *ctxt, uintptr_t error_raw)
{
    error_code_page_t error = { .raw = error_raw };

    if (error.bits.user_mode) {
        printk("USER PAGE FAULT: at %p, error: %p\n", ctxt->ip, error);
    } else {
        if (!error.bits.violation) {
            printk("SUPERVISOR PAGE ABSENT FROM %p: at %p, error: %p, sp: %p\n", get_cr2(), ctxt->ip, error, get_sp());
        } else {
            printk("UNKNOWN PAGE FAULT: at %p, error: %p\n", ctxt->ip, error);
        }
    }
    halt();
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
            case 14:
                idt[i] = idt_descriptor(&handle_page_fault, get_cs(),
                                        IDT_GATE_INTERRUPT32, 0);
                break;
            case 8: case 10: case 11: case 12: case 13: case 17:
                idt[i] = idt_descriptor(&handle_exception, get_cs(),
                                        IDT_GATE_INTERRUPT32, 0);
                break;
            default:
                idt[i] = idt_descriptor(&handle_interrupt, get_cs(),
                                        IDT_GATE_INTERRUPT32, 0);
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
