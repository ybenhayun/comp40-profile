/* 
 * Derek Benson and Norman Young
 * 3.31.15
 * memory.c
 *
 * Interface for the loading module
 *
 */

#include "memory_new.h"

#ifndef LOAD_H
#define LOAD_H

/* reads file and parses instructions into 32bit codewords */
Memory_T readFile(FILE *input, uint64_t size);

#endif