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

// Powers of 10
static const uint64_t EXP10[20] = {
    1UL,
    10UL,
    100UL,
    1000UL,
    10000UL,
    100000UL,
    1000000UL,
    10000000UL,
    100000000UL,
    1000000000UL,
    10000000000UL,
    100000000000UL,
    1000000000000UL,
    10000000000000UL,
    100000000000000UL,
    1000000000000000UL,
    10000000000000000UL,
    100000000000000000UL,
    1000000000000000000UL,
    10000000000000000000UL
};

/**
 * Integral log base 10 of int
 * TODO efficiency
 */
static int ilog10u(uint64_t v)
{
    return (v >= EXP10[19]) ? 19 :
        (v >= EXP10[18]) ? 18 :
        (v >= EXP10[17]) ? 17 :
        (v >= EXP10[16]) ? 16 :
        (v >= EXP10[15]) ? 15 :
        (v >= EXP10[14]) ? 14 :
        (v >= EXP10[13]) ? 13 :
        (v >= EXP10[12]) ? 12 :
        (v >= EXP10[11]) ? 11 :
        (v >= EXP10[10]) ? 10 :
        (v >= EXP10[9]) ? 9 :
        (v >= EXP10[8]) ? 8 :
        (v >= EXP10[7]) ? 7 :
        (v >= EXP10[6]) ? 6 :
        (v >= EXP10[5]) ? 5 :
        (v >= EXP10[4]) ? 4 :
        (v >= EXP10[3]) ? 3 :
        (v >= EXP10[2]) ? 2 :
        (v >= EXP10[1]) ? 1 : 0;
}

// TODO make this actually work
static uint64_t div64(uint64_t a, uint64_t b)
{
    uint64_t res;
    for (res = 0; b * res <= a; res++);
    return res - 1;
}

/**
 * Return integral log base 2 of unsigned int
 * TODO efficiency
 */
static int ilog2u(uint64_t v)
{
    int i = 0;
    while (v >>= 1)
        i++;
    return i;
}

#endif // _KERNEL_MATH_H
