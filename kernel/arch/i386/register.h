/**
 * register.h: routines for obtaining/setting register values
 *
 * TODO define routines here with inline C functions with inline assembler and
 * delete register.s
 */

#ifndef _KERNEL_REGISTER_H
#define _KERNEL_REGISTER_H

#include "std.h"

uint32_t dump_eflags(void);

// Dump control registers
uint32_t dump_cr0(void);
uint32_t dump_cr1(void);
uint32_t dump_cr2(void);
uint32_t dump_cr3(void);
uint32_t dump_cr4(void);

// Dump segment registers
uint16_t dump_cs(void);
uint16_t dump_ds(void);
uint16_t dump_es(void);
uint16_t dump_fs(void);
uint16_t dump_gs(void);
uint16_t dump_ss(void);

#endif // _KERNEL_REGISTER_H

/**
 * Control registers
 * 
 * CR4:
 *  bits 31:23  Reserved
 *  bits 
 */
