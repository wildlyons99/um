/* decode.h
 * Authors: Tom Lyons     tlyons01
 *          Noah Stiegler nsteig01
 * CS40 HW6 UM
 * Tufts University
 * 11/20/2023
 * 
 * Exports a method for decoding a 32 bit word (containing an UM assembly 
 * instruction) into its component pieces (opcode, register indices, values) as
 * well as a type for expressing opcodes
 * 
 */

#ifndef DECODE_H
#define DECODE_H

#include <stdlib.h>
#include <stdint.h>

typedef enum Um_opcode {
        CMOV = 0, SLOAD, SSTORE, ADD, MUL, DIV,
        NAND, HALT, MAP, UNMAP, OUT, IN, LOADP, LV
} Um_opcode;

Um_opcode decode_word(uint32_t word, unsigned *rA, unsigned *rB, 
                      unsigned *rC, unsigned *load_rA, 
                      uint32_t *load_val); 

#endif