## boot.s: Assembly bootstrap file

.section .bss
.align 16
stack_bottom:
.skip 16384     # 16 KiB
stack_top:

.section .text
.global _start
.type _start, @function
_start:

    # Setup stack
    mov $stack_top, %esp

    # Begin kernel
    call kernel_main

# Set the size of the start symbol (useful for debugging)
.size _start, . - _start
