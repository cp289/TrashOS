## dump.s: dump register values
# TODO convert this file to inline C functions with inline assembler

.section .text

# TODO fix this
.global dump_eflags
.type dump_eflags, @function
dump_eflags:
    //mov %eflags,   %eax
    ret

# Segment registers

.global dump_cs
.type dump_cs, @function
dump_cs:
    mov %cs,    %eax
    ret

.global dump_ds
.type dump_ds, @function
dump_ds:
    mov %ds,    %eax
    ret

.global dump_es
.type dump_es, @function
dump_es:
    mov %es,    %eax
    ret

.global dump_fs
.type dump_fs, @function
dump_fs:
    mov %fs,    %eax
    ret

.global dump_gs
.type dump_gs, @function
dump_gs:
    mov %gs,    %eax
    ret

.global dump_ss
.type dump_ss, @function
dump_ss:
    mov %ss,    %eax
    ret
