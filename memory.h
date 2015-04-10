/* 
 * Derek Benson and Norman Young
 * 3.31.15
 * memory.h
 *
 * Interface for the memory module of the UM
 *
 */

#include <stdint.h>

#ifndef MEMORY_H
#define MEMORY_H

#define T Memory_T
typedef struct T *T;

/* allocates space for a new memory and places the given segment at segment0 */
T Memory_new(uint32_t *segment, uint32_t size);

/* deallocates space for the given memory */
void Memory_free(T *memory);

/* allocates space for a new segment given the size */
uint32_t map(T memory, uint32_t size);

/* deallocates memory for a segment given the segment ID */
void unmap(T memory, uint32_t segmentid);

/* returns the value located in memory at $m[$r[b]][$r[c]] */
uint32_t segload(T memory, uint32_t segmentid, uint32_t offset);

/* stores the given word c at the location in memory $m[$r[a]][$r[b]] */
void segstore(T memory, uint32_t segmentid, uint32_t offset, uint32_t val);

/* Duplicates segment at $m[$r[b]]; duplicate replaces segment 0 */
void loadprog(T memory, uint32_t segmentid);

/* Returns the array of values in the segment */
uint32_t* retrievesegment(T memory, uint32_t segmentid);

#undef T
#endif
