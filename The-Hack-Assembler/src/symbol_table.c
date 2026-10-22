#define _POSIX_C_SOURCE 200809L
#include "symbol_table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INIT_TABLE_SIZE 512 // power of 2 for better perf

symTable* initTable(void) {
    symTable* emptyTable = (symTable*)malloc(sizeof(symTable));
    emptyTable->capacity = INIT_TABLE_SIZE;
    emptyTable->size = 0;
    emptyTable->entries = (entry**)malloc(emptyTable->capacity * sizeof(entry*));

    for (int i = 0; i < emptyTable->capacity; i++) {
        emptyTable->entries[i] = NULL;
    }

    return emptyTable;
}

void freeTable(symTable* table) {
    entry* temp;
    entry* prev;
    for (int i = 0; i < table->capacity; i++) {
        temp = table->entries[i];
        prev = table->entries[i];
        while (temp != NULL) {
            free(temp->name);
            temp = temp->next;
            free(prev);
            prev = temp;
        }
    }
    free(table->entries);
    free(table);
}

unsigned long hash(const char* str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return hash;
}

static symTable* resize(symTable* table) {
    entry* temp;
    int newIdx;

    if (table->size * 3 < 2 * table->capacity) return table;
 
    symTable* newTable = (symTable *) malloc(sizeof(symTable));
    newTable->capacity = table->capacity * 2;
    newTable->size = 0;
    newTable->entries = (entry **) calloc(newTable->capacity, sizeof(entry *));

    for (int i = 0; i < table->capacity; i++) {
        temp = table->entries[i];
        while (temp != NULL) {
            entry* next = temp->next;   
            newIdx = hash(temp->name) % newTable->capacity;
            temp->next = newTable->entries[newIdx];
            newTable->entries[newIdx] = temp;
            temp = next;
            newTable->size++;
        }
    }
    // free only the old array and struct, not the nodes
    free(table->entries);
    free(table);
    return newTable;    
}

void addSym(symTable** table, const char* name, int address) {
    *table = resize(*table);
    int idx = hash(name) % (*table)->capacity;
    entry* temp = (entry*)malloc(sizeof(entry));
    temp->address = address;
    temp->name = strdup(name);
    temp->next = (*table)->entries[idx];
    (*table)->entries[idx] = temp;
    ((*table)->size++);
}

int getSym(symTable* table, const char* name) {
    if (!table || !name) return -1;
    int idx = hash(name) % table->capacity;
    entry* temp = table->entries[idx];

    while (temp != NULL) {
        if (strcmp(temp->name, name) == 0) {
            return temp->address;
        }
        temp = temp->next;
    }
    return -1;
}