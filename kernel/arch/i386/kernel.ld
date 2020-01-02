/**
 * kernel.ld: Linker file for the kernel
 */

ENTRY(_start);

SECTIONS
{
    /**
     * The following creates a virtual higher-half address space, leaving 512M
     * for the kernel.
     */
    _KERNEL_LMA = 1M;
    _KERNEL_VMA = 0xE0000000;

    /**
     * Each section is aligned to a 4K (page) boundary. The AT function
     * instructs the linker to load each section starting at KERNEL_LMA.
     */

    . = _KERNEL_LMA;

    .init ALIGN(4K) :
    {
        _INIT_START = ABSOLUTE(.);

        *(.multiboot2)
        *(.init)

        kernel/arch/i386/init.o
        kernel/arch/i386/init_printk.o
        kernel/arch/i386/init_vga.o
        (.text .rodata* .data)

        _INIT_END = ALIGN(4K);
        _INIT_SIZE = _INIT_END - _INIT_START;
    }

    .init_bss ALIGN(4K) :
    {
        _INIT_BSS_START = ABSOLUTE(.);

        *(.init_bss)

        kernel/arch/i386/init.o
        kernel/arch/i386/init_printk.o
        kernel/arch/i386/init_vga.o
        (COMMON .bss)

        _INIT_BSS_END = ALIGN(4K);
        _INIT_BSS_SIZE = _INIT_BSS_END - _INIT_BSS_START;
    }


    /* Physical address offset for start of kernel */
    /* _LMA_OFFSET = _KERNEL_LMA + _INIT_SIZE + _INIT_BSS_SIZE;*/
    _LMA_OFFSET = _INIT_BSS_END;

    . = _KERNEL_VMA;

    .text ALIGN(4K) : AT(ADDR(.text) - _KERNEL_VMA + _LMA_OFFSET)
    {
        _TEXT_START = ABSOLUTE(.);

        EXCLUDE_FILE (
            *kernel/arch/i386/init.o
            *kernel/arch/i386/init_printk.o
            *kernel/arch/i386/init_vga.o
        ) *(.text)

        _TEXT_END = ALIGN(4K);
        _TEXT_SIZE = _TEXT_END - _TEXT_START;
    }

    .rodata ALIGN(4K) : AT(ADDR(.rodata) - _KERNEL_VMA + _LMA_OFFSET)
    {
        _RODATA_START = ABSOLUTE(.);

        EXCLUDE_FILE (
            *kernel/arch/i386/init.o
            *kernel/arch/i386/init_printk.o
            *kernel/arch/i386/init_vga.o
        ) *(.rodata*)

        _RODATA_END = ALIGN(4K);
        _RODATA_SIZE = _RODATA_END - _RODATA_START;
    }

    .data ALIGN(4K) : AT(ADDR(.data) - _KERNEL_VMA + _LMA_OFFSET)
    {
        _DATA_START = ABSOLUTE(.);

        EXCLUDE_FILE (
            *kernel/arch/i386/init.o
            *kernel/arch/i386/init_printk.o
            *kernel/arch/i386/init_vga.o
        ) *(.data)

        _DATA_END = ALIGN(4K);
        _DATA_SIZE = _DATA_END - _DATA_START;
    }

    .bss ALIGN(4K) : AT(ADDR(.bss) - _KERNEL_VMA + _LMA_OFFSET)
    {
        _BSS_START = ABSOLUTE(.);

        EXCLUDE_FILE (
            *kernel/arch/i386/init.o
            *kernel/arch/i386/init_printk.o
            *kernel/arch/i386/init_vga.o
        ) *(COMMON)
        EXCLUDE_FILE (
            *kernel/arch/i386/init.o
            *kernel/arch/i386/init_printk.o
            *kernel/arch/i386/init_vga.o
        ) *(.bss)

        _BSS_END = ALIGN(4K);
        _BSS_SIZE = _BSS_END - _BSS_START;
    }

    _KERNEL_END = ALIGN(4K);
    _KERNEL_SIZE = _KERNEL_END - _KERNEL_VMA;
}
