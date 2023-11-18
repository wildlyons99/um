/* segmem.h
 * Authors: Tom Lyons     tlyons01
 *          Noah Stiegler nstieg01
 * Date: November 20, 2023
 * CS40 HW 6 Universal Machine
 * 
 * Implements segmented memory. It keeps track of the instruction pointer. 
 * Stores the currently executing code as well as data in 
 * memory. It fetches the next instruction, fetches words from memory, stores 
 * words in memory, maps new segments and unmaps existing segments.
 */


#ifndef SEGMEM_H
#define SEGMEM_H

#include <stdio.h>
#include <stdint.h>

/* Use 32 bit words */
typedef uint32_t word_t;

typedef struct SegMem_T *SegMem_T;

SegMem_T SegMem_new(FILE *input);

word_t SegMem_fetch_next_i(SegMem_T mem);

void SegMem_put_word(SegMem_T mem, word_t seg_id, word_t word_idx, word_t word);

word_t SegMem_get_word(SegMem_T mem, word_t seg_id, word_t word_idx);

void SegMem_unmap(SegMem_T mem, word_t seg_id);

word_t SegMem_map(SegMem_T mem, word_t size);

void SegMem_free(SegMem_T *mem);

void SegMem_load_program(SegMem_T mem, word_t seg_id,
                         word_t new_program_counter); 


#endif