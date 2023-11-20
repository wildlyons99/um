/* registers.c
 * Authors: Tom Lyons     tlyons01
 *          Noah Stiegler nstieg01
 * Date: November 20, 2023
 * CS40 HW 6 Universal Machine
 * 
 * Our registers module is in charge of representing the data stored in the 
 * general purpose registers of a 32-bit word-addressable machine
 * 
 */

/* C Std Lib */
#include <stdlib.h>

/* Hanson Libs */
#include <assert.h>
#include <mem.h>

/* Header */
#include "registers.h"

/* The registers implementation */
struct Registers_T {
        unsigned num_registers; /* How many registers there are */
        uint32_t *data;        /* the data in the registers */
};


/* Registers_new
 * Purpose: 
 *      Initialize a new register file where each register contains 0
 * Arguments:
 *      (unsigned) num_registers - The number of registers in the file
 * Returns:
 *      (Registers_T) A new Registers_T with num_registers registers
 * Notes:
 *      - CRE for num_registers to be 0
 *      - CRE if memory cannot be allocated for registers
 *      - All registers start initialized to 0
 *      - Makes registers which hold 32-bit values
 */
Registers_T Registers_new(unsigned num_registers)
{
        assert(num_registers > 0);

        Registers_T new_reg = NEW(new_reg);
        new_reg->num_registers = num_registers; 
        new_reg->data = CALLOC(num_registers, sizeof(uint32_t)); 

        return new_reg;
} 

/* Registers_get
 * Purpose:
 *      Get a value in a register
 * Arguments:
 *      (Registers_T) regs - The registers to get the value from
 *      (unsigned) reg_index - Which register to get the value from
 * Notes:
 *      - CRE if regs or regs->data is NULL
 *      - CRE if reg_index refers to a register whose index is larger than
 *        the number of registers
 */
uint32_t Registers_get(Registers_T regs, unsigned reg_index)
{
        /* check inputs */
        assert(regs != NULL); 
        assert(regs->data != NULL); 
        assert(reg_index < regs->num_registers); 
        
        return regs->data[reg_index]; 
}


/* Registers_set
 * Purpose:
 *      Set a value in a register
 * Arguments:
 *      (Registers_T) regs - The registers to get the value from
 *      (unsigned) reg_index - Which register to get the value from
 *      (uint32_t) value - The value to put into the register
 * Notes:
 *      - CRE if regs or regs->data is NULL
 *      - CRE if reg_index refers to a register whose index is larger than
 *        the number of registers
 */
void Registers_set(Registers_T regs, unsigned reg_index, uint32_t value)
{
        /* check inputs */
        assert(regs != NULL); 
        assert(regs->data != NULL); 
        assert(reg_index < regs->num_registers); 
        
        regs->data[reg_index] = value; 
}


/* Registers_free
 * Purpose: 
 *      To free the registers instance
 * Argument: 
 *      (Registers_T *) register_p - Address of the Registers_T to free
 * Notes:
 *      - CRE for register_p or *register_p to be NULL
 *      - Sets *register_p to be null
 */
void Registers_free(Registers_T *register_p)
{
        assert(register_p != NULL);
        Registers_T registers = *register_p;
        assert(registers != NULL);
        
        FREE(registers->data);
        FREE(registers);
        *register_p = NULL;
}