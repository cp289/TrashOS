/**
 * io.h: I/O stuff
 */

#ifndef _KERNEL_IO_H
#define _KERNEL_IO_H

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

#endif // _KERNEL_IO_H
