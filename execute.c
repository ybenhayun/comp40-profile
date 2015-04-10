/* 
 * Derek Benson and Norman Young
 * 3.31.15
 * memory.c
 *
 * Interface for the execute module.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "bitpack.h"
#include "execute.h"

/* PRIVATE FUNCTIONS */
static inline uint32_t fetch(uint32_t progcounter);
static bool decode_and_execute(Memory_T memory, uint32_t inst, 
                               uint32_t *progcounter, uint32_t *registers);
static uint32_t *segmentzero;

enum opcodes {MOVE = 0, LOAD, STORE, ADD, MULT, DIV, NAND, HALT, MAP, UNMAP,
              OUTPUT, INPUT, LOADP, LOADV };
/*
 * execute takes a Memory_T that holds an initialized segment zero. 
 * Execute initalizes the program counter, the registers, and segment zero.
 * It runs an execution loop until the program is halted by the um
 * halt instruction.
 */


void execute (Memory_T memory)
{
        uint32_t progcounter = 0;
        uint32_t registers[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    	segmentzero = retrievesegment(memory, 0);
    	bool halted = false;
        while (!halted) {
                halted = decode_and_execute(memory, fetch(progcounter),
                                            &progcounter, registers);
        }
}


/* fetch returns the instruction located at the current program counter */

static inline uint32_t fetch(uint32_t progcounter)
{
	return segmentzero[progcounter];
}

/*
 * decode_and_execute takes a Memory_T, the current instruction, the current
 * program counter, and the machine registers.
 * It parses the opcode and then switches based on the opcode to run that 
 * specific instruction. The function returns false for any valid instruction
 * that is not halt. An invalid instruction or a halt instruction will return
 * true and halt execution of the UM.
 */

static inline bool decode_and_execute(Memory_T memory, uint32_t inst, 
                               uint32_t *progcounter, uint32_t *registers)
{
        uint32_t opcode = inst >> 28;
        uint32_t ra, rb, rc;

        /* Separate LOADV out because of it's different codeword structure */
        if (opcode == LOADV) {
                ra = (inst << 4) >> 29;
                registers[ra] = (inst << 7) >> 7;

        } else {
                /* Parse the register information */
                ra = (inst << 23) >> 29;
                rb = (inst << 26) >> 29;
                rc = (inst << 29) >> 29;

                /* Determine the instruction */
                switch(opcode)
                {
                        case MOVE:
                                if (registers[rc] != 0) 
                                        registers[ra] = registers[rb];
                        break;
                        case LOAD:
                                registers[ra] = segload(memory, registers[rb], 
                                                        registers[rc]);

                        break;
                        case STORE:
                                segstore(memory, registers[ra], registers[rb],
                                         registers[rc]);

                        break;
                        case ADD:
                                registers[ra] = registers[rb] + registers[rc];
                        break;
                        case MULT:
                                registers[ra] = registers[rb] * registers[rc];
                        break;
                        case DIV:
                                registers[ra] = registers[rb] / registers[rc];
                        break;
                        case NAND:
                                registers[ra] = 
                                              ~(registers[rb] & registers[rc]);
                        break;
                        case HALT:
                                return true;
                        break;
                        case MAP:
                                registers[rb] = map(memory, registers[rc]);
                        break;
                        case UNMAP:
                                unmap(memory, registers[rc]);
                        break;
                        case OUTPUT:
                                putchar(registers[rc] & 0xff);
                        break;
                        case INPUT:;
                                int x = getchar();
                                if (x != EOF) {
                                    registers[rc] = x & 0xff;
                                } else {
                                    registers[rc] = 0xffffffff;
                                }
                        break;
                        case LOADP:
                                if(registers[rb] != 0) {
                                        loadprog(memory, registers[rb]);
                                        segmentzero = 
                                                    retrievesegment(memory, 0);
                                }
                                *progcounter = registers[rc];
                                return false;
                        break;
                        default: 
                                return true;
                }
        }
        /* increment program counter and continue execution */
        (*progcounter)++;
        return false;
}
