#ifndef PARSER
#define PARSER
#include "symbol_table.h"
#include <stdint.h>
#include <stdio.h>
#include <ctype.h>

typedef enum { A_INSTR, C_INSTR, L_INSTR } instrType;

typedef struct parsedPacket {
    uint16_t value; // numeric val of a instr
    char* symbol; // for dbg
    char* comp;
    char* dest;
    char* jump;
} ParsedPacket;

void sanitizeLine(char* line);

instrType detectInstrType(const char* line);

char* extractSym(const char* line);
char* extractDest(const char* line);
char* extractComp(const char* line);
char* extractJump(const char* line);

ParsedPacket* parseCInstruction(const char* line);
ParsedPacket* parseAInstruction(symTable* table, const char* line);
void freeParsedPacket(ParsedPacket* packet);

#endif