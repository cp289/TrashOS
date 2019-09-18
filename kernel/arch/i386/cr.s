## cr.s: x86 control registers

.section .text

.global get_cr0
.type get_cr0, @function
get_cr0:
    mov %cr0,   %eax
    ret

# NOTE: CR1 is reserved and any access throws an exception
.global get_cr1
.type get_cr1, @function
get_cr1:
    mov %cr1,   %eax
    ret

.global get_cr2
.type get_cr2, @function
get_cr2:
    mov %cr2,   %eax
    ret

.global get_cr3
.type get_cr3, @function
get_cr3:
    mov %cr3,   %eax
    ret

.global get_cr4
.type get_cr4, @function
get_cr4:
    mov %cr4,   %eax
    ret
