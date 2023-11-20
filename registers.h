/* registers.h
 * Authors: Tom Lyons     tlyons01
 *          Noah Stiegler nstieg01
 * Date: November 20, 2023
 * CS40 HW 6 Universal Machine
 * 
 * Our registers module is in charge of representing the data stored in the 
 * general purpose registers of a 32-bit word-addressable machine
 */

#ifndef REGISTERS_H
#define REGISTERS_H

#include <stdint.h>

typedef struct Registers_T *Registers_T;

extern Registers_T Registers_new(unsigned num_registers); 
extern void Registers_set(Registers_T regs, unsigned reg_index, uint32_t value);
extern uint32_t Registers_get(Registers_T regs, unsigned reg_index);
extern void Registers_free(Registers_T *register_p);

#endif