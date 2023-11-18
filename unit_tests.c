/*
 * unit_tests.c
 */

/* C Std Libs */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/* Hanson Libs */
#include <except.h>
#include <assert.h>

/* Our Libs */
#include "segmem.h"

/* Function Declarations */
void check_constructor_destructor();
void check_fetch_next_i_basic();
void check_get_word_seg_0();
void check_put_word_seg_0();
void check_put_get_word_seg_0();
void put_oob_segid();
void put_oob_wordid();
void get_oob_segid();
void get_oob_wordid();
void map_unmap_at_limit(); 
void map_map_segs(); 
void check_new_segment_all_0s(); 
void get_put_word_new_segments(); 
void check_load_seg_0(); 
void check_load_seg_other(); 

int main()
{
        /* Constructor/destructor */
        check_constructor_destructor(); 
        
        /* Fetch next instruction */
        check_fetch_next_i_basic(); 

        /* Getting/putting word */
        check_get_word_seg_0();
        check_put_word_seg_0(); 
        check_put_get_word_seg_0(); 
        put_oob_segid();
        put_oob_wordid();
        get_oob_segid();
        get_oob_wordid();
        
        /* mapping tests */
        map_map_segs(); 
        map_unmap_at_limit(10);
        map_unmap_at_limit(0xffff);
        // map_unmap_at_limit(0xfffffff);

        check_new_segment_all_0s(); 
        get_put_word_new_segments(); 
        
        check_load_seg_0(); 
        check_load_seg_other(); 
}

/* Ensure memory can be properly allacated and deallocated */
void check_constructor_destructor() {
        /* Open a hardcoded test file */
        FILE *input = fopen("add.um", "r"); 
        assert(input != NULL);

        /* Pass it to memory as the initial program */
        SegMem_T mem = SegMem_new(input);

        /* Close the file */
        fclose(input);

        /* Free the memory with the destructor */
        SegMem_free(&mem);

        /* Check that destructor set the variable to NULL */
        assert(mem == NULL);
}

/* Reads a simple example file with the constructor. Compare instructions 
 * fetched with those in the hardcoded file. Uses SegMem_new() and SegMem_free()
 */ 
void check_fetch_next_i_basic()
{
        /* Open the file passed in and check it */
        FILE *input = fopen("add.um", "r"); 
        assert(input != NULL);

        /* Pass it to memory as the initial program */
        SegMem_T mem = SegMem_new(input);

        /* Close the file */
        fclose(input); 

        /* Check the instructions in the file are what we expect them to be */
        assert(SegMem_fetch_next_i(mem) == 0x30000053); 
        assert(SegMem_fetch_next_i(mem) == 0x70000000);

        /* Free the memory with the destructor */
        SegMem_free(&mem);

        /* Check that destructor set the variable to NULL */
        assert(mem == NULL);
}


/* Reads a simple example file with the constructor. 
 * Compares words read at various indices with those in the file. 
 * Uses SegMem_new(), _get_word(), and _free() */
void check_get_word_seg_0() 
{
        /* Open the file passed in and check it */
        FILE *input = fopen("add.um", "r"); 
        assert(input != NULL);

        /* Pass it to memory as the initial program */
        SegMem_T mem = SegMem_new(input);

        /* Close the file */
        fclose(input); 

        /* Check the instructions in the file are what we expect them to be */
        /* Check the instructions in the file are what we expect them to be */
        assert(SegMem_get_word(mem, 0, 0) == 0x30000053); 
        assert(SegMem_get_word(mem, 0, 1) == 0x70000000);

        /* Free the memory with the destructor */
        SegMem_free(&mem);

        /* Check that destructor set the variable to NULL */
        assert(mem == NULL);
}

/* Read a simple example file with the constructor. Replace every other word 
 * with various values. Compare values read out by fetch_next_i */
void check_put_word_seg_0() 
{
       /* Open the file passed in and check it */
        FILE *input = fopen("add.um", "r"); 
        assert(input != NULL);

        /* Pass it to memory as the initial program */
        SegMem_T mem = SegMem_new(input);

        /* Close the file */
        fclose(input);

        /* Put some instructions in - changing what seg0 is */
        uint32_t val1 = 0x89abcdef;
        uint32_t val2 = 0x01234567;
        SegMem_put_word(mem, 0, 0, val1);
        SegMem_put_word(mem, 0, 1, val2);

        /* Check the instructions in the file are what we expect them to be */
        assert(SegMem_fetch_next_i(mem) == val1); 
        assert(SegMem_fetch_next_i(mem) == val2);

        /* Free the memory with the destructor */
        SegMem_free(&mem);

        /* Check that destructor set the variable to NULL */
        assert(mem == NULL); 
}

/* Reads a simple example file with the constructor. _put_word()s words in every
 * other value, then checks that those values are what they should be 
 * with _get_word() */
void check_put_get_word_seg_0() 
{
        /* Open the file passed in and check it */
        FILE *input = fopen("add.um", "r"); 
        assert(input != NULL);

        /* Pass it to memory as the initial program */
        SegMem_T mem = SegMem_new(input);

        /* Close the file */
        fclose(input);

        /* Put some instructions in - changing what seg0 is */
        uint32_t val1 = 0x89abcdef;
        uint32_t val2 = 0x01234567;
        SegMem_put_word(mem, 0, 0, val1);
        SegMem_put_word(mem, 0, 1, val2);

        /* Check the instructions in the file are what we expect them to be */
        assert(SegMem_get_word(mem, 0, 0) == val1); 
        assert(SegMem_get_word(mem, 0, 1) == val2);

        /* Free the memory with the destructor */
        SegMem_free(&mem);

        /* Check that destructor set the variable to NULL */
        assert(mem == NULL);
}



/* Check that putting a word out of bounds CREs */
void put_oob_segid() 
{
        /* Open the file passed in and check it */
        FILE *input = fopen("add.um", "r"); 
        assert(input != NULL);

        /* Pass it to memory as the initial program */
        SegMem_T mem = SegMem_new(input);

        /* Close the file */
        fclose(input);

        /* Try to put something out of bounds */
        bool failed = false;
        TRY {
                SegMem_put_word(mem, 100, 0, 0x89abcdef);
        } ELSE {
                failed = true;
        } END_TRY;

        assert(failed); 

        /* Free the memory with the destructor */
        SegMem_free(&mem);

        /* Check that destructor set the variable to NULL */
        assert(mem == NULL);
}

/* Check that putting a word out of bounds CREs */
void put_oob_wordid() 
{
        /* Open the file passed in and check it */
        FILE *input = fopen("add.um", "r"); 
        assert(input != NULL);

        /* Pass it to memory as the initial program */
        SegMem_T mem = SegMem_new(input);

        /* Close the file */
        fclose(input);

        /* Try to put something out of bounds */
        bool failed = false;
        TRY {
                SegMem_put_word(mem, 0, 100, 0x1);
        } ELSE {
                failed = true;
        } END_TRY;

        assert(failed);

        /* Free the memory with the destructor */
        SegMem_free(&mem);

        /* Check that destructor set the variable to NULL */
        assert(mem == NULL);
}

/* Check that getting a word out of bounds CREs */
void get_oob_segid() 
{
        /* Open the file passed in and check it */
        FILE *input = fopen("add.um", "r"); 
        assert(input != NULL);

        /* Pass it to memory as the initial program */
        SegMem_T mem = SegMem_new(input);

        /* Close the file */
        fclose(input);

        /* Try to put something out of bounds */
        bool failed = false;
        TRY {
                SegMem_put_word(mem, 100, 0, 0x0);
        } ELSE {
                failed = true;
        } END_TRY;

        assert(failed); 

        /* Free the memory with the destructor */
        SegMem_free(&mem);

        /* Check that destructor set the variable to NULL */
        assert(mem == NULL);
}

/* Check that getting a word out of bounds CREs */
void get_oob_wordid() 
{
        /* Open the file passed in and check it */
        FILE *input = fopen("add.um", "r"); 
        assert(input != NULL);

        /* Pass it to memory as the initial program */
        SegMem_T mem = SegMem_new(input);

        /* Close the file */
        fclose(input);

        /* Try to put something out of bounds */
        bool failed = false;
        TRY {
                SegMem_put_word(mem, 0, 100, 0x023);
        } ELSE {
                failed = true;
        } END_TRY;

        assert(failed);

        /* Free the memory with the destructor */
        SegMem_free(&mem);

        /* Check that destructor set the variable to NULL */
        assert(mem == NULL);
}

void map_map_segs()
{
        /* Open the file passed in and check it */
        FILE *input = fopen("add.um", "r"); 
        assert(input != NULL);

        /* Pass it to memory as the initial program */
        SegMem_T mem = SegMem_new(input);

        /* Close the file */
        fclose(input);
        
        SegMem_map(mem, 0);
        SegMem_map(mem, 1); 

        /* Free the memory with the destructor */
        SegMem_free(&mem);

        /* Check that destructor set the variable to NULL */
        assert(mem == NULL);
}


/* Test by mapping and unmapping 2^32 + 1 times to make sure 
 * we can re-use identifiers */
void map_unmap_at_limit(uint32_t num_times)
{
        /* Open the file passed in and check it */
        FILE *input = fopen("add.um", "r"); 
        assert(input != NULL);

        /* Pass it to memory as the initial program */
        SegMem_T mem = SegMem_new(input);

        /* Close the file */
        fclose(input);

        /* Map then immediately unmap 2^32 times to make sure we reuse */
        uint32_t segid = 0;
        uint32_t size = 0;
        for (uint32_t i = 0; i < num_times - 1; i++) {
                size = i % 100 + 100;
                segid = SegMem_map(mem, size);
                assert(segid = 1);
                SegMem_unmap(mem, segid);
        }

        /* Free the memory with the destructor */
        SegMem_free(&mem);

        /* Check that destructor set the variable to NULL */
        assert(mem == NULL);
}

/* Make sure a new segment is initialized to be all 0s */
void check_new_segment_all_0s()
{
        /* Open the file passed in and check it */
        FILE *input = fopen("add.um", "r"); 
        assert(input != NULL);

        /* Pass it to memory as the initial program */
        SegMem_T mem = SegMem_new(input);

        /* Close the file */
        fclose(input);

        /* Map a new segment with some words. They should all be 0 */
        uint32_t size = 33;
        uint32_t segid = SegMem_map(mem, size);

        for (uint32_t i = 0; i < size; i++) {
                assert(SegMem_get_word(mem, segid, i) == 0);
        }
        
        /* Unmap the segment */
        SegMem_unmap(mem, segid);

        /* Free the memory with the destructor */
        SegMem_free(&mem);

        /* Check that destructor set the variable to NULL */
        assert(mem == NULL);
}

/* Map several new segments then put and get words from them to ensure 
 * they are as they should be */
void get_put_word_new_segments()
{
        /* Open the file passed in and check it */
        FILE *input = fopen("add.um", "r"); 
        assert(input != NULL);

        /* Pass it to memory as the initial program */
        SegMem_T mem = SegMem_new(input);

        /* Close the file */
        fclose(input);

        /* Map a new segment with some words. They should all be 0 */
        uint32_t size = 33;
        uint32_t segid = SegMem_map(mem, size);

        SegMem_put_word(mem, 0, 0, 0xabc);
        SegMem_put_word(mem, 0, 1, 0x123);
        SegMem_put_word(mem, segid, 0, 0xff);
        SegMem_put_word(mem, segid, 32, 999);

        assert(SegMem_get_word(mem, 0, 0) == 0xabc);
        assert(SegMem_get_word(mem, 0, 1) == 0x123);
        assert(SegMem_get_word(mem, segid, 0) == 0xff);
        assert(SegMem_get_word(mem, segid, 32) == 999);


        /* Try to put something out of bounds */
        bool failed = false;
        TRY {
                SegMem_put_word(mem, segid, size + 1, 0x023);
        } ELSE {
                failed = true;
        } END_TRY;
        assert(failed);

        /* Try to put something out of bounds */
        failed = false;
        TRY {
                SegMem_get_word(mem, segid, size + 1);
        } ELSE {
                failed = true;
        } END_TRY;
        assert(failed);
        
        /* Unmap the segment */
        SegMem_unmap(mem, segid);

        /* Free the memory with the destructor */
        SegMem_free(&mem);

        /* Check that destructor set the variable to NULL */
        assert(mem == NULL);
}

/* 
 * Test on segment 0 by loading a program, fetching its instructions 
 * and printing them out, then loading segment 0 at program counter 0, 
 * and fetching the same instructions and printing them out to make sure 
 * they match. 
 *
 * Uses _load_program() and _fetch_next_i() as well as the 
 * constructor to make sure that loading segment 0 works to change
 * the program counter and doesn’t change data
 */
void check_load_seg_0()
{
        /* Open the file passed in and check it */
        FILE *input = fopen("add.um", "r"); 
        assert(input != NULL);

        /* Pass it to memory as the initial program */
        SegMem_T mem = SegMem_new(input);

        /* Close the file */
        fclose(input); 

        /* Check the instructions in the file are what we expect them to be */
        assert(SegMem_fetch_next_i(mem) == 0x30000053); 
        assert(SegMem_fetch_next_i(mem) == 0x70000000);

        SegMem_load_program(mem, 0, 0); 

        assert(SegMem_fetch_next_i(mem) == 0x30000053); 
        assert(SegMem_fetch_next_i(mem) == 0x70000000);

        /* Free the memory with the destructor */
        SegMem_free(&mem);

        /* Check that destructor set the variable to NULL */
        assert(mem == NULL);
}


/* 
 * Test on a different segment by printing out instructions fetched and 
 * program counter, then instructions fetched and program counter after loading
 * a program in a different segment created by mapping and then 
 * put_wording values into it 
 * Uses _put() and _load_program() and _fetch_next_i() as well as the 
 * constructor to make sure that loading segment other than the zero 
 * segment works to change the program counter and doesn’t change data
 */
void check_load_seg_other()
{
         /* Open the file passed in and check it */
        FILE *input = fopen("add.um", "r"); 
        assert(input != NULL);

        /* Pass it to memory as the initial program */
        SegMem_T mem = SegMem_new(input);

        /* Close the file */
        fclose(input); 

        /* Check the instructions in the file are what we expect them to be */
        assert(SegMem_fetch_next_i(mem) == 0x30000053); 
        assert(SegMem_fetch_next_i(mem) == 0x70000000);

        uint32_t size = 5;
        uint32_t segid = SegMem_map(mem, size);

        uint32_t i1, i2, i3, i4, i5;
        i1 = 0xd2000030;
        i2 = 0xd4000006;
        i3 = 0x300000ca;
        i4 = 0xa0000003;
        i5 = 0x70000000;

        SegMem_put_word(mem, segid, 0, i1);
        SegMem_put_word(mem, segid, 1, i2);
        SegMem_put_word(mem, segid, 2, i3);
        SegMem_put_word(mem, segid, 3, i4);
        SegMem_put_word(mem, segid, 4, i5);

        SegMem_load_program(mem, segid, 0); 

        assert(SegMem_fetch_next_i(mem) == i1); 
        assert(SegMem_fetch_next_i(mem) == i2);
        assert(SegMem_fetch_next_i(mem) == i3);
        assert(SegMem_fetch_next_i(mem) == i4);
        assert(SegMem_fetch_next_i(mem) == i5);

        /* Free the memory with the destructor */
        SegMem_free(&mem);

        /* Check that destructor set the variable to NULL */
        assert(mem == NULL);
}

