#define _GNU_SOURCE
#include "assembler.h"
#include "bitvector.h"
#include "codegen.h"
#include "parser.h"
#include "symbol_table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

BitVector* bv = NULL;
FILE* file = NULL;


int main (int argc, char **argv) { 
    initBitVector((size_t) 65536);
    if (argc < 2) {
        fprintf(stderr, "\nUsage: %s <file.asm>\n", argv[0]);
        return EXIT_FAILURE;
    }

    file = fopen(argv[1], "r");
    if (!file) {
        perror("Failed to open file");
        return EXIT_FAILURE;
    } 

    char* programExecNameBuffer = strdup(argv[1]);
    
    char* dot = strrchr(programExecNameBuffer, '.');
    if (dot) *dot = '\0';

    symTable* table = initTable();
    assemblerFirstPass(&table);
    assemblerSecondPass(programExecNameBuffer, table);

    freeBitVector();
    freeTable(table);
    free(programExecNameBuffer);
    fclose(file);
    bv = NULL;
    file = NULL;
    return EXIT_SUCCESS;
}
