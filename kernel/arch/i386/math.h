/**
 * math.h: Mathematical operations
 * 
 * TODO The current implementation keeps makes a local copy of this library for
 * each object file during compilation. Does this make sense? Is there a
 * performance gain with this approach (e.g. better cache locality or compiler
 * optimizations like inlining for these static functions)?
 */

#ifndef _KERNEL_MATH_H
#define _KERNEL_MATH_H

// int powers of 10
static const int iEXP10[] = {1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000};

/**
 * Integral log base 10 of int
 * TODO efficiency
 */
static int ilog10u(unsigned int v)
{
    return (v >= 1000000000) ? 9 : (v >= 100000000) ? 8 : (v >= 10000000) ? 7 :
        (v >= 1000000) ? 6 : (v >= 100000) ? 5 : (v >= 10000) ? 4 : (v >= 1000)
        ? 3 : (v >= 100) ? 2 : (v >= 10) ? 1 : 0;
}

/**
 * Return integral log base 2 of unsigned int
 * TODO efficiency
 */
static int ilog2u(unsigned int v)
{
    int i = 0;
    while (v >>= 1)
        i++;
    return i;
}

#endif // _KERNEL_MATH_H
