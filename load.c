/* 
 * Derek Benson and Norman Young
 * 4.3.15
 * memory.c
 *
 * Implementation for the loading module
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "load.h"

/* 
 * readFile
 * takes in a file and the size of the file
 * parses the file into 32bit codewords according to the specs
 * returns a Memory_T with the first instruction at segment 0
 */
Memory_T readFile(FILE *input, uint64_t size)
{
        size = size / 4;
        uint32_t segment[size];
        assert(segment != NULL);
        for (unsigned int i = 0; i < size; i++) {
                char word[4];
		word[3] = getc(input);        
                word[2] = getc(input);        
                word[1] = getc(input);         
                word[0] = getc(input);        
	        
                /* segment[i] = *((uint32_t *)word); */
                uint32_t *word_ptr = (uint32_t *)word;
		segment[i] = *word_ptr;
        }
        /* initialize memory with first codeword at segment zero */
        return Memory_new(segment, size);
}
