/*
 * umlab.c
 *
 * Functions to generate UM unit tests. Once complete, this module
 * should be augmented and then linked against umlabwrite.c to produce
 * a unit test writing program.
 *  
 * A unit test is a stream of UM instructions, represented as a Hanson
 * Seq_T of 32-bit words adhering to the UM's instruction format.  
 * 
 * Any additional functions and unit tests written for the lab go
 * here. 
 *  
 */


#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <seq.h>
#include <bitpack.h>


typedef uint32_t Um_instruction;
typedef enum Um_opcode {
        CMOV = 0, SLOAD, SSTORE, ADD, MUL, DIV,
        NAND, HALT, ACTIVATE, INACTIVATE, OUT, IN, LOADP, LV
} Um_opcode;


/* Functions that return the two instruction types */
static const unsigned RWIDTH = 3;
static const unsigned IWIDTH = 4;
Um_instruction three_register(Um_opcode op, int ra, int rb, int rc) 
{
        Um_instruction inst = 0x0;
        inst = Bitpack_newu(inst, RWIDTH, 0, rc);
        inst = Bitpack_newu(inst, RWIDTH, 3, rb);
        inst = Bitpack_newu(inst, RWIDTH, 6, ra); 
        return Bitpack_newu(inst, IWIDTH, 28, op); 
}

Um_instruction loadval(unsigned ra, unsigned val) {
        Um_instruction inst = 0x0;
        inst = Bitpack_newu(inst, IWIDTH, 28, LV);
        inst = Bitpack_newu(inst, RWIDTH, 25, ra);
        return Bitpack_newu(inst, 25, 0, val);
}


/* Wrapper functions for each of the instructions */

typedef enum Um_register { r0 = 0, r1, r2, r3, r4, r5, r6, r7 } Um_register;

static inline Um_instruction cmove(Um_register a, Um_register b, Um_register c) 
{
        return three_register(CMOV, a, b, c);
}

static inline Um_instruction sload(Um_register a, Um_register b, Um_register c) 
{
        return three_register(SLOAD, a, b, c);
}
static inline Um_instruction sstore(Um_register a, Um_register b, Um_register c) 
{
        return three_register(SSTORE, a, b, c);
}
static inline Um_instruction add(Um_register a, Um_register b, Um_register c) 
{
        return three_register(ADD, a, b, c);
}
static inline Um_instruction mul(Um_register a, Um_register b, Um_register c) 
{
        return three_register(MUL, a, b, c);
}
static inline Um_instruction div(Um_register a, Um_register b, Um_register c) 
{
        return three_register(DIV, a, b, c);
}
static inline Um_instruction nand(Um_register a, Um_register b, Um_register c) 
{
        return three_register(NAND, a, b, c);
}
static inline Um_instruction halt(void) 
{
        return three_register(HALT, 0, 0, 0);
}
static inline Um_instruction map(Um_register b, Um_register c) 
{
        return three_register(ACTIVATE, 0, b, c);
}
static inline Um_instruction unmap(Um_register c) 
{
        return three_register(INACTIVATE, 0, 0, c);
}

Um_instruction out(Um_register c)
{
        return three_register(OUT, 0, 0, c);
}

Um_instruction in(Um_register c)
{
        return three_register(IN, 0, 0, c);
}

static inline Um_instruction loadp(Um_register b, Um_register c) 
{
        return three_register(LOADP, 0, b, c);
}



/* Functions for working with streams */

static inline void append(Seq_T stream, Um_instruction inst)
{
        assert(sizeof(inst) <= sizeof(uintptr_t));
        Seq_addhi(stream, (void *)(uintptr_t)inst);
}

const uint32_t Um_word_width = 32;

void Um_write_sequence(FILE *output, Seq_T stream)
{
        assert(output != NULL && stream != NULL);
        int stream_length = Seq_length(stream);
        for (int i = 0; i < stream_length; i++) {
                Um_instruction inst = (uintptr_t)Seq_remlo(stream);
                for (int lsb = Um_word_width - 8; lsb >= 0; lsb -= 8) {
                        fputc(Bitpack_getu(inst, 8, lsb), output);
                }
        }
      
}


/* Unit tests for the UM */

void build_halt_test(Seq_T stream)
{
        append(stream, halt());
}

void build_halt_output_test(Seq_T stream)
{
        append(stream, out(r1));
        append(stream, halt());
}

void build_load_output_test(Seq_T stream)
{
        append(stream, loadval(r1, 3 + '0'));
        append(stream, out(r1));
        append(stream, halt());
}

void build_in_out_test(Seq_T stream)
{
        append(stream, in(r1));
        append(stream, out(r1));
        append(stream, halt());
}

void build_verbose_halt_test(Seq_T stream)
{
        append(stream, halt());
        append(stream, loadval(r1, 'B'));
        append(stream, out(r1));
        append(stream, loadval(r1, 'a'));
        append(stream, out(r1));
        append(stream, loadval(r1, 'd'));
        append(stream, out(r1));
        append(stream, loadval(r1, '!'));
        append(stream, out(r1));
        append(stream, loadval(r1, '\n'));
        append(stream, out(r1));
}

void build_add_test(Seq_T stream)
{
        append(stream, add(r1, r2, r3));
        append(stream, halt());
}

void build_add_test_output(Seq_T stream)
{
        append(stream, loadval(r2, '0' + 3));
        append(stream, loadval(r3, 4));
        append(stream, add(r1, r2, r3));
        append(stream, out(r1));
        append(stream, halt());
}

void build_mult_test_output(Seq_T stream)
{
        append(stream, loadval(r2, 3));
        append(stream, loadval(r3, 3));
        append(stream, loadval(r7, '0'));
        append(stream, mul(r1, r2, r3));
        append(stream, add(r1, r1, r7));
        append(stream, out(r1));
        append(stream, halt());
}

void build_div_test_output(Seq_T stream)
{
        append(stream, loadval(r2, 6));
        append(stream, loadval(r3, 2));
        append(stream, loadval(r7, '0'));
        append(stream, div(r1, r2, r3));
        append(stream, add(r1, r1, r7));
        append(stream, out(r1));
        append(stream, halt());
}

void build_nand_test_output(Seq_T stream)
{
        /* 1010 A */
        /* 1100 C */
        /* ----   */
        /* 0111 7 */
        append(stream, loadval(r2, 0xfffffA));
        append(stream, loadval(r3, 0xfffffC));
        append(stream, nand(r1, r2, r3));
        append(stream, loadval(r7, '0'));
        append(stream, add(r1, r1, r7));
        append(stream, out(r1));
        append(stream, halt());
}

void build_cmove_test_no(Seq_T stream)
{
        append(stream, loadval(r1, 1));
        append(stream, loadval(r2, 2));
        append(stream, loadval(r3, 0));
        /*                    a   b   c  */
        append(stream, cmove(r1, r2, r3)); /* if r[c] != 0, r[a] = r[b] */
        /* r[c] is 0, so r[a] should still be r[a] */
        append(stream, loadval(r7, '0'));
        append(stream, add(r1, r1, r7));
        append(stream, out(r1));
        append(stream, halt());
}

void build_cmove_test_yes(Seq_T stream)
{
        append(stream, loadval(r1, 1));
        append(stream, loadval(r2, 2));
        append(stream, loadval(r3, 999));
        /*                    a   b   c  */
        append(stream, cmove(r1, r2, r3)); /* if r[c] != 0, r[a] = r[b] */
        /* r[c] isn't 0, so r[a] should be r[b] */
        append(stream, loadval(r7, '0'));
        append(stream, add(r1, r1, r7));
        append(stream, out(r1));
        append(stream, halt());
}

void build_sstore_test(Seq_T stream)
{
        /* Value to get output by output instruction*/
        append(stream, loadval(r0, '0' + 4));
        
        /* Make address to place our instruction */
        append(stream, loadval(r2, 0)); /* seg 0*/
        append(stream, loadval(r3, 7)); /* the first halt is 7 */

        /* get output(r1) in r7 */
        append(stream, loadval(r6, 0x100));
        append(stream, loadval(r7, 0xa00000));
        append(stream, mul(r7, r7, r6));

        /* Put output as the next instruction */
        append(stream, sstore(r2, r3, r7));
        append(stream, halt()); /* dummy instruction to replace */
        append(stream, halt()); 
}

void build_ssload_test(Seq_T stream)
{
        /* Value to get read and output by output instruction */
        append(stream, loadval(r0, '0' + 4));
        
        /* Put our value into memory */
        append(stream, loadval(r2, 0)); /* seg 0*/
        append(stream, loadval(r3, 7)); /* the first halt is 7 */
        append(stream, sstore(r2, r3, r0));

        /* Read value from memory */
        append(stream, sload(r7, r2, r3));
        
        /* Output value */
        append(stream, out(r7)); /* should output 4 */

        append(stream, halt());
        append(stream, halt()); /* instruction to get overridden */
}

void build_map_test(Seq_T stream)
{
        /* Map a new segment */
        append(stream, loadval(r0, 1));
        append(stream, map(r1, r0)); /* r1 stores segid */

        /* Load a value */
        append(stream, loadval(r2, 0)); /* r2 holds word index */
        append(stream, loadval(r3, 9)); /* r3 holds value */
        append(stream, sstore(r1, r2, r3));

        /* Read a value */
        append(stream, sload(r7, r1, r2)); /* put in r7 */

        /* Output value */
        append(stream, loadval(r6, '0'));
        append(stream, add(r7, r7, r6));
        append(stream, out(r7)); /* should output 9 */

        append(stream, halt());
}

void build_unmap_test(Seq_T stream)
{
        /* Map a new segment */
        append(stream, loadval(r0, 1));
        append(stream, map(r1, r0)); /* r1 stores segid */

        /* Load a value */
        append(stream, loadval(r2, 0)); /* r2 holds word index */
        append(stream, loadval(r3, 9)); /* r3 holds value */
        append(stream, sstore(r1, r2, r3));

        /* Unmap the segment */
        append(stream, unmap(r1));

        append(stream, halt());
}


void build_load_program_mapped(Seq_T stream)
{
        /* Map a new segment */
        append(stream, loadval(r0, 1));
        append(stream, map(r1, r0)); /* r1 stores segid */

        /* Make a halt instruction 0x70000000 */
        append(stream, loadval(r2, 0x700000)); /* r2 holds halt */
        append(stream, loadval(r3, 0x100));
        append(stream, mul(r2, r2, r3));
        
        /* Load the instructions */
        append(stream, loadval(r4, 0)); /* r2 holds word index */
        append(stream, sstore(r1, r4, r2));

        /* Load program */
        append(stream, loadp(r1, r4)); /* put in r7 */

        /* --- should never get here --- */

        /* Output value */
        append(stream, loadval(r7, 2));
        append(stream, out(r7)); /* should output 2 */

        append(stream, halt());
}

void build_load_program0(Seq_T stream)
{       
        /* Load program */
        append(stream, loadval(r0, 0));
        append(stream, loadval(r1, 5));
        append(stream, loadp(r0, r1)); /* put in r7 */

        /* --- should never get here --- */

        /* Output value */
        append(stream, loadval(r7, 2));
        append(stream, out(r7)); /* should output 2 */

        /* Branch to here */
        append(stream, halt());

        append(stream, halt());
}



void build_print_six_test(Seq_T stream)
{
        append(stream, loadval(r1, 48)); /* '0' */
        append(stream, loadval(r2, 6)); /* '6' */
        append(stream, add(r3, r1, r2));
        append(stream, out(r3));
        append(stream, halt());
}

void do_something_1_million_times(Seq_T stream)
{
        /* Do something 100,000 times 10 times */
        for (int outer = 0; outer < 10; outer++) {
                /* Do some instructions 99,998 times, then output a value */
                for (int i = 0; i < 99998; i++) {
                        append(stream, loadval(r1, (i & 0xf))); 
                }
                append(stream, loadval(r2, outer  + '0')); 
                append(stream, out(r2));
        }
        append(stream, halt());
}       