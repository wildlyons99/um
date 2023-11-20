/* decode.c
 * Authors: Tom Lyons     tlyons01
 *          Noah Stiegler nsteig01
 * CS40 HW6 UM
 * Tufts University
 * 11/20/2023
 * 
 * Implements a method for decoding a 32 bit word (containing an UM assembly 
 * instruction) into its component pieces (opcode, register indices, values) 
 * 
 */

/* C Libs */
#include <stdint.h>
#include <bitpack.h>

/* Header */
#include "decode.h"

/* decode_word
 * Purpose: 
 *      Decodes a 32-bit um instruction into its component op code, 
 *      register(s) and possible value. It is up to the client of this function
 *      to determine which of these values to use and which to ignore based
 *      on the op-code of the word
 * Arguments: 
 *      (uint32_t) word - The instruction to decode
 *      ---- Parameters returning values by reference ----
 *      (unsigned) *rA - If a regular instruction, which register is A
 *      (unsigned) *rB - If a regular instruction, which register is B
 *      (unsigned) *rC - If a regular instruction, which register is C
 *      (unsigned) *load_rA - If a load value instruction, which is rA
 *      (uint32_t) *load_val - If a load value instruction, the value to be 
 *                             loaded into rA
 * Returns: 
 *      (Um_opcode) that specifies the operation to be performed in the UM
 * Notes: 
 *      - Register values passed in via paramters are passed by reference
 *        and are updated to the appropriate value within this function
 */
Um_opcode decode_word(uint32_t word, unsigned *rA, unsigned *rB, 
                      unsigned *rC, unsigned *load_rA, 
                      uint32_t *load_val)
{
        /* get opcode */
        Um_opcode opcode = Bitpack_getu(word, 4, 28); 

        /* get registers */
        *rA = Bitpack_getu(word, 3, 6);
        *rB = Bitpack_getu(word, 3, 3);
        *rC = Bitpack_getu(word, 3, 0);

        /* get loadval register and value */
        *load_rA = Bitpack_getu(word, 3, 25);
        *load_val = Bitpack_getu(word, 25, 0);

        return opcode;
}
