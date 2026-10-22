#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "codegen.h"
#include "parser.h"
#include "symbol_table.h"
#include <stdio.h>

extern FILE* file;

void assemblerFirstPass(symTable** table);
void assemblerSecondPass(const char* programExecutableName, symTable* table);

/*
This function:

Parses the syntax.

Resolves symbols in A-instructions:

If it’s a numeric constant, store it as-is.

If it’s a symbol:

If it exists in symTable, fetch its address.

If not, assign it the next available RAM address (starting at 16) and store it.

Fills out the ParsedPacket.
*/

#endif
