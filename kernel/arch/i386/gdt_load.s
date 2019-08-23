## gdt_load.s: load Global Descriptor Table

## Notes on segment selectors
#
# Segment selectors are the values loaded into the processor's segment registers
# (CS, DS, ES, FS, GS, SS) with the following format:
#   bits 15:3 ->    index (the index into the GDT given the declaration: uint64_t
#                   GDT[])
#   bit     2 ->    0 = GDT,    1 = LDT (Local Descriptor Table)
#   bits  1:0 ->    Requested Privelege Level (ring value)
#
# For official documentation on segment selectors, see Intel Software
# Development Manual, Vol. 3A, section 3.4.1.

.section .data

# This is the 6 byte value to load into the GDT register with the `lgdt`
# instruction
.align 4
gdt_desc:
    .word   0   # Size of GDT
    .long   0   # Address of GDT

.section .text

.global gdt_load
.type gdt_load, @function
gdt_load: #(gdt_t *gdt, size_t size);

    mov     4(%esp),    %eax
    mov     %eax,       2+gdt_desc
    mov     8(%esp),    %ax
    mov     %ax,        gdt_desc
    lgdt    gdt_desc

    # Load selector for ring 0 pointing to entry 1 of GDT
    ljmp    $0x08,      $gdt_reload_cs

gdt_reload_cs:
    # Load selector for ring 0 pointing to entry 2 of GDT
    mov     $0x10,  %ax
    mov     %ax,    %ds
    mov     %ax,    %es
    mov     %ax,    %fs
    mov     %ax,    %gs
    mov     %ax,    %ss
    ret
