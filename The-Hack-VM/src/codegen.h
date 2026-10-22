#ifndef CODEGEN_H
#define CODEGEN_H
#include "parser.h"
#include "types.h"

char* codeGen(VMPacket* vmPacket, const char* fileName);
void setCodegenFile(const char* fileName);
char* bootstrapCode(void);

#endif
