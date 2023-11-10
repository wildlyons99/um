/* registers.c */

#include <stdint.h>


/* Registers_init
 * Purpose: 
 *      Initialize a new register file where each register contains 0
 * Arguments:
 *      (unsigned) num_registers - The number of registers in the file
 * Returns:
 *      A pointer to an array of registers
 * Notes:
 *      - CRE for num_registers to be 0
 *      - CRE if memory cannot be allocated for registers
 *      - All registers start initialized to 0
 *      - Makes registers which hold 32-bit values
 */
uint32_t *Registers_init(unsigned num_registers); 


/* register_free
 * Purpose: 
 *      To free the registers instance
 * Argument: 
 *      (uint32_t **) register_p - Pointer to the address of the 
                                   registers to free
 * Notes:
 *      - CRE for register_p or *register_p to be NULL
 *      - Sets *register_p to be null
 */
void register_free(uint32_t **register_p); 