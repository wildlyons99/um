/* testing fun edge cases */

#include <stdint.h>

#include <seq.h>
#include <assert.h>

#include <stdio.h>
#include <stdlib.h>

#include <mem.h>
#include <except.h>

extern const Except_T Mem_Failed;

int main()
{
        // uint32_t size = (~0) >> 1;
        Seq_T mem = Seq_new(0x5bffffff);
        uint32_t size = ~0;
        fprintf(stderr, "Size: %u\n", size);
        TRY
                for (uint32_t i = 0; i < size; i++) {
                        Seq_addhi(mem, (void *)(uintptr_t)i);     
                }
                
        EXCEPT (Mem_Failed)
                fprintf(stderr, "Mem failed error\n");

        ELSE 
                fprintf(stderr, "Something else happened\n"); 
                                
        END_TRY; 

        printf("Sequence length after adding many: %u\n", Seq_length(mem)); 

        TRY
                Seq_addhi(mem, (void *)(uintptr_t)10);

        EXCEPT (Mem_Failed)
                fprintf(stderr, "AFTER: Mem failed error\n");

        ELSE 
                fprintf(stderr, "AFTER: Something else happened\n"); 
                                
        END_TRY; 

        printf("Int length: %d\n", Seq_length(mem)); 
        printf("Long length: %u\n", Seq_length(mem)); 


        
}