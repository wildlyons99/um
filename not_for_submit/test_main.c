/* testing fun edge cases */

#include <stdint.h>

#include <seq.h>
#include <assert.h>

#include <stdio.h>
#include <stdlib.h>

#include <mem.h>
#include <except.h>
#include <bitpack.h>

#include "decode.h"
#include "registers.h"
#include "segmem.h"

extern const Except_T Mem_Failed;

int main()
{
        // // uint32_t size = (~0) >> 1;
        // Seq_T mem = Seq_new(0x5bffffff);
        // uint32_t size = ~0;
        // fprintf(stderr, "Size: %u\n", size);
        // TRY
        //         for (uint32_t i = 0; i < size; i++) {
        //                 Seq_addhi(mem, (void *)(uintptr_t)i);     
        //         }
                
        // EXCEPT (Mem_Failed)
        //         fprintf(stderr, "Mem failed error\n");

        // ELSE 
        //         fprintf(stderr, "Something else happened\n"); 
                                
        // END_TRY; 

        // printf("Sequence length after adding many: %u\n", Seq_length(mem)); 

        // TRY
        //         Seq_addhi(mem, (void *)(uintptr_t)10);

        // EXCEPT (Mem_Failed)
        //         fprintf(stderr, "AFTER: Mem failed error\n");

        // ELSE 
        //         fprintf(stderr, "AFTER: Something else happened\n"); 
                                
        // END_TRY; 

        // printf("Int length: %d\n", Seq_length(mem)); 
        // printf("Long length: %u\n", Seq_length(mem)); 

        /* Decode it */
        // unsigned rA, rB, rC, loadval_rA;
        // uint32_t loadval_value;
        // Um_opcode opcode = decode_word(0xffffffff, &rA, &rB, &rC, &loadval_rA, 
        //                         &loadval_value);

        // printf("%u\n", opcode);

        uint32_t size = ~0;
        printf("%u\n", size);
        size = size / 2;
        printf("%u\n", size);
        size = size / 2;
        printf("%u\n", size);
        // size = size + 1;
        Seq_T seq = Seq_new(size);
        Seq_free(&seq);
        
}