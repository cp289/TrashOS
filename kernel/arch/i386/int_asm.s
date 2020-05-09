# int.s: ASM routines for interrupt handling

.globl  int_handle_proc_switch
.type   int_handle_proc_switch, @function
int_handle_proc_switch:

    # Save context
    movl    %edi,  -4(%esp)
    movl    %esi,  -8(%esp)
    movl    %ebp, -12(%esp)
    movl    %ebx, -16(%esp)
    movl    %edx, -20(%esp)
    movl    %ecx, -24(%esp)
    movl    %eax, -28(%esp)
    movl    $0,   -32(%esp)
    movl    $0,   -36(%esp)
    movl    $0,   -40(%esp)
    movl    $0,   -44(%esp)
    movw    %gs,  -32(%esp)
    movw    %fs,  -36(%esp)
    movw    %es,  -40(%esp)
    movw    %ds,  -44(%esp)

    # Setup function parameter (pointer to above proc_frame_t)
    leal    -44(%esp), %eax
    movl    %eax, -48(%esp)

    # Update stack pointer
    subl    $48, %esp

    # TODO move proc_store_ctxt() and proc_load_ctxt() inside proc_next()?
    # Store current process context
    call proc_store_ctxt

    # Switch to next process
    call proc_next

    # Load next process context
    call proc_load_ctxt

    # Signal End Of Interrupt
    call lapic_send_eoi

    # Restore stack pointer
    addl    $48, %esp

    # Load new process context
    movl    -4(%esp), %edi
    movl    -8(%esp), %esi
    movl   -12(%esp), %ebp
    movl   -16(%esp), %ebx
    movl   -20(%esp), %edx
    movl   -24(%esp), %ecx
    movl   -28(%esp), %eax
    movw   -32(%esp),  %gs
    movw   -36(%esp),  %fs
    movw   -40(%esp),  %es
    movw   -44(%esp),  %ds

    # Return from interrupt
    iret
