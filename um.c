/* Um.c 
 * Authors: Tom Lyons     tlyons01
 *          Noah Stiegler nsteig01
 * CS40 HW6 UM
 * Tufts University
 * 11/20/2023
 * 
 * 
 */



/* Standard Libs */
#include <stdlib.h>
#include <stdio.h>

/* Hanson Libs */
#include <assert.h>

/* Our Modules */
#include "segmem.h"

/* Private helper functions */
void Um_run(FILE *input); 
static void print_usage();

// static const int NUM_REGISTERS = 8;

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
        
        return 0; 
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
 *      Run the UM emulator on the code stored in the provided filename
 * Arguments:
 *      (char *) filename - The name of the file to read the program from
 * Notes:
 *      - CRE for filename to be NULL
 *      - CRE if filename can't be opened
 */
void Um_run(FILE *input)
{
        SegMem_T memory = SegMem_new(input);


        SegMem_free(&memory); 
}

