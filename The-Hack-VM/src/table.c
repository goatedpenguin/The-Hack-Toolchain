#define _POSIX_C_SOURCE 200809L
#include "table.h"
#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

table* initTable() {
    table* hashTable = (table*)malloc(sizeof(table));
    hashTable->entries = (entry**)malloc(TABLE_SIZE * sizeof(entry*));
    for (int i = 0; i < TABLE_SIZE; i++) {
        hashTable->entries[i] = NULL;
    }
    return hashTable;
}

void freeTable(table* hashTable) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        entry* cur = hashTable->entries[i];
        while (cur) {
            entry* next = cur->next;
            free(cur->key);
            free(cur->val);
            free(cur);
            cur = next;
        }
    }
    free(hashTable->entries);
    free(hashTable);
}

static uint32_t hash(const char* key) {
    uint32_t hash = 0;

    while (*key != '\0') {
        hash += (uint8_t)*key;
        hash += (hash << 10);
        hash ^= (hash >> 6);
        key++;
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash;
}

static void addCommand(table* hashTable, const char* key, vmWord* val) {
    int idx = hash(key) % TABLE_SIZE;
    entry* new = (entry*)malloc(sizeof(entry));
    new->next = hashTable->entries[idx];
    hashTable->entries[idx] = new;
    new->key = strdup(key);
    new->val = val;
}

vmWord* getCommand(table* hashTable, const char* key) {
    int idx = hash(key) % TABLE_SIZE;
    entry* cur = hashTable->entries[idx];
    while (cur != NULL) {
        if (strcmp(cur->key, key) == 0) {
            return cur->val;
        }
        cur = cur->next;
    }
    return NULL;
}

static vmWord* createSegment(memorySegments seg) {
    vmWord* word = (vmWord*)malloc(sizeof(vmWord));
    word->tag = SEGMENT;
    word->vmCommandType.segment = seg;
    return word;
}

static vmWord* createBuiltin(builtins b) {
    vmWord* word = (vmWord*)malloc(sizeof(vmWord));
    word->tag = BUILTIN;
    word->vmCommandType.builtin = b;
    return word;
}

static vmWord* createMemoryCmd(VMCommands cmd) {
    vmWord* word = (vmWord*)malloc(sizeof(vmWord));
    word->tag = MEMORY_CMD;
    word->vmCommandType.command = cmd;
    return word;
}

static vmWord* createFuncCmd(VMCommands cmd) {
    vmWord* word = (vmWord*)malloc(sizeof(vmWord));
    word->tag = FUNC_CMD;
    word->vmCommandType.command = cmd;
    return word;
}

static vmWord* createBranch(branch branchOperator) {
    vmWord* word = (vmWord*)malloc(sizeof(vmWord));
    word->tag = BRANCH;
    word->vmCommandType.branchOperator = branchOperator;
    return word;
}

void buildVMTable(table* hashTable) {
    addCommand(hashTable, "push", createMemoryCmd(C_PUSH));
    addCommand(hashTable, "pop", createMemoryCmd(C_POP));

    addCommand(hashTable, "function", createFuncCmd(F_FUNCTION));
    addCommand(hashTable, "call", createFuncCmd(F_CALL));
    addCommand(hashTable, "return", createFuncCmd(F_RETURN));

    addCommand(hashTable, "label", createBranch(B_LABEL));
    addCommand(hashTable, "goto", createBranch(B_GOTO));
    addCommand(hashTable, "if-goto", createBranch(B_IFGOTO));

    addCommand(hashTable, "add", createBuiltin(A_ADD));
    addCommand(hashTable, "sub", createBuiltin(A_SUB));
    addCommand(hashTable, "neg", createBuiltin(A_NEG));
    addCommand(hashTable, "eq", createBuiltin(A_EQ));
    addCommand(hashTable, "gt", createBuiltin(A_GT));
    addCommand(hashTable, "lt", createBuiltin(A_LT));
    addCommand(hashTable, "and", createBuiltin(A_AND));
    addCommand(hashTable, "or", createBuiltin(A_OR));
    addCommand(hashTable, "not", createBuiltin(A_NOT));

    addCommand(hashTable, "argument", createSegment(MEM_ARG));
    addCommand(hashTable, "local", createSegment(MEM_LOCAL));
    addCommand(hashTable, "static", createSegment(MEM_STATIC));
    addCommand(hashTable, "constant", createSegment(MEM_CONSTANT));
    addCommand(hashTable, "this", createSegment(MEM_THIS));
    addCommand(hashTable, "that", createSegment(MEM_THAT));
    addCommand(hashTable, "pointer", createSegment(MEM_POINTER));
    addCommand(hashTable, "temp", createSegment(MEM_TEMP));
}
