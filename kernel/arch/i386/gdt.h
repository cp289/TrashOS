/**
 * gdt.h: Global Descriptor Table
 *
 * For documentation, see: Intel Software Development Manual 3A 3-9
 */

#ifndef _KERNEL_GDT_H
#define _KERNEL_GDT_H

#include "std.h"

/**
 * GDT Segment Descriptor
 *  (15:0)  Segment limit bits 15:0
 *  (31:16) Base address bits 15:0
 *  (39:32) Base address bits 23:16
 *  (40)    Accessed bit: whether CPU has accessed this selector
 *  (41)    R/W bit: Whether segment is readable for code selectors, whether
 *          segment is writable for data selectors (data is always readable)
 *  (42)    Direction/Conforming bit: Direction bit for data selectors (0 ->
 *          segment grows upwards, 1 -> segment grows downwards), Conforming bit
 *          for code selectors (0 -> only executable from ring set in Privilege,
 *          1 -> executable from ring set in Privilege or any lower privilege
 *          ring)
 *  (43)    Executable bit: 0 -> data selector, 1 -> code selector
 *
 *  (44)    Descriptor type: 0 -> system segment, 1 -> code/data segment
 *  (46:45) Privilege:  ring level 0, 1, 2, or 3
 *  (47)    Present bit: must be 1 for valid selectors
 *
 *  (51:48) Segment limit bits 19:16
 *
 *  (52)    AVL: Available for use by system software
 *  (53)    L: 64-bit code segment (IA-32e mode only)
 *  (54)    Size: 0 -> 16-bit protected mode, 1 -> 32-bit protected mode
 *  (55)    Granularity: 0 -> limit defined in 1B blocks, 1 -> limit defined in
 *          4K blocks (page granularity)
 *  (63:56) Base address 31:24
 *
 * limit: 20-bit segment size
 * base: 32-bit linear address where segment begins
 */

// Definitions for segment descriptor bits 55:44 (relative to bit 40)
#define SEG_PRIV(x) (((x) & 0x3) << 5)  // Privilege
#define SEG_PRES(x) ((x) << 7)          // Present bit
#define SEG_AVLS(x) ((x) << 12)         // Available for system use
#define SEG_LONG(x) ((x) << 13)         // Long mode
#define SEG_SIZE(x) ((x) << 14)         // Size
#define SEG_GRAN(x) ((x) << 15)         // Granularity

// Privilege level constants
enum {
    RING0 = 0,
    RING1,
    RING2,
    RING3,
};

// Indexes into GDT
enum {
    GDT_IDX_NULL = 0,
    GDT_IDX_CODE_PL0,
    GDT_IDX_DATA_PL0,
    GDT_IDX_TSS_PL0,
    GDT_IDX_CODE_PL3,
    GDT_IDX_DATA_PL3,
    GDT_IDX_TSS_PL3,
    GDT_IDX_LIMIT,
};

// GDT selector constants
enum {
    GDT_SEL_CODE_PL0 = (GDT_IDX_CODE_PL0 << 3) | 0,
    GDT_SEL_DATA_PL0 = (GDT_IDX_DATA_PL0 << 3) | 0,
    GDT_SEL_TSS_PL0  = (GDT_IDX_TSS_PL0  << 3) | 0,
    GDT_SEL_CODE_PL3 = (GDT_IDX_CODE_PL3 << 3) | 3,
    GDT_SEL_DATA_PL3 = (GDT_IDX_DATA_PL3 << 3) | 3,
    GDT_SEL_TSS_PL3  = (GDT_IDX_TSS_PL3  << 3) | 3,
};

// Constants for segment descriptor bits 43:40
enum {
    SEG_SYS_TSS_16 = 1, // 16-bit TSS (Available)
    SEG_SYS_LDT,        // LDT
    SEG_SYS_TSS_16_B,   // 16-bit TSS (Busy)
    SEG_SYS_CALL_16,    // 16-bit Call Gate
    SEG_SYS_TASK,       // Task Gate
    SEG_SYS_INT_16,     // 16-bit Interrupt Gate
    SEG_SYS_TRAP_16,    // 16-bit Trap Gate
    SEG_SYS_TSS = 9,    // TSS (Available)
    SEG_SYS_TSS_B = 11, // TSS (Busy)
    SEG_SYS_CALL,       // Call Gate
    SEG_SYS_INT = 14,   // Interrupt Gate
    SEG_SYS_TRAP,       // Trap Gate
    SEG_DATA_,          // Read-only
    SEG_DATA_A,         // Read-only, accessed
    SEG_DATA_W,         // Read/write
    SEG_DATA_WA,        // Read/write, accessed
    SEG_DATA_D,         // Read-only, expand down
    SEG_DATA_DA,        // Read-only, expand down, accessed
    SEG_DATA_DW,        // Read/write, expand down
    SEG_DATA_DWA,       // Read/write, expand down, accessed
    SEG_CODE_,          // Execute-only
    SEG_CODE_A,         // Execute-only, accessed
    SEG_CODE_R,         // Execute/read
    SEG_CODE_RA,        // Execute/read, accessed
    SEG_CODE_C,         // Execute-only, conforming
    SEG_CODE_CA,        // Execute-only, conforming, accessed
    SEG_CODE_CR,        // Execute/read, conforming
    SEG_CODE_CRA,       // Execute/read, conforming, accessed
};

#define GDT_CODE_PL0    SEG_PRES(1) | SEG_AVLS(0) | SEG_LONG(0) | \
                        SEG_SIZE(1) | SEG_GRAN(1) | SEG_PRIV(0) | \
                        SEG_CODE_R

#define GDT_DATA_PL0    SEG_PRES(1) | SEG_AVLS(0) | SEG_LONG(0) | \
                        SEG_SIZE(1) | SEG_GRAN(1) | SEG_PRIV(0) | \
                        SEG_DATA_W

#define GDT_CODE_PL3    SEG_PRES(1) | SEG_AVLS(0) | SEG_LONG(0) | \
                        SEG_SIZE(1) | SEG_GRAN(1) | SEG_PRIV(3) | \
                        SEG_CODE_R

#define GDT_DATA_PL3    SEG_PRES(1) | SEG_AVLS(0) | SEG_LONG(0) | \
                        SEG_SIZE(1) | SEG_GRAN(1) | SEG_PRIV(3) | \
                        SEG_DATA_W

#define GDT_TSS_PL0     SEG_PRES(1) | SEG_AVLS(0) | SEG_LONG(0) | \
                        SEG_SIZE(0) | SEG_GRAN(1) | SEG_PRIV(0) | \
                        SEG_SYS_TSS

#define GDT_TSS_PL3     SEG_PRES(1) | SEG_AVLS(0) | SEG_LONG(0) | \
                        SEG_SIZE(0) | SEG_GRAN(1) | SEG_PRIV(3) | \
                        SEG_SYS_TSS

typedef struct {
    uint32_t prev_task_link     : 16;
    uint32_t reserved0          : 16;
    uint32_t esp0;                      // Ring 0 esp
    uint32_t ss0                : 16;   // Ring 0 ss
    uint32_t reserved1          : 16;
    uint32_t esp1;                      // Ring 1 esp
    uint32_t ss1                : 16;   // Ring 1 ss
    uint32_t reserved2          : 16;
    uint32_t esp2;                      // Ring 2 esp
    uint32_t ss2                : 16;   // Ring 2 ss
    uint32_t reserved3          : 16;
    uint32_t cr3;                       // Page directory base
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint32_t es                 : 16;
    uint32_t reserved4          : 16;
    uint32_t cs                 : 16;
    uint32_t reserved5          : 16;
    uint32_t ss                 : 16;
    uint32_t reserved6          : 16;
    uint32_t ds                 : 16;
    uint32_t reserved7          : 16;
    uint32_t fs                 : 16;
    uint32_t reserved8          : 16;
    uint32_t gs                 : 16;
    uint32_t reserved9          : 16;
    uint32_t ldt_selector       : 16;
    uint32_t reserved10         : 16;
    uint32_t T                  : 1;
    uint32_t reserved11         : 15;
    uint32_t io_map_base        : 16;
} tss_t;

typedef union {
    struct {
        uint32_t rpl    : 2;    // Requested Privilege Level
        uint32_t ti     : 1;    // Table Indicator (0: GDT, 1: LDT)
        uint32_t index  : 13;   // Table index
    };
    uint32_t raw;
} gdt_sel_t;

// TODO make this static and add routines to modify esp0, etc.
extern tss_t TSS;       // Task State Segment

void gdt_init(void);

#endif // _KERNEL_GDT_H
