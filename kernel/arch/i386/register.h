/**
 * register.h: routines for obtaining/setting register values
 *
 * TODO define routines here with inline C functions with inline assembler and
 * delete register.s
 */

#ifndef _KERNEL_REGISTER_H
#define _KERNEL_REGISTER_H

#include <stdint.h>

// Dump segment registers
uint16_t dump_cs(void);
uint16_t dump_es(void);
uint16_t dump_fs(void);
uint16_t dump_gs(void);
uint16_t dump_ss(void);

#endif // _KERNEL_REGISTER_H
