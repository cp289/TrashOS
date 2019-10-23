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
 *   bits  1:0 ->    Requested Privelege Level (ring value)
 *
 * For official documentation on segment selectors, see Intel Software
 * Development Manual, Vol. 3A, section 3.4.1.
 */

#include "gdt.h"
#include "std.h"

// Global Descriptor Table
extern gdt_t GDT[5];
gdt_t GDT[5];

// This is the 6 byte value to load into the GDT register with the `lgdt`
// instruction
static uint64_t gdt_desc;

void gdt_load(gdt_t *gdt, size_t size);

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
    gdt_desc = ((uint64_t)(uintptr_t)GDT << 16) | sizeof(GDT);

    // Create null descriptor
    GDT[0] = gdt_descriptor(0, 0, 0);
    // Open up entire 4GiB address space to rings 0 and 3
    GDT[1] = gdt_descriptor(0, 0xfffff, GDT_CODE_PL0);
    GDT[2] = gdt_descriptor(0, 0xfffff, GDT_DATA_PL0);
    GDT[3] = gdt_descriptor(0, 0xfffff, GDT_CODE_PL3);
    GDT[4] = gdt_descriptor(0, 0xfffff, GDT_DATA_PL3);

    asm (
        "lgdt    (%0)\n\t"
        // Load selector for ring 0 pointing to entry 1 of GDT
        "ljmp    $0x08,      $gdt_reload_cs\n"

    "gdt_reload_cs:\n\t"
        // Load selector for ring 0 pointing to entry 2 of GDT
        "movw    %1,    %%ds\n\t"
        "movw    %1,    %%es\n\t"
        "movw    %1,    %%fs\n\t"
        "movw    %1,    %%gs\n\t"
        "movw    %1,    %%ss\n\t"
        : // No outputs
        : "r" (&gdt_desc),
          "rm" ((uint16_t)0x10)
        : // No clobbers
    );
}
