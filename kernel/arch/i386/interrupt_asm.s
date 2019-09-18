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
