#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assert.h"
#include "fmt.h"
#include "seq.h"

extern void Um_write_sequence(FILE *output, Seq_T instructions);

extern void build_halt_test(Seq_T stream);
extern void build_halt_output_test(Seq_T stream); 
extern void build_load_output_test(Seq_T stream); 
extern void build_in_out_test(Seq_T stream); 
extern void build_verbose_halt_test(Seq_T stream);
extern void build_add_test(Seq_T stream);
extern void build_add_test_output(Seq_T stream);
extern void build_mult_test_output(Seq_T stream);
extern void build_print_six_test(Seq_T stream);
extern void build_div_test_output(Seq_T stream);
extern void build_cmove_test_no(Seq_T stream);
extern void build_cmove_test_yes(Seq_T stream);
extern void build_nand_test_output(Seq_T stream);
extern void build_sstore_test(Seq_T stream);
extern void build_ssload_test(Seq_T stream);
extern void build_map_test(Seq_T stream);
extern void build_unmap_test(Seq_T stream);
extern void build_load_program_mapped(Seq_T stream);
extern void build_load_program0(Seq_T stream);
extern void do_something_1_million_times(Seq_T stream);


/* The array `tests` contains all unit tests for the lab. */

static struct test_info {
        const char *name;
        const char *test_input;          /* NULL means no input needed */
        const char *expected_output;
        /* writes instructions into sequence */
        void (*build_test)(Seq_T stream);
} tests[] = {
        { "halt",         NULL, "", build_halt_test },
        { "halt_output", NULL, "\0", build_halt_output_test},
        { "load_output", NULL , "3",  build_load_output_test},
        { "in_out", "7", "7", build_in_out_test}, 
        { "add",          NULL, "", build_add_test },
        { "add_w_output",  NULL, "7", build_add_test_output }, 
        { "mult_w_output",  NULL, "9", build_mult_test_output }, 
        { "div_w_output",  NULL, "3", build_div_test_output }, 
        { "cmove_test_no", NULL, "1", build_cmove_test_no},
        { "cmove_test_yes", NULL, "2", build_cmove_test_yes},
        { "halt-verbose", NULL, "", build_verbose_halt_test },
        { "sstore_test", NULL, "4", build_sstore_test},
        { "print-six", NULL, "6", build_print_six_test },
        { "nand_test_output", NULL, "7", build_nand_test_output},
        { "ssload_test", NULL, "4", build_ssload_test}, 
        { "map_test", NULL, "9", build_map_test},
        { "unmap_test", NULL, "", build_unmap_test}, 
        { "load_program_mapped", NULL, "", build_load_program_mapped}, 
        { "load_program0", NULL, "", build_load_program0},
        { "do_something_1_million_times", NULL, "0123456789", do_something_1_million_times}
};

  
#define NTESTS (sizeof(tests)/sizeof(tests[0])) 

/*
 * open file 'path' for writing, then free the pathname;
 * if anything fails, checked runtime error
 */
static FILE *open_and_free_pathname(char *path);

/*
 * if contents is NULL or empty, remove the given 'path', 
 * otherwise write 'contents' into 'path'.  Either way, free 'path'.
 */
static void write_or_remove_file(char *path, const char *contents);

static void write_test_files(struct test_info *test);


int main (int argc, char *argv[])
{
        bool failed = false;
        if (argc == 1)
                for (unsigned i = 0; i < NTESTS; i++) {
                        printf("***** Writing test '%s'.\n", tests[i].name);
                        write_test_files(&tests[i]);
                }
        else
                for (int j = 1; j < argc; j++) {
                        bool tested = false;
                        for (unsigned i = 0; i < NTESTS; i++)
                                if (!strcmp(tests[i].name, argv[j])) {
                                        tested = true;
                                        write_test_files(&tests[i]);
                                }
                        if (!tested) {
                                failed = true;
                                fprintf(stderr,
                                        "***** No test named %s *****\n",
                                        argv[j]);
                        }
                }
        return failed; /* failed nonzero == exit nonzero == failure */
}


static void write_test_files(struct test_info *test)
{
        FILE *binary = open_and_free_pathname(Fmt_string("%s.um", test->name));
        Seq_T instructions = Seq_new(0);
        test->build_test(instructions);
        Um_write_sequence(binary, instructions);
        Seq_free(&instructions);
        fclose(binary);

        write_or_remove_file(Fmt_string("%s.0", test->name),
                             test->test_input);
        write_or_remove_file(Fmt_string("%s.1", test->name),
                             test->expected_output);
}


static void write_or_remove_file(char *path, const char *contents)
{
        if (contents == NULL || *contents == '\0') {
                remove(path);
        } else {
                FILE *input = fopen(path, "wb");
                assert(input != NULL);

                fputs(contents, input);
                fclose(input);
        }
        free(path);
}


static FILE *open_and_free_pathname(char *path)
{
        FILE *fp = fopen(path, "wb");
        assert(fp != NULL);

        free(path);
        return fp;
}
