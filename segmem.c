/* segmem 
 * Authors: Tom Lyons     tlyons01
 *          Noah Stiegler nstieg01
 * 
 * 
 */

/* Header */
#include "segmem.h"

/* C Std Libs */
#include <stdio.h>
#include <stdint.h> 

/* Hanson Libs */
#include <seq.h>
#include <table.h>

/* Complete the incomplete struct definition */
struct SegMem_T {
        Seq_T seg0;
        Table_T data_segments;
        uint32_t instruction_pointer;
}; 

/* SegMem_new
 * Purpose: 
 *      Creates a new segmented memory instance with the contents of the file
 *      passed in as the contents of Segment 0. Sets the instruction pointer
 *      to be word 0.
 * Arguments: 
 *      (FILE *) input_file - An opened filestream to a file containing a .um
 *                            program
 * Returns: 
 *      (SegMem_T) that contains the entirity of the new memory instance for 
 *                 the universal machine 
 * Notes: 
 *      - CRE for input_file to be NULL
 */
SegMem_T SegMem_new(FILE *input_file); 


/* SegMem_fetch_next_i
 * Purpose: 
 *      Fetches the next um instruction from the program loaded in segment 0
 * Arguments:
 *      (SegMem_T) mem - The memory to get the instruction from
 * Returns:
 *      (uint32_t) that contains the next instruction for the um to execute
 * Notes:
 *      - CRE for mem to be NULL 
 *      - URE for the next instruction to be asked for when there are no more
 *        instructions
 */
uint32_t SegMem_fetch_next_i(SegMem_T mem);

/* SegMem_map
 * Purpose:
 *      Maps a new segment in the memory of a given size and gives back its id
 * Arguments:
 *      (SegMem_T) mem - The memory to map a new segment in
 *      (uint32_t) size - the number of words the segment can store
 * Returns:
 *      (uint32_t) that contains the segment identifier for the newly mapped
 *                 segment
 * Notes:
 *      - CRE for mem to be NULL
 *      - CRE for a segment to be mapped if 2^32 segments are already mapped
 *      - CRE if there isn't enough memory to map a segment of the given size
 */
uint32_t SegMem_map(SegMem_T mem, uint32_t size); 

/* SegMem_unmap
 * Purpose:
 *      Unmap a segment of memory allowing for its segment ID to be reused
 * Arguments: 
 *      (SegMem_T) mem - The memory to unmap the segment in
 * Notes:
 *      - CRE for mem to be NULL
 *      - URE if unmapping a segment that is not mapped.
 */
void SegMem_unmap(SegMem_T mem); 


/* SegMem_get_word
 * Purpose:
 *      Retrieve a word stored in memory at the given segment and index
 * Arguments:
 *      (SegMem_T) mem - The memory to get the word from
 *      (uint32_t) seg_id - The segment to get the word from
 *      (uint32_t) word_idx - Which word in the segment to get
 * Notes:
 *      - CRE for mem to be NULL
 *      - URE for seg_id to refer to a segment which doesn't exist
 *      - URE for word_idx to refer to a word outside the bounds of the segment
 */
uint32_t SegMem_get_word(SegMem_T mem, uint32_t seg_id, uint32_t word_idx); 


/* SegMem_put_word
 * Purpose:
 *      Puts a word into memory at the given segment and index
 * Arguments:
 *      (SegMem_T) mem - The memory to put the word into
 *      (uint32_t) seg_id - The segment to put the word into
 *      (uint32_t) word_idx - Which word in the segment to set
 *      (uint32_t) word - the word to be put into memory 
 * Notes:
 *      - CRE for mem to be NULL
 *      - URE for seg_id 
 */
uint32_t SegMem_put_word(SegMem_T mem, uint32_t seg_id, uint32_t word_idx, 
                         uint32_t word); 

/* SegMem_load_program
 * Purpose: 
 *      Loads a new program stored in the given segment to run by copying the
 *      contents of that segment into segment 0 and setting the program counter
 *      to the provided value
 * Arguments:
 *      (SegMem_T) mem - The memory to load the program from/into
 *      (uint32_t) seg_id - The segment in mem to load the program from
 *      (uint32_t) new_program_counter - The word index in the new program to 
 *                                       start reading instructions from
 * Notes:
 *      - CRE for mem to be NULL
 *      - URE for seg_id to not refer to a mapped segment
 *      - URE for new_program_counter to refer to an instruction out of bounds
 *        of the new program
 *      - Incredibly quick to load segment 0
 * */
void SegMem_load_program(SegMem_T mem, uint32_t seg_id,
                         uint32_t new_program_counter);

/* SegMem_free
 * Purpose: 
 *      To free the memory used to store SegMem information
 * Argument: 
 *      (SegMem_T) mem - The memory to free
 * Notes:
 *      - CRE if mem is null
 *      - CRE if mem->seg0 or mem->data_segments is NULL
 */
void SegMem_free(SegMem_T &mem); 


/* ---- private static funcs */

initialize segment id tracker

turn file into segment

make new segment segment 0

