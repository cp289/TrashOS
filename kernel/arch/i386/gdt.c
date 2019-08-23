/**
 * gdt_init.c: Global Descriptor Table
 *
 * For documentation, see: Intel Software Development Manual 3A 3-9
 */

#include <stddef.h>
#include <stdint.h>

#include "gdt.h"

// Global Descriptor Table
extern gdt_t GDT[5];
gdt_t GDT[5];

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
    // Create null descriptor
    GDT[0] = gdt_descriptor(0, 0, 0);
    // Open up entire 4GiB address space to rings 0 and 3
    GDT[1] = gdt_descriptor(0, 0xfffff, GDT_CODE_PL0);
    GDT[2] = gdt_descriptor(0, 0xfffff, GDT_DATA_PL0);
    GDT[3] = gdt_descriptor(0, 0xfffff, GDT_CODE_PL3);
    GDT[4] = gdt_descriptor(0, 0xfffff, GDT_DATA_PL3);

    gdt_load(GDT, sizeof(GDT));
}
