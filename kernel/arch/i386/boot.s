## boot.s: Setup temporary stack and jump to C kernel

.section .init_bss

.align 16
init_stack_bottom:
.skip 16384     # 16 KiB
init_stack_top:

.section .init

.global _start
.type _start, @function
_start:

    # Setup stack
    mov $init_stack_top, %esp

    # Configure virtual address space
    call init

    # Begin kernel
    call kernel_main

# Set the size of the start symbol (useful for debugging)
.size _start, . - _start
