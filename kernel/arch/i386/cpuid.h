/**
 * cpuid.h: CPUID and other feature-detection routines
 */

#ifndef _KERNEL_CPUID_H
#define _KERNEL_CPUID_H

#include "std.h"

static inline void cpuid_id_string(char *id_string_buffer)
{
    asm (
        "movl $0, %%eax\n\t"
        "cpuid\n\t"
        "movl %%ebx, %0\n\t"
        "movl %%edx, %1\n\t"
        "movl %%ecx, %2\n\t"
        : "=rm" ( ((reg_t*)id_string_buffer)[0] ),
          "=rm" ( ((reg_t*)id_string_buffer)[1] ),
          "=rm" ( ((reg_t*)id_string_buffer)[2] )
        : // No inputs
        : "eax", "ebx", "edx", "ecx"
    );

    id_string_buffer[3 * sizeof(reg_t)] = 0;
}

#endif // _KERNEL_CPUID_H
