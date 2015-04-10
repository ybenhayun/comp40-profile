/* 
 * Derek Benson and Norman Young
 * 3.31.15
 * memory.c
 *
 * Implementation for the memory module of the UM
 *
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stack.h>
#include <seq.h>
#include <assert.h>
#include "memory.h"

#define T Memory_T

/* private struct to simulate the memory */
struct T {
        Seq_T disk;             /* sequence of memory */
        uint32_t next_id;
        Stack_T unmapped;       /* contains segment IDs of unmapped segments */
};

struct segment{
        uint32_t size;
        uint32_t id;
        uint32_t values[];
};

/*
 * Memory_new
 * takes in an initial segment and the size of the segment, returns a new
 * Memory_T struct with the initial segment value at segment zero
 */
T Memory_new(uint32_t *segment, uint32_t size)
{

        T memory = malloc(sizeof(*memory));
        assert(memory != NULL);
        memory->disk = Seq_new(1);
        memory->unmapped = Stack_new();
        memory->next_id = 1;

        struct segment *zero = 
                        malloc(sizeof(*zero) + size * sizeof(*zero->values));
        assert(zero != NULL);
        for (unsigned i = 0; i < size; i++) {
                zero->values[i] = segment[i];
        }
        zero->size = size;
        zero->id = 0;
        Seq_addlo(memory->disk, zero);
        return memory;
}

/* 
 * Memory_free
 * frees the memory and its segments
 */
void Memory_free(T *memory)
{
        int size = Seq_length((*memory)->disk);
        for (int i = 0; i < size; i++) {
                struct segment *segment = Seq_get((*memory)->disk, i);
                if (segment != NULL) {
                        unmap((*memory), i);
                }
        }
        Seq_free(&((*memory)->disk));
        Stack_free(&((*memory)->unmapped));
        free(*memory);
}

/*
 * map
 * takes in a memory and the size of the segment to be mapped
 * returns the segmend ID of the newly mapped segment
 */
uint32_t map(T memory, uint32_t size)
{
        struct segment *seg = 
                        malloc(sizeof(*seg) + size * sizeof(*seg->values));
        assert(seg != NULL);
        seg->size = size;

        /* initializes new segment values to zero */
        for(unsigned i = 0; i < size; i++) {
                seg->values[i] = 0;
        }

        /* checks stack for unmapped segment IDs */
        if (Stack_empty(memory->unmapped)) {
                /* if it's empty, add to and increment next segment ID */
                Seq_addhi(memory->disk, seg);
                seg->id = memory->next_id;
                memory->next_id++;
        }
        else {
                /* stack not empty, map to ID on the top of the stack */
                uint32_t index = 
                            (uint32_t)(uintptr_t)Stack_pop(memory->unmapped);
                seg->id = index;
                Seq_put(memory->disk, seg->id, seg);
        }
        return seg->id;
}

/* 
 * unmap
 * takes in a memory and the segment ID of the segment to the unmapped 
 * unmaps the segment and adds its ID to the stack
 */
void unmap(T memory, uint32_t segmentid)
{       
        struct segment *segment = Seq_get(memory->disk, segmentid);
        free(segment);
        Seq_put(memory->disk, segmentid, NULL);
        /* add ID to stack */
        Stack_push(memory->unmapped, (void *)(uintptr_t)segmentid);
}

/*
 * segload
 * takes in a memory, a segment ID and the offset of the targeted value to 
 * be loaded
 * returns the value at that segment and offset
 */
uint32_t segload(T memory, uint32_t segmentid, uint32_t offset)
{       
        struct segment *segment = Seq_get(memory->disk, segmentid);
        return segment->values[offset];
}

/*
 * segstore
 * takes in a memory, a segment ID, an offset, and the value to be stored
 * stores the value in the specified segment and offset
 */
void segstore(T memory, uint32_t segmentid, uint32_t offset, uint32_t val)
{
        struct segment *segment = Seq_get(memory->disk, segmentid);
        segment->values[offset] = val; 
}

/*
 * loadprog
 * takes in a memory and a segment ID
 * takes the segment at the specified ID, duplicates the segment and loads it
 * at segment zero to be run
 */
void loadprog(T memory, uint32_t segmentid)
{
        struct segment *segment = Seq_get(memory->disk, segmentid);
        struct segment *zero = malloc(sizeof(*zero) + 
                                      segment->size*sizeof(*segment->values));
        assert(zero != NULL);
        unsigned size = segment->size;
        /* duplicate specified segment */
        for (unsigned i = 0; i < size; ++i)
        {
                zero->values[i] = segment->values[i];
        }
        zero->id = 0;
        zero->size = segment->size;

        /* load it at segment 0 */
        struct segment *old = Seq_put(memory->disk, 0, zero);
        free(old);
}

/*
 * retrievesegment
 * takes in a memory and segment ID and returns a pointer to the array of 
 * values in the specified segment
 */
uint32_t* retrievesegment(T memory, uint32_t segmentid)
{
	struct segment *segment = Seq_get(memory->disk, segmentid);
	return segment->values;
}

#undef T
