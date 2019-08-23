/**
 * gdt.h: Global Descriptor Table
 *
 * For documentation, see: Intel Software Development Manual 3A 3-9
 */

#ifndef _KERNEL_GDT_H
#define _KERNEL_GDT_H

#include <stdint.h>

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
 *          for code selectors (0 -> only executable from ring set in Privelege,
 *          1 -> executable from ring set in Privelege or any lower privelege
 *          ring)
 *  (43)    Executable bit: 0 -> data selector, 1 -> code selector
 *
 *  (44)    Descriptor type: 0 -> system segment, 1 -> code/data segment
 *  (46:45) Privelege:  ring level 0, 1, 2, or 3
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
#define SEG_TYPE(x) ((x) << 4)          // Descriptor type
#define SEG_PRIV(x) (((x) & 0x3) << 5)  // Privelege
#define SEG_PRES(x) ((x) << 7)          // Present bit
#define SEG_AVLS(x) ((x) << 12)         // Available for system use
#define SEG_LONG(x) ((x) << 13)         // Long mode
#define SEG_SIZE(x) ((x) << 14)         // Size
#define SEG_GRAN(x) ((x) << 15)         // Granularity

// Constants for segment descriptor bits 43:40
enum {
    SEG_DATA_,      // Read-only
    SEG_DATA_A,     // Read-only, accessed
    SEG_DATA_W,     // Read/write
    SEG_DATA_WA,    // Read/write, accessed
    SEG_DATA_D,     // Read-only, expand down
    SEG_DATA_DA,    // Read-only, expand down, accessed
    SEG_DATA_DW,    // Read/write, expand down
    SEG_DATA_DWA,   // Read/write, expand down, accessed
    SEG_CODE_,      // Execute-only
    SEG_CODE_A,     // Execute-only, accessed
    SEG_CODE_R,     // Execute/read
    SEG_CODE_RA,    // Execute/read, accessed
    SEG_CODE_C,     // Execute-only, conforming
    SEG_CODE_CA,    // Execute-only, conforming, accessed
    SEG_CODE_CR,    // Execute/read, conforming
    SEG_CODE_CRA,   // Execute/read, conforming, accessed
};

#define GDT_CODE_PL0    SEG_TYPE(1) | SEG_PRES(1) | SEG_AVLS(0) | \
                        SEG_LONG(0) | SEG_SIZE(1) | SEG_GRAN(1) | \
                        SEG_PRIV(0) | SEG_CODE_R

#define GDT_DATA_PL0    SEG_TYPE(1) | SEG_PRES(1) | SEG_AVLS(0) | \
                        SEG_LONG(0) | SEG_SIZE(1) | SEG_GRAN(1) | \
                        SEG_PRIV(0) | SEG_DATA_W

#define GDT_CODE_PL3    SEG_TYPE(1) | SEG_PRES(1) | SEG_AVLS(0) | \
                        SEG_LONG(0) | SEG_SIZE(1) | SEG_GRAN(1) | \
                        SEG_PRIV(3) | SEG_CODE_R

#define GDT_DATA_PL3    SEG_TYPE(1) | SEG_PRES(1) | SEG_AVLS(0) | \
                        SEG_LONG(0) | SEG_SIZE(1) | SEG_GRAN(1) | \
                        SEG_PRIV(3) | SEG_DATA_W

typedef uint64_t gdt_t;

void gdt_init(void);

#endif // _KERNEL_GDT_H
