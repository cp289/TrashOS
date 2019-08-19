/**
 * kernel.ld: Linker file for the kernel
 */

ENTRY(_start);

SECTIONS
{
    . = 1M;

    /**
     * NOTE: the expression `BLOCK(4K) : ALIGN(4K)` simply aligns the output
     * section to a 4K byte boundary. According to the documentation of ld, this
     * is probably redundant. The expression `ALIGN(4K) :` is shorter and
     * probably does the same thing (except perhaps on some legacy systems).
     */
    .text BLOCK(4K) : ALIGN(4K)
    {
        *(.multiboot2)
        *(.text)
    }

    .rodata BLOCK(4K) : ALIGN(4K)
    {
        *(.rodata)
    }

    .data BLOCK(4K) : ALIGN(4K)
    {
        *(.data)
    }

    .bss BLOCK(4K) : ALIGN(4K)
    {
        *(COMMON)
        *(.bss)
    }
}

