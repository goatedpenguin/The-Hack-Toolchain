#ifndef TYPES_H
#define TYPES_H

typedef enum {
    B_LABEL,
    B_GOTO,
    B_IFGOTO,
} branch;

typedef enum {
    MEM_INVALID,
    MEM_TEMP,
    MEM_STATIC,
    MEM_LOCAL,
    MEM_ARG,
    MEM_THIS,
    MEM_THAT,
    MEM_POINTER,
    MEM_CONSTANT,
} memorySegments;

typedef enum {
    A_INVALID,
    A_ADD,
    A_SUB,
    A_NEG,
    A_EQ,
    A_GT,
    A_LT,
    A_AND,
    A_OR,
    A_NOT,
} builtins;

typedef enum {
    C_INVALID,
    C_ARITHMETIC,
    C_BRANCH,
    C_PUSH,
    C_POP,
    F_FUNCTION,
    F_CALL,
    F_RETURN,
} VMCommands;

typedef enum {
    BUILTIN,
    BRANCH,
    MEMORY_CMD,
    FUNC_CMD,
    SEGMENT
} vmWordType;

typedef enum {
    PUSH,
    POP
} instr;

#endif
