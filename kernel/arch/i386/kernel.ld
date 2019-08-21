/**
 * kernel.ld: Linker file for the kernel
 */

ENTRY(_start);

SECTIONS
{
    /**
     * TODO The following line would create a virtual higher-half address space,
     * leaving 512M for the kernel.
     * KERNEL_VMA = 0xE0000000;
     */

    KERNEL_LMA = 1M;
    KERNEL_VMA = 1M;

    . = KERNEL_VMA;

    /**
     * Each section is aligned to a 4K (page) boundary. The AT function
     * instructs the linker to load each section starting at KERNEL_LMA.
     */

    .text ALIGN(4K) : AT(ADDR(.text) - KERNEL_VMA + KERNEL_LMA)
    {
        *(.multiboot2)
        *(.text)
    }

    .rodata ALIGN(4K) : AT(ADDR(.rodata) - KERNEL_VMA + KERNEL_LMA)
    {
        *(.rodata)
    }

    .data ALIGN(4K) : AT(ADDR(.data) - KERNEL_VMA + KERNEL_LMA)
    {
        *(.data)
    }

    .bss ALIGN(4K) : AT(ADDR(.bss) - KERNEL_VMA + KERNEL_LMA)
    {
        *(COMMON)
        *(.bss)
    }
}
