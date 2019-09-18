/**
 * cr.h: Control registers
 */

#ifndef _KERNEL_CR_H
#define _KERNEL_CR_H

#include "std.h"

uint32_t get_cr0(void);
// NOTE: CR1 is reserved and any access throws an exception
uint32_t get_cr1(void);
uint32_t get_cr2(void);
uint32_t get_cr3(void);
uint32_t get_cr4(void);

#endif // _KERNEL_CR_H
