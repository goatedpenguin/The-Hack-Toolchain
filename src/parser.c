#define _POSIX_C_SOURCE 200809L
#include "parser.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static tokLine* tokenize(const char* line) {
    tokLine* tok = malloc(sizeof(tokLine));
    char* str = strdup(line);
    tok->numToks = 0;
    tok->tokens = NULL;
    for (char* temp = strtok(str, " \t\r\n"); temp; temp = strtok(NULL, " \t\r\n")) {
        tok->tokens = realloc(tok->tokens, (tok->numToks + 1) * sizeof(char*));
        tok->tokens[tok->numToks++] = strdup(temp);
    }
    free(str);
    return tok;
}

static int validSymbol(const char* name) {
    if (!*name || (!isalpha((unsigned char)*name) && !strchr("_.$:", *name))) return 0;
    for (; *name; name++) {
        if (!isalnum((unsigned char)*name) && !strchr("_.$:", *name)) return 0;
    }
    return 1;
}

static int getNumber(const char* str) {
    int value = 0;
    if (!*str) return -1;
    for (; *str; str++) {
        if (*str < '0' || *str > '9') return -1;
        value = value * 10 + (*str - '0');
        if (value > 32767) return -1;
    }
    return value;
}

void freeVMPacket(VMPacket* packet) {
    if (!packet) return;
    for (int i = 0; i < packet->toks->numToks; i++)
        free(packet->toks->tokens[i]);
    free(packet->toks->tokens);
    free(packet->toks);
    if (packet->type == FUNC_CMD) free(packet->commandContext.functionInfo.functionName);
    free(packet);
}

VMPacket* buildVMPacket(const char* command, table* hashTable) {
    VMPacket* packet = calloc(1, sizeof(VMPacket));
    tokLine* toks = tokenize(command);
    packet->toks = toks;
    if (!toks->numToks) goto invalid;

    vmWord* entry = getCommand(hashTable, toks->tokens[0]);
    if (!entry || entry->tag == SEGMENT) goto invalid;
    packet->type = entry->tag;

    int expected = 3;
    if (entry->tag == BUILTIN || (entry->tag == FUNC_CMD && entry->vmCommandType.command == F_RETURN))
        expected = 1;
    else if (entry->tag == BRANCH)
        expected = 2;
    if (toks->numToks != expected) goto invalid;

    int number = 0;
    if (expected == 3) {
        number = getNumber(toks->tokens[2]);
        if (number < 0) goto invalid;
    }

    switch (packet->type) {
    case BUILTIN:
        packet->commandContext.nativeOperations.nativeOPS = entry->vmCommandType.builtin;
        break;
    case BRANCH:
        if (!validSymbol(toks->tokens[1])) goto invalid;
        packet->commandContext.branching.branchCommand = entry->vmCommandType.branchOperator;
        break;
    case MEMORY_CMD: {
        vmWord* segment = getCommand(hashTable, toks->tokens[1]);
        if (!segment || segment->tag != SEGMENT) goto invalid;
        memorySegments seg = segment->vmCommandType.segment;
        instr stackInstr = entry->vmCommandType.command == C_PUSH ? PUSH : POP;
        if ((seg == MEM_CONSTANT && stackInstr == POP) || (seg == MEM_POINTER && number > 1) ||
            (seg == MEM_TEMP && number > 7) || (seg == MEM_STATIC && number > 239))
            goto invalid;
        packet->commandContext.memManage.segment = seg;
        packet->commandContext.memManage.offset = number;
        packet->commandContext.memManage.stackInstr = stackInstr;
        break;
    }
    case FUNC_CMD:
        packet->commandContext.functionInfo.command = entry->vmCommandType.command;
        if (entry->vmCommandType.command != F_RETURN) {
            if (!validSymbol(toks->tokens[1])) goto invalid;
            packet->commandContext.functionInfo.functionName = strdup(toks->tokens[1]);
            if (entry->vmCommandType.command == F_FUNCTION)
                packet->commandContext.functionInfo.nLocalVars = number;
            else
                packet->commandContext.functionInfo.nArgs = number;
        }
        break;
    default:
        goto invalid;
    }
    return packet;

invalid:
    freeVMPacket(packet);
    return NULL;
}
