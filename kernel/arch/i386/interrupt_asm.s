## interrupt_asm.s: Interrupts

## IDTR Register
#   bits 47:16  IDT base address
#   bits 15:0   IDT limit (byte size of IDT)

.section .data
.align 8
idt:
    .word 0
    .long 0

.section .text

.global idtr_load
.type idtr_load, @function
idtr_load: # (void *base, uint16_t limit)
    movl    4(%esp),    %eax
    movl    %eax,       idt+2
    movw    8(%esp),    %eax
    movw    %ax,        idt
    lidt    idt
    ret

.global catch_interrupt
.type catch_interrupt, @function
catch_interrupt:

    # Make new stack frame
    pushl   %ebp
    movl    %esp,   %ebp

    # Copy CPU interrupt values to new stack frame to be parameters for
    # handle_interrupt()
    pushl    12(%ebp)
    pushl     8(%ebp)
    pushl     4(%ebp)

    # Since the PUSHA instruction is a bit wasteful (it pushes the base pointer
    # and stack pointer), we save all important registers manually
    pushl   %eax
    pushl   %ecx
    pushl   %edx
    pushl   %ebx
    pushl   %esi
    pushl   %edi

    call    handle_interrupt

    # Restore important registers
    popl    %edi
    popl    %esi
    popl    %ebx
    popl    %edx
    popl    %ecx
    popl    %eax

    addl    $12,    %esp

    # Restore old stack frame
    popl    %ebp

    iret

.global catch_exception
.type catch_exception, @function
catch_exception:

    # Make new stack frame
    pushl   %ebp
    movl    %esp,   %ebp

    # Copy CPU interrupt values to new stack frame to be parameters for
    # handle_exception(). In this case, we expect an error code to be the last
    # value pushed to the stack (located at 4(%ebp) ).
    pushl    16(%ebp)
    pushl    12(%ebp)
    pushl     8(%ebp)
    pushl     4(%ebp)

    # Since the PUSHA instruction is a bit wasteful (it pushes the base pointer
    # and stack pointer), we save all important registers manually
    pushl   %eax
    pushl   %ecx
    pushl   %edx
    pushl   %ebx
    pushl   %esi
    pushl   %edi

    call    handle_exception

    # Restore important registers
    popl    %edi
    popl    %esi
    popl    %ebx
    popl    %edx
    popl    %ecx
    popl    %eax

    addl    $16,    %esp

    # Restore old stack frame
    popl    %ebp

    iret
