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
    uintptr_t sp;       // Only present for privilege change
    uintptr_t ss;       // Only present for privilege change
} interrupt_frame_t;

typedef struct
{
    uint64_t offset_lo  : 16;   // Offset 15:0
    uint64_t selector   : 16;   // Code segment selector
    uint64_t _reserved0 : 5;
    uint64_t type       : 8;    // Gate type
    uint64_t dpl        : 2;    // Destination privilege level
    uint64_t present    : 1;    // Segment present flag
    uint64_t offset_hi  : 16;   // Offset 31:16
} idt_descriptor_t;

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
    };
} error_code_page_t;

void int_init(void);

#endif // _KERNEL_INTERRUPT_H
