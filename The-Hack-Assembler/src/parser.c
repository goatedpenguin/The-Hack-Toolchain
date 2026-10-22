#define _GNU_SOURCE
#include "parser.h"
#include "bitvector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

extern BitVector* bv;

void sanitizeLine(char* line) {
    if (!line) return;

    
    char* comment = strstr(line, "//");
    if (comment) *comment = '\0';

    
    char* src = line;
    char* dst = line;

    while (*src) {
        if (!isspace((unsigned char)*src)) {
            *dst++ = *src;
        }
        src++;
    }
    *dst = '\0';  
}

instrType detectInstrType(const char* line) {
    if (line[0] == '@') {
        return A_INSTR;
    }

    if (line[0] == '(') {
        return L_INSTR;
    }

    return C_INSTR;
}

char* extractSym(const char* line) {
    instrType type = detectInstrType(line);
    int len = strlen(line);
    char* sym = malloc(len); 
    int i = 0, j = 1;

    if (!sym) return NULL;

    switch (type) {
        case A_INSTR:
            while (j < len && line[j] != '\n' && line[j] != '\r') {
                sym[i++] = line[j++];
            }
            break;
        case L_INSTR:
            while (j < len && line[j] != ')' && line[j] != '\n' && line[j] != '\r') {
                sym[i++] = line[j++];
            }
            break;
        case C_INSTR:
            free(sym);
            return NULL;
    }

    sym[i] = '\0';
    return sym;
}

char* extractDest(const char* line) {
    int i = 0;
    int containsEqual = 0;
    char* res = malloc(strlen(line) + 1);

    while (line[i] != '\0') {
        if (line[i] == '=') {
            containsEqual = 1;
            break;
        }
        i++;
    }

    if (!containsEqual) {
        free(res);
        return NULL;
    }

    int j;
    for (j = 0; j < i; j++) {
        res[j] = line[j];
    }

    res[j] = '\0';
    return res;
}

char* extractComp(const char* line) {
    char* comp = malloc(strlen(line) + 1);  
    strcpy(comp, line);         
    int start = -1, end = -1, i = 0;
    int tempIdx = 0;

    while (line[i]) {
        if (line[i] == '=') {
            start = i + 1;
        }
        if (line[i] == ';') {
            end = i;
        }
        i++;
    }

    if (start == -1 && end == -1) {
        return comp;
    } else if (start != -1 && end == -1) {
        int len = strlen(line);
        for (int i = start; i < len; i++) {
            comp[tempIdx] = line[i];
            tempIdx++;
        }
        comp[tempIdx] = '\0';
    } else if (start == -1 && end != -1) {
        for (int i = 0; i < end; i++) {
            comp[tempIdx] = line[i];
            tempIdx++;
        }
        comp[tempIdx] = '\0';
    } else {
        for (int i = start; i < end; i++) {
            comp[tempIdx] = line[i];
            tempIdx++;
        }
        comp[tempIdx] = '\0';
    }
    return comp;
}

char* extractJump(const char* line) {
    int i = 0;
    int containsSemicolon = 0;
    char* res = malloc(strlen(line) + 1);

    while (line[i] != '\0') {
        if (line[i] == ';') {
            containsSemicolon = 1;
            break;
        }
        i++;
    }

    if (!containsSemicolon) {
        free(res);
        return NULL;
    }

    int idx = 0;
    int j = i + 1;
    while (line[j] != '\0') {
        res[idx++] = line[j++];
    }

    res[idx] = '\0';
    return res;
}

void freeParsedPacket(ParsedPacket* packet) {
    if (!packet) return;
    free(packet->dest);
    free(packet->comp);
    free(packet->jump);
    free(packet);
}

ParsedPacket* parseCInstruction(const char* line) {
    ParsedPacket* packet = (ParsedPacket*)malloc(sizeof(ParsedPacket));
    packet->comp = extractComp(line);
    packet->dest = extractDest(line);
    packet->jump = extractJump(line);

    return packet;
}

ParsedPacket* parseAInstruction(symTable* table, const char* line) {
    // detect if a instr is binded to a label, is a var, or a numeric

    ParsedPacket* packet = (ParsedPacket*)malloc(sizeof(ParsedPacket));
    packet->dest = NULL;
    packet->comp = NULL;
    packet->jump = NULL;

    char* symbol = extractSym(line);
    int retval = getSym(table, symbol);

    // label processing
    if (retval != -1) {
        packet->value = retval;
        free(symbol);
        return packet;
    }

    // else, check the string to see whether its a numeric or a var e.x. "5" vs
    // "i" check numeric first if not, then it must be a var:

    if (symbol[0] >= '0' && symbol[0] <= '9') {
        int val = atoi(symbol);
        packet->value = val;
        free(symbol);
        return packet;
    } 

    // bit vector collision and resolve logic goes here
    size_t freeRamAddr = getNextFreeSlot();
    if (freeRamAddr > 16383) {
        free(symbol);
        free(packet);
        return NULL; // to prevent overflow in to screen region memory
    }
    setBit(freeRamAddr); // mark as used
    packet->value = freeRamAddr; // hack addr only go up till 24576, way less than 2^16. Typecast from size_t -> uint16_t is safe here 
    free(symbol);
    return packet;
}
