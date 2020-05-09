/**
 * gdt_init.c: Global Descriptor Table
 *
 * For documentation, see: Intel Software Development Manual 3A 3-9
 *
 *
 * Notes on segment selectors
 *
 * Segment selectors are the values loaded into the processor's segment registers
 * (CS, DS, ES, FS, GS, SS) with the following format:
 *   bits 15:3 ->    index (the index into the GDT given the declaration: uint64_t
 *                   GDT[])
 *   bit     2 ->    0 = GDT,    1 = LDT (Local Descriptor Table)
 *   bits  1:0 ->    Requested Privilege Level (ring value)
 *
 * For official documentation on segment selectors, see Intel Software
 * Development Manual, Vol. 3A, section 3.4.1.
 */

#include "asm.h"
#include "gdt.h"

static gdt_t GDT[GDT_IDX_LIMIT];    // Global Descriptor Table
tss_t TSS = { 0 };                  // Task State Segment
static uint64_t gdt_sel;

// Returns gdt descriptor
static uint64_t gdt_descriptor(uint32_t base, uint32_t limit, uint16_t flags)
{
    uint64_t descriptor;
    descriptor  = limit         & 0x000f0000;
    descriptor |= (flags << 8)  & 0x00f0ff00;
    descriptor |= (base >> 16)  & 0x000000ff;
    descriptor |= base          & 0xff000000;

    descriptor <<= 32;
    descriptor |= limit & 0x0000ffff;
    descriptor |= base  << 16;

    return descriptor;
}

/**
 * Initialize GDT
 */
void gdt_init(void)
{
    // Create null descriptor
    GDT[GDT_IDX_NULL] = gdt_descriptor(0, 0, 0);

    // Open up entire 4GiB address space to rings 0 and 3
    GDT[GDT_IDX_CODE_PL0] = gdt_descriptor(0, 0xfffff, GDT_CODE_PL0);
    GDT[GDT_IDX_DATA_PL0] = gdt_descriptor(0, 0xfffff, GDT_DATA_PL0);
    GDT[GDT_IDX_TSS_PL0]  = gdt_descriptor((uintptr_t)&TSS, sizeof(TSS), GDT_TSS_PL0);
    GDT[GDT_IDX_CODE_PL3] = gdt_descriptor(0, 0xfffff, GDT_CODE_PL3);
    GDT[GDT_IDX_DATA_PL3] = gdt_descriptor(0, 0xfffff, GDT_DATA_PL3);
    GDT[GDT_IDX_TSS_PL3]  = gdt_descriptor((uintptr_t)&TSS, sizeof(TSS), GDT_TSS_PL3);

    TSS.ss0  = GDT_SEL_DATA_PL0;
    TSS.esp0 = (uintptr_t)0;

    gdt_sel = ((uint64_t)(uintptr_t)&GDT << 16) | (uint64_t)sizeof(GDT);
    load_gdt(&gdt_sel);

    set_cs(GDT_SEL_CODE_PL0);
    set_ds(GDT_SEL_DATA_PL0);
    set_es(GDT_SEL_DATA_PL0);
    set_fs(GDT_SEL_DATA_PL0);
    set_gs(GDT_SEL_DATA_PL0);
    set_ss(GDT_SEL_DATA_PL0);
    set_tr(GDT_SEL_TSS_PL3);
}
