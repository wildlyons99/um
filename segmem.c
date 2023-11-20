/* segmem.c
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

/* Header */
#include "segmem.h"

/* C Std Libs */
#include <stdint.h> 

/* Hanson Libs */
#include <seq.h>
#include <mem.h>
#include <assert.h>

/* Course Libs */
#include <bitpack.h>

/* 32 bit words */
static const int BYTES_IN_WORD = sizeof(word_t) / sizeof(char); 
static const int BITS_IN_BYTE = sizeof(char) * 8;

/* Guess of how many words a program is when loaded for the first time */
static const unsigned PROGRAM_SIZE_GUESS = 65536;
/* Guess of how many segments a program will use */
static const unsigned SEGMENTS_TO_USE_GUESS = 1024;

/* helper function defintions */
static word_t read_word(FILE *input);

/* Defines the implementation of a SegMen_T instance */
struct SegMem_T {
        /* Sequence of segments (represented as sequences) */
        Seq_T data_segments;
        
        /* Sequence of segment IDs. Seq represents a stack
         * INVARIANT: Holds unmapped segment IDs (which had been previously 
         * mapped)
         * Note: Use high as the top of the stack */
        Seq_T unmapped_stack;
        
        /* Index in seg0 of next instruction to run (instruction pointer) */
        word_t ip;
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
 *      - CRE for the file to end in the middle of a 32-bit word
 *      - Reads file till the end but does not close it
 */
SegMem_T SegMem_new(FILE *input)
{
        assert(input != NULL);
        
        Seq_T seg0 = Seq_new(PROGRAM_SIZE_GUESS);
        
        /* Read in file one 32-bit word at a time */
        int c;
        while (!feof(input)) {
                /* Check to make sure we aren't at the end of a file */
                c = fgetc(input);
                if (c != EOF) {
                        ungetc(c, input);
                        Seq_addhi(seg0, (void *)(uintptr_t)read_word(input)); 
                }
        }

        /* Put struct together */
        SegMem_T new_mem = NEW(new_mem);
        Seq_T data_segments = Seq_new(SEGMENTS_TO_USE_GUESS);
        Seq_addhi(data_segments, seg0);
        new_mem->data_segments = data_segments;
        new_mem->unmapped_stack = Seq_new(SEGMENTS_TO_USE_GUESS);
        new_mem->ip = 0;
        
        return new_mem;
}



/* read_word
 * Purpose:
 *      Read a single 32-bit word from an input in Big-Endian order
 * Arguments:
 *      (FILE *) input
 * Returns: 
 *      (word_t) the bitpacked word in big endian from input
 * Notes:
 *      - CRE for input to be NULL
 *      - CRE for file to end in the middle of a 32-bit word
 */
static word_t read_word(FILE *input) 
{
        assert(input != NULL);
        
        /* bitpack 32 bit word with data from file in big endian order */
        word_t word = 0;
        int c;
        for (int i = BYTES_IN_WORD - 1; i >= 0; i--) {
                c = fgetc(input);
                assert(c != EOF);

                word = Bitpack_newu(word, 8, i * BITS_IN_BYTE, c); 
        }

        return word;
}

/* SegMem_fetch_next_i
 * Purpose: 
 *      Fetches the next um instruction from the program loaded in segment 0
 * Arguments:
 *      (SegMem_T) mem - The memory to get the instruction from
 * Returns:
 *      (word_t) that contains the next instruction for the um to execute
 * Notes:
 *      - CRE for mem to be NULL 
 *      - CRE for mem->data_segments to be NULL
 *      - URE for the next instruction to be asked for when there are no more
 *        instructions
 */
word_t SegMem_fetch_next_i(SegMem_T mem)
{
        assert(mem != NULL);
        assert(mem->data_segments != NULL);

        Seq_T seg0 = Seq_get(mem->data_segments, 0);
        
        return (uintptr_t)Seq_get(seg0, mem->ip++);
}

/* SegMem_map
 * Purpose:
 *      Maps a new segment in the memory of a given size and gives back its id
 * Arguments:
 *      (SegMem_T) mem - The memory to map a new segment in
 *      (word_t) size - the number of words the segment should store
 * Returns:
 *      (word_t) that contains the segment identifier for the newly mapped
 *                 segment
 * Notes:
 *      - CRE for mem to be NULL or mem->data_segments, or mem_unmapped_stack
 *        to be NULL
 *      - CRE for a segment to be mapped if 2^32 segments are already mapped
 *      - CRE if there isn't enough memory to map a segment of the given size
 *      - The Hanson sequence implementation can only store a max of 2^31 
 *        segements at once
 *              - Regardless, storing 2^31 segments (if empty, they're still
 *                void pointers which take 8 bytes of memory) would use ~17 GB 
 *                of memory, which is outside the mem scope (of our program)
 *              - And storing 2^32 32-bit words (hypothetically the max in
 *                one segment) would take the same amount of memory and would
 *                not be supported by the Hanson sequence 
 */
word_t SegMem_map(SegMem_T mem, word_t size)
{
        assert(mem != NULL); 
        assert(mem->data_segments != NULL);
        assert(mem->unmapped_stack != NULL); 

        /* make a new sequence of the correct length filled with 0s */
        Seq_T new_seg = Seq_new(size);
        for (uint32_t i = 0; i < size; i++) {
                Seq_addhi(new_seg, (void *)(uintptr_t)0x0);
        }
        
        /* Figure out what segment id this segment should be */
        word_t segment_id;
        if (Seq_length(mem->unmapped_stack) > 0) {
                /* We have an unmapped space in data_segments */
                segment_id = (uintptr_t)Seq_remhi(mem->unmapped_stack);
                Seq_put(mem->data_segments, segment_id, new_seg);
        } else {
                /* Need to expand data_segments */
                Seq_addhi(mem->data_segments, new_seg);
                segment_id = Seq_length(mem->data_segments) - 1;
        }

        return segment_id;
}

/* SegMem_unmap
 * Purpose:
 *      Unmap a segment of memory allowing for its segment ID to be reused
 * Arguments: 
 *      (SegMem_T) mem - The memory to unmap the segment in
 *      (word_t) seg_id - Which segment to unmap
 * Notes:
 *      - CRE for mem, mem->data_segments, or mem->unmapped_stack to be NULL
 *      - URE if unmapping a segment that is not mapped.
 */
void SegMem_unmap(SegMem_T mem, word_t seg_id)
{
        assert(mem != NULL);
        assert(mem->data_segments != NULL);
        assert(mem->unmapped_stack != NULL);

        /* Get segment to unmap */
        Seq_T segment = Seq_get(mem->data_segments, seg_id);
        assert(segment != NULL);

        /* Free it and put NULL in its place */
        Seq_free(&segment);
        Seq_put(mem->data_segments, seg_id, NULL); 
        
        /* Save its ID in our stack to reuse */
        Seq_addhi(mem->unmapped_stack, (void *)(uintptr_t)seg_id);
}


/* SegMem_get_word
 * Purpose:
 *      Retrieve a word stored in memory at the given segment and index
 * Arguments:
 *      (SegMem_T) mem - The memory to get the word from
 *      (word_t) seg_id - The segment to get the word from
 *      (word_t) word_idx - Which word in the segment to get
 * Returns:
 *      (word_t) holding the value stored at that location in memory
 * Notes:
 *      - CRE for mem to be NULL
 *      - CRE for mem->data_segments to be NULL
 *      - CRE for seg_id to refer to a segment which doesn't exist
 *      - URE for word_idx to refer to a word outside the bounds of the segment
 *        but the Hanson Sequence might CRE it
 */
word_t SegMem_get_word(SegMem_T mem, word_t seg_id, word_t word_idx)
{
        assert(mem != NULL);
        assert(mem->data_segments != NULL);

        Seq_T segment = Seq_get(mem->data_segments, seg_id); 
        assert(segment != NULL);

        return (uintptr_t)Seq_get(segment, word_idx);
}


/* SegMem_put_word
 * Purpose:
 *      Puts a word into memory at the given segment and index
 * Arguments:
 *      (SegMem_T) mem - The memory to put the word into
 *      (word_t) seg_id - The segment to put the word into
 *      (word_t) word_idx - Which word in the segment to set
 *      (word_t) word - the word to be put into memory 
 * Notes:
 *      - CRE for mem to be NULL
 *      - CRE for mem->data_segments to be NULL
 *      - CRE for seg_id to refer to a segment which is unmapped
 *      - URE for word_idx to refer to a word outside of a segment (although
 *        the Hanson sequence might CRE it)
 */
void SegMem_put_word(SegMem_T mem, word_t seg_id, word_t word_idx, 
                         word_t word)
{
        assert(mem != NULL);
        assert(mem->data_segments != NULL);

        Seq_T segment = Seq_get(mem->data_segments, seg_id); 
        assert(segment != NULL);

        Seq_put(segment, word_idx, (void *)(uintptr_t)word);
}

/* SegMem_load_program
 * Purpose: 
 *      Loads a new program stored in the given segment to run by copying the
 *      contents of that segment into segment 0 and setting the program counter
 *      to the provided value. Quick to load segment 0
 * Arguments:
 *      (SegMem_T) mem - The memory to load the program from/into
 *      (word_t) seg_id - The segment in mem to load the program from
 *      (word_t) new_program_counter - The word index in the new program to 
 *                                       start reading instructions from
 * Notes:
 *      - CRE for mem, mem->data_segments, or mem->unmapped_stack to be NULL
 *      - CRE for seg_id to not 
 *      - URE for seg_id to not refer to a mapped segment
 *      - URE for new_program_counter to refer to an instruction out of bounds
 *        of the new program
 *      - Incredibly quick to load segment 0
 * */
void SegMem_load_program(SegMem_T mem, word_t seg_id,
                         word_t new_program_counter)
{
        assert(mem != NULL);
        assert(mem->data_segments != NULL);
        assert(mem->unmapped_stack != NULL);

        /* Update instruction pointer */
        mem->ip = new_program_counter;

        /* Fast case - don't need to copy anything! */
        if (seg_id == 0) { 
                return; 
        } 

        /* Free the old seg0 */
        Seq_T old_seg0 = Seq_get(mem->data_segments, 0);
        Seq_free(&old_seg0); 
        
        /* Make a copy of the segment to load */
        assert(seg_id < (uint32_t)Seq_length(mem->data_segments));
        Seq_T to_copy = Seq_get(mem->data_segments, seg_id); 
        assert(to_copy != NULL);       
        int length = Seq_length(to_copy);
        Seq_T new_seg_0 = Seq_new(length);
        for (int i = 0; i < length; i++) {
                Seq_addhi(new_seg_0, Seq_get(to_copy, i)); 
        }

        /* Put the new segment in segment 0 */
        Seq_put(mem->data_segments, 0, new_seg_0);
}

/* SegMem_free
 * Purpose: 
 *      To free the memory used to store SegMem information
 * Argument: 
 *      (SegMem_T) mem - The memory to free
 * Notes:
 *      - CRE if mem is null
 *      - CRE mem->data_segments is NULL
 *      - CRE if mem->unmapped_stack is NULL
 */
void SegMem_free(SegMem_T *mem)
{
        assert(mem != NULL);
        assert(*mem != NULL);

        SegMem_T memory = *mem;
        assert(memory->data_segments != NULL);
        assert(memory->unmapped_stack != NULL);

        /* Free all segments in our data segments */
        uint32_t length = Seq_length(memory->data_segments);
        for (uint32_t i = 0; i < length; i++) {
                Seq_T to_delete = Seq_get(memory->data_segments, i);
                if (to_delete != NULL) {
                        Seq_free(&to_delete);
                }
        }

        /* Free the collection of segments */
        Seq_free(&(memory->data_segments));
        
        /* Free the stack holding unmapped memory addresses */
        Seq_free(&(memory->unmapped_stack));

        /* Free struct */
        FREE(memory);

        /* Set user's pointer to NULL */
        *mem = NULL;
}
