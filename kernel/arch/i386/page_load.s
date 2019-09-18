/**
 * page_load.s: Load paging directories / tables
 */

.section .text

.global page_load_dir
.type page_load_dir, @function
page_load_dir:  # (void *page_dir)

    #push %ebp
    #mov %esp,           %ebp

    mov 4(%esp),        %eax
    mov %eax,           %cr3

    #mov %ebp,           %esp
    #pop %ebp
    ret

.global page_enable
.type page_enable, @function
page_enable:


    #push %ebp
    #mov %esp,           %ebp

    mov %cr0,           %eax
    or  $0x80000001,    %eax
    mov %eax,           %cr0

    #mov %ebp,           %esp
    #pop %ebp
    ret
