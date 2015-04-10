/* 
 * Derek Benson and Norman Young
 * 4.3.15
 * memory.c
 *
 * main for UM emulator
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "memory.h"
#include "load.h"
#include "execute.h"

int main(int argc, char *argv[])
{
        if (argc == 1) {
                printf("You need to enter a file");
                exit(1);
        } else if (argc > 2) {
                printf("Too many arguments");
                exit(1);
        } else {
                for (int i = 1; i < argc; i++) {
                        FILE *fp = fopen(argv[i], "r");
                        if (fp == NULL) {
                                fprintf(stderr, 
                                        "%s: %s %s %s\n",
                                        argv[0], "Could not open file ",
                                        argv[i], "for reading");
                                exit(1);
                        }
                        /* Determine file size */
                        struct stat file;
                        stat(argv[i], &file);

                        /* Create Virtual Memory */
                        Memory_T memory = readFile(fp, (uint64_t)file.st_size);

                        /* Execute UM */
                        execute(memory);

                        /* Cleanup Memory */
                        Memory_free(&memory);
                        fclose(fp);
                }
        }
        return 0;
}


