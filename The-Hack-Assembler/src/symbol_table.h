#ifndef SYMBOL_TABLE
#define SYMBOL_TABLE

typedef struct entry {
    char* name;
    int address;
    struct entry* next;
} entry;

typedef struct {
    entry** entries;
    int capacity;
    int size;
} symTable;

symTable* initTable(void);
void freeTable(symTable* table);
unsigned long hash(const char* str);
void addSym(symTable** table, const char* name, int address);
int getSym(symTable* table, const char* name);

#endif
