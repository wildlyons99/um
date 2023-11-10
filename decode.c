/* decode.c*/

#include <stdint.h>

#include "decode.h"

/* decode_word
 * Purpose: 
 *      To decode a 32-bit um instruction into its component op code, 
 *      register(s) and possible value. It is up to the client of this function
 *      to determine which of these values to use and which to ignore based
 *      on the op-code of the word
 * Arguments: 
 *      (uint32_t) word - The instruction to decode
 *      (Um_register ) *rA - If a regular instruction, which register is A
 *      (Um_register) *rB - If a regular instruction, which register is B
 *      (Um_register) *rC - If a regular instruction, which register is C
 *      (Um_register) *load_rA - If a load value instruction, which is rA
 *      (uint32_t) *load_val - If a load value instruction, the value to be 
 *                             loaded into rA
 * Returns: 
 *      (Um_opcode) that specifies the operation to be performed in the UM
 */
Um_opcode decode_word(uint32_t word, Um_register *rA, Um_register *rB, 
                      Um_register *rC, Um_register *load_rA, 
                      uint32_t *load_val); 
