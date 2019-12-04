/**
 * io.h: I/O stuff
 */

#ifndef _KERNEL_IO_H
#define _KERNEL_IO_H

#include "std.h"

/**
 * Kernel printf function
 * RETURN
 *  success: number of bytes written
 *  failure: negative error code
 * TODO implement error codes and messages. Messages should be in an error
 * string table. The error code itself should be the negative index of the
 * corresponding message in the string table. Determine whether a global string
 * table (for the entire kernel) or a local string table makes more sense.
 */
int kprintf(const char *format, ... );

static inline void outb(uint16_t port, uint8_t byte)
{
    asm (
        "outb %0, %1\n\t"
        : // No outputs
        : "a" (byte),   // Use al
          "Nd" (port)   // Use 8-bit immediate or dx
        : // No clobbers
    );
}

// TODO will this work in general?
static inline void io_wait(void)
{
    // Wait for IO operation to complete by wasting an IO cycle
    // TODO port 0x80 is mapped to DMA Controller, LPC, PCI, or PCIe, so this
    // probably doesn't make sense in the long run
    asm (
        "outb %0, $0x80\n\t"
        : // No outputs
        : "a" (0)
        : // No clobbers
    );
}

#endif // _KERNEL_IO_H
