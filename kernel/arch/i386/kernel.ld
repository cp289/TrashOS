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
        *(.multiboot2)
        *(.init)
        kernel/arch/i386/init.o (.text .rodata* .data)
    }

    .init_bss ALIGN(4K) :
    {
        *(.init_bss)
        kernel/arch/i386/init.o (COMMON .bss)
    }

    _INIT_SIZE = ALIGN(4K) - _KERNEL_LMA;
    . = _KERNEL_VMA;

    .text ALIGN(4K) : AT(ADDR(.text) - _KERNEL_VMA + _KERNEL_LMA + _INIT_SIZE)
    {
        EXCLUDE_FILE (*kernel/arch/i386/init.o) *(.text)
    }

    .rodata ALIGN(4K) : AT(ADDR(.rodata) - _KERNEL_VMA + _KERNEL_LMA + _INIT_SIZE)
    {
        EXCLUDE_FILE (*kernel/arch/i386/init.o) *(.rodata*)
    }

    .data ALIGN(4K) : AT(ADDR(.data) - _KERNEL_VMA + _KERNEL_LMA + _INIT_SIZE)
    {
        EXCLUDE_FILE (*kernel/arch/i386/init.o) *(.data)
    }

    .bss ALIGN(4K) : AT(ADDR(.bss) - _KERNEL_VMA + _KERNEL_LMA + _INIT_SIZE)
    {
        EXCLUDE_FILE (*kernel/arch/i386/init.o) *(COMMON)
        EXCLUDE_FILE (*kernel/arch/i386/init.o) *(.bss)
    }

    _KERNEL_SIZE = . - _KERNEL_VMA;
}
