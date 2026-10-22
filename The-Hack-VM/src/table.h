#ifndef TABLE_H
#define TABLE_H

#define TABLE_SIZE 53
#include <stdint.h>
#include "types.h"

typedef struct {
    vmWordType tag;
    union {
        memorySegments segment;
        builtins builtin;
        VMCommands command;
        branch branchOperator;
    } vmCommandType;
} vmWord;

typedef struct commandEntry {
    char* key;
    vmWord* val;
    struct commandEntry* next;
} entry;

// Note each entry is guaranteed to be unique.
// Note this is a read only table hence, the fixed entry size.
typedef struct {
    entry** entries;
} table;

table* initTable();
vmWord* getCommand(table* hashTable, const char* key);
void buildVMTable(table* hashTable);
void freeTable(table* hashTable);

#endif
