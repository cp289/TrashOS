/**
 * string.h: String library
 *
 * TODO The current implementation keeps makes a local copy of this library for
 * each object file during compilation. Does this make sense? Is there a
 * performance gain with this approach (e.g. better cache locality or compiler
 * optimizations like inlining for these static functions)?
 */

#ifndef _KERNEL_STRING_H
#define _KERNEL_STRING_H

static size_t strlen(const char* str)
{
    size_t len = 0;
    while (str[len]) len++;
    return len;
}

#endif // _KERNEL_STRING_H
