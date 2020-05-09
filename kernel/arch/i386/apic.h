/**
 * apic.h: Advanced Programmable Interrupt Controller
 */

#ifndef _KERNEL_APIC_H
#define _KERNEL_APIC_H

#include "std.h"

// Constants
enum {
    MAX_PHYS_ADDR       = 32,
    IA32_APIC_BASE_MSR  = 0x1b,
};

// APIC register offsets
enum {
    APIC_ID         = 0x20, // (R/W) APIC ID
    APIC_VER        = 0x30, // (R)   APIC version
    APIC_TPR        = 0x80, // (R/W) Task priority register
    APIC_APR        = 0x90, // (R)   Arbitration priority register
    APIC_PPR        = 0xa0, // (R)   Processor priority register
    APIC_EOI        = 0xb0, //   (W) EOI register
    APIC_RRD        = 0xc0, // (R)   Remote read register
    APIC_LDR        = 0xd0, // (R/W) Logical destination register
    APIC_DFR        = 0xe0, // (R/W) Destination format register
    APIC_SVR        = 0xf0, // (R/W) Spurious interrupt vector register
    APIC_ISR        = 0x100,// (R)   In-service register
    APIC_TMR        = 0x180,// (R)   Trigger mode register
    APIC_IRR        = 0x200,// (R)   Interrupt request register
    APIC_ESR        = 0x280,// (R)   Error status register
    APIC_LVT_CMCI   = 0x2f0,// (R/W) LVT corrected machine check interrupt
    APIC_ICR        = 0x300,// (R/W) Interrupt command register
    APIC_LVT_TR     = 0x320,// (R/W) LVT timer register
    APIC_LVT_TSR    = 0x330,// (R/W) LVT thermal sensor register
    APIC_LVT_PMCR   = 0x340,// (R/W) LVT performance monitoring counters register
    APIC_LVT_LINT0  = 0x350,// (R/W) LVT LINT0 register
    APIC_LVT_LINT1  = 0x360,// (R/W) LVT LINT1 register
    APIC_LVT_ER     = 0x370,// (R/W) LVT error register
    APIC_INIT_COUNT = 0x380,// (R/W) Initial count register (timer)
    APIC_CURR_COUNT = 0x390,// (R)   Current count register (timer)
    APIC_DIV_CONFIG = 0x3e0,// (R/W) Divide configuration register (timer)
};

// APIC register indexes
enum {
    APIC_ID_IDX         = 0x20 >> 2, // (R/W) APIC ID
    APIC_VER_IDX        = 0x30 >> 2, // (R)   APIC version
    APIC_TPR_IDX        = 0x80 >> 2, // (R/W) Task priority register
    APIC_APR_IDX        = 0x90 >> 2, // (R)   Arbitration priority register
    APIC_PPR_IDX        = 0xa0 >> 2, // (R)   Processor priority register
    APIC_EOI_IDX        = 0xb0 >> 2, //   (W) EOI register
    APIC_RRD_IDX        = 0xc0 >> 2, // (R)   Remote read register
    APIC_LDR_IDX        = 0xd0 >> 2, // (R/W) Logical destination register
    APIC_DFR_IDX        = 0xe0 >> 2, // (R/W) Destination format register
    APIC_SVR_IDX        = 0xf0 >> 2, // (R/W) Spurious interrupt vector register
    APIC_ISR_IDX        = 0x100 >> 2,// (R)   In-service register
    APIC_TMR_IDX        = 0x180 >> 2,// (R)   Trigger mode register
    APIC_IRR_IDX        = 0x200 >> 2,// (R)   Interrupt request register
    APIC_ESR_IDX        = 0x280 >> 2,// (R)   Error status register
    APIC_LVT_CMCI_IDX   = 0x2f0 >> 2,// (R/W) LVT corrected machine check interrupt
    APIC_ICR_IDX        = 0x300 >> 2,// (R/W) Interrupt command register
    APIC_LVT_TR_IDX     = 0x320 >> 2,// (R/W) LVT timer register
    APIC_LVT_TSR_IDX    = 0x330 >> 2,// (R/W) LVT thermal sensor register
    APIC_LVT_PMCR_IDX   = 0x340 >> 2,// (R/W) LVT performance monitoring counters register
    APIC_LVT_LINT0_IDX  = 0x350 >> 2,// (R/W) LVT LINT0 register
    APIC_LVT_LINT1_IDX  = 0x360 >> 2,// (R/W) LVT LINT1 register
    APIC_LVT_ER_IDX     = 0x370 >> 2,// (R/W) LVT error register
    APIC_INIT_COUNT_IDX = 0x380 >> 2,// (R/W) Initial count register (timer)
    APIC_CURR_COUNT_IDX = 0x390 >> 2,// (R)   Current count register (timer)
    APIC_DIV_CONFIG_IDX = 0x3e0 >> 2,// (R/W) Divide configuration register (timer)
};

// APIC timer constants
enum {
    APIC_TIMER_DIV2      = 0x0,  // Divide by 2
    APIC_TIMER_DIV4      = 0x1,  // Divide by 4
    APIC_TIMER_DIV8      = 0x2,  // Divide by 8
    APIC_TIMER_DIV16     = 0x3,  // Divide by 16
    APIC_TIMER_DIV32     = 0x8,  // Divide by 32
    APIC_TIMER_DIV64     = 0x9,  // Divide by 64
    APIC_TIMER_DIV128    = 0xa,  // Divide by 128
    APIC_TIMER_DIV1      = 0xb,  // Divide by 1

    APIC_TIMER_ONE_SHOT     = 0, // Single timer signal
    APIC_TIMER_PERIODIC     = 1, // Recurrent timer signal
    APIC_TIMER_TSC_DEADLINE = 2, // Use TSC_DEADLINE_MSR for timing
};

// APIC interrupt delivery constants
enum {
    APIC_DELIVER_FIXED  = 0,
    APIC_DELIVER_SMI    = 2,
    APIC_DELIVER_NMI    = 4,
    APIC_DELIVER_INIT   = 5,
    APIC_DELIVER_EXTINT = 7,
};

typedef struct {
    uint64_t    _reserved0  : 8;
    uint64_t    bsp         : 1;
    uint64_t    _reserved1  : 2;
    uint64_t    enable      : 1;
    uint64_t    base        : MAX_PHYS_ADDR;
    uint64_t    _reserved2  : 52 - MAX_PHYS_ADDR;
} apic_base_msr_t;

// LVT register formats
typedef union {
    struct {
        uint32_t    version                     : 8;
        uint32_t    _reserved0                  : 8;
        uint32_t    max_lvt_entry               : 8;  // Number of LVT entries minus 1
        uint32_t    eoi_broadcast_suppression   : 1;
        uint32_t    _reserved1                  : 7;
    } ver;

    // Error Status Register (ESR)
    struct {
        uint32_t    send_checksum_error         : 1;
        uint32_t    receive_checksum_error      : 1;
        uint32_t    send_accept_error           : 1;
        uint32_t    receive_accept_error        : 1;
        uint32_t    redirectable_ipi            : 1;
        uint32_t    send_illegal_vector         : 1;
        uint32_t    received_illegal_vector     : 1;
        uint32_t    illegal_register_address    : 1;
        uint32_t    _reserved                   : 24;
    } esr;

    // Spurious-Interrupt Vector Register
    struct {
        uint32_t    vector                      : 8;
        uint32_t    enable                      : 1;
        uint32_t    focus_processor_checking    : 1;
        uint32_t    _reserved0                  : 2;
        uint32_t    eoi_broadcast_suppression   : 1;
        uint32_t    _reserved1                  : 19;
    } svr;

    // Standard LVT register
    struct {
        uint32_t    vector          : 8;
        uint32_t    delivery        : 3; // See APIC interrupt delivery constants
        uint32_t    _reserved0      : 1;
        uint32_t    delivery_status : 1;
        uint32_t    pin_polarity    : 1; // Interrupt input pin polarity
        uint32_t    remote_irr      : 1;
        uint32_t    trigger_mode    : 1;
        uint32_t    masked          : 1;
        uint32_t    timer_mode      : 2; // Only for APIC timer
        uint32_t    _reserved1      : 13;
    } lvt;

    // APIC timer divide register
    struct {
        uint32_t    divide      : 4;
        uint32_t    _reserved0  : 28;
    } timer;

    // Raw register value
    uint32_t raw;
} apic_lvt_reg_t;

void apic_init(void);
void lapic_send_eoi(void);
apic_lvt_reg_t lapic_get_esr(void);

#endif // _KERNEL_APIC_H
