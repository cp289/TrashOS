## boot.s: Setup temporary stack and jump to C kernel

.section .init_bss

.align 16
_INIT_STACK_BOTTOM:
.skip 16384     # 16 KiB
.global _INIT_STACK_TOP
_INIT_STACK_TOP:

.section .init

.global _start
.type _start, @function
_start:

    # Setup stack
    mov $_INIT_STACK_TOP, %esp

    # Configure virtual address space and call kernel_main
    call init

# Set the size of the start symbol (useful for debugging)
.size _start, . - _start
