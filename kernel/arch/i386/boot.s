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

    # Setup paging
    #call page_init

    # Begin kernel
    call kernel_main

    # Place system into infinite loop

    # Disable interrupts
    cli
    # Wait for next interrupt (there are none; this locks the system)
1:  hlt
    # Jump to halt if non-maskable interrupt or system management mode occurs
    jmp 1b

# Set the size of the start symbol (useful for debugging)
.size _start, . - _start
