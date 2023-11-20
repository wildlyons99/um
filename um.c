/* um.c 
 * Authors: Tom Lyons     tlyons01
 *          Noah Stiegler nsteig01
 * CS40 HW6 UM
 * Tufts University
 * 11/20/2023
 * 
 * The UM module is the overarching module that handles the fetch, decode, 
 * and execute operations of the universal machine. It is a client of the 
 * registers and memory modules, as well as the decode module.
 *
 * The UM module takes the name of a file containing Universal Machine
 * assembly instructions as an argument. It loads that program and runs it
 * 
 */

/* Standard Libs */
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

/* Hanson Libs */
#include <assert.h>

/* Our Modules */
#include "segmem.h"
#include "registers.h"
#include "decode.h"

/* UM Parameters */
static const int NUM_REGISTERS = 8;

/* Private helper functions */
void Um_run(FILE *input);
static void print_usage();
void execute(SegMem_T mem, Registers_T regs, Um_opcode opcode, 
             unsigned rA, unsigned rB, unsigned rC,
             unsigned loadval_rA, unsigned loadval_value);

int main(int argc, char *argv[])
{
        /* Check the right number of input arguments */
        if (argc != 2) {
                print_usage();
        }
        
        /* Open the file passed in and check it */
        FILE *input = fopen(argv[1], "r"); 
        if (input == NULL) {
                fprintf(stderr, "%s: No such file or directory\n", argv[1]);
                exit(EXIT_FAILURE);
        }

        /* Give um the opened file */
        Um_run(input);

        /* Close the file */
        fclose(input); 
        
        return EXIT_SUCCESS; 
}


/* print_usage
 * Purpose:
 *      Prints the usage for um
 * Notes:
 *      - Prints to stderr and exits program without freeing memory
 */
static void print_usage()
{
        fprintf(stderr, "Usage: ./um [um_program.um]\n");
        exit(EXIT_FAILURE);
}



/* Um_run
 * Purpose: 
 *      Run the UM emulator on the code stored in the opened file passed in
 * Arguments:
 *      (FILE *) input - An opened file to read the program from
 * Notes:
 *      - CRE for input to be NULL
 *      - Assumes input is already open
 */
void Um_run(FILE *input)
{
        assert(input != NULL); 
        
        /* Initialize the memory and registers */
        SegMem_T memory = SegMem_new(input);
        Registers_T registers = Registers_new(NUM_REGISTERS);

        /* Fetch, decode, execute! */
        Um_opcode opcode;
        do {
                /* Fetch an instruction */
                word_t instruction = SegMem_fetch_next_i(memory);
                
                /* Decode it */
                unsigned rA, rB, rC, loadval_rA;
                uint32_t loadval_value;
                opcode = decode_word(instruction, &rA, &rB, &rC, &loadval_rA, 
                                       &loadval_value);

                /* Do it */
                execute(memory, registers, opcode, 
                        rA, rB, rC, 
                        loadval_rA, loadval_value);
        } while (opcode != HALT); 

        SegMem_free(&memory); 
        Registers_free(&registers);
}

/* execute
 * Purpose:
 *      Execute the UM instruction passed in with given parameters on the 
 *      memory and registers passed in
 * Arguments: 
 *      (SegMem_T) mem - The memory to perform operations on
 *      (Registers_T) regs - The registers to perform operations on
 *      (Um_opcode) opcode - The code of the operation to perform
 *      (unsigned) rA - Which register is register A for 3 register instructions
 *      (unsigned) rB - Which register is register B for 3 register instructions
 *      (unsigned) rC - Which register is register C for 3 register instructions
 *      (unsigned) loadval_rA - Which register is register A for a loadval
 *                              instruction
 *      (unsigned) load_value - If the instruction is loadval, what value to
 *                              load
 * Notes: 
 *      - CRE for mem or regs to be NULL
 */
void execute(SegMem_T mem, Registers_T regs, Um_opcode opcode, 
             unsigned rA, unsigned rB, unsigned rC, 
             unsigned loadval_rA, unsigned loadval_value)
{
        assert(mem != NULL); 
        assert(regs != NULL); 

        /* Get the values of the registers we care about for 3-register 
         * instructions */
        word_t rA_val = Registers_get(regs, rA);
        word_t rB_val = Registers_get(regs, rB);
        word_t rC_val = Registers_get(regs, rC);
        
        /* Performs the operation based on the opcode */
        word_t word, seg_id;
        switch (opcode) {
        case CMOV:
                if (rC_val != 0) {
                        Registers_set(regs, rA, rB_val);
                }
                break; 
        case SLOAD:
                word = SegMem_get_word(mem, rB_val, rC_val); 
                Registers_set(regs, rA, word);
                break; 
        case SSTORE:
                SegMem_put_word(mem, rA_val, rB_val, rC_val);
                break; 
        case ADD:
                Registers_set(regs, rA, rB_val + rC_val);
                break; 
        case MUL:
                Registers_set(regs, rA, rB_val * rC_val);
                break; 
        case DIV:
                /* don't check for div by 0 for performance */
                Registers_set(regs, rA, rB_val / rC_val); 
                break; 
        case NAND:
                Registers_set(regs, rA, ~(rB_val & rC_val)); 
                break; 
        case HALT: 
                /* do nothing, let loop end */
                break; 
        case MAP:
                seg_id = SegMem_map(mem, rC_val);
                Registers_set(regs, rB, seg_id);
                break; 
        case UNMAP:
                SegMem_unmap(mem, rC_val);
                break; 
        case OUT:
                /* URE to out a value larger than 255 */
                printf("%c", (char)rC_val); 
                break; 
        case IN:
                /* Input is [0, 255] */
                Registers_set(regs, rC, (char)getchar());
                break; 
        case LOADP:
                SegMem_load_program(mem, rB_val, rC_val);
                break; 
        case LV:
                Registers_set(regs, loadval_rA, loadval_value);
                break;
        default:
                fprintf(stderr, "Instruction opcode not valid: %u\n", opcode);
                assert(false);
        }
}