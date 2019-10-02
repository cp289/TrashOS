/**
 * page_load.s: Load paging directories / tables
 */

.section .text

.global page_load_dir
.type page_load_dir, @function
page_load_dir:

    movl    4(%esp),    %eax
    movl    %eax,       %cr3

    ret

.global page_enable
.type page_enable, @function
page_enable:

    movl    %cr0,       %eax
    orl     $0x80000001,%eax
    movl    %eax,       %cr0

    ret
