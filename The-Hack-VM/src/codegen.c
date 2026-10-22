#define _POSIX_C_SOURCE 200809L
#include "codegen.h"
#include "mnemonics.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

static int compareIdx = 0;
static int callIdx = 0;
static int functionIdx = 0;
static char* currentFunction = NULL;

static char* formatTemplate(const char** lines, ...) {
    size_t len = 0;
    for (int i = 0; lines[i] != NULL; i++)
        len += strlen(lines[i]) + 1;

    char* fmt = malloc(len + 1);
    size_t idx = 0;
    for (int i = 0; lines[i] != NULL; i++) {
        size_t lineLen = strlen(lines[i]);
        memcpy(fmt + idx, lines[i], lineLen);
        idx += lineLen;
        fmt[idx++] = '\n';
    }
    fmt[idx] = '\0';

    va_list args, copy;
    va_start(args, lines);
    va_copy(copy, args);
    int bufferSize = vsnprintf(NULL, 0, fmt, copy);
    va_end(copy);
    if (bufferSize < 0) {
        va_end(args);
        free(fmt);
        return NULL;
    }
    char* res = malloc((size_t)bufferSize + 1);
    vsnprintf(res, (size_t)bufferSize + 1, fmt, args);
    va_end(args);
    free(fmt);
    return res;
}

void setCodegenFile(const char* fileName) {
    free(currentFunction);
    currentFunction = fileName ? strdup(fileName) : NULL;
}

static char* emitCallStr(const char* functionName, int nArgs) {
    int id = callIdx++;
    return formatTemplate(call, id, nArgs, functionName, id);
}

char* bootstrapCode(void) {
    char* init = emitCallStr("Sys.init", 0);
    if (!init) return NULL;
    const char* prefix = "@256\nD=A\n@SP\nM=D\n";
    const char* end = "(__VM$HALT)\n@__VM$HALT\n0;JMP\n";
    char* res = malloc(strlen(prefix) + strlen(init) + strlen(end) + 1);
    strcpy(res, prefix);
    strcat(res, init);
    strcat(res, end);
    free(init);
    return res;
}

static char* emitCompareOp(const char* jump) {
    int id = compareIdx++;
    if (strcmp(jump, "JEQ") == 0) return formatTemplate(compareOp, id, jump, id, id, id);

    const char* positive = strcmp(jump, "JGT") == 0 ? "TRUE" : "FALSE";
    const char* negative = strcmp(jump, "JGT") == 0 ? "FALSE" : "TRUE";
    return formatTemplate(compareSigned, id, positive, id, id, id, negative, id, id, id, jump, id, id, id, id);
}

char* codeGen(VMPacket* vmPacket, const char* fileName) {
    if (!vmPacket) return NULL;

    if (vmPacket->type == BUILTIN) {
        switch (vmPacket->commandContext.nativeOperations.nativeOPS) {
        case A_ADD:
            return formatTemplate(binaryOp, "D+M");
        case A_SUB:
            return formatTemplate(binaryOp, "M-D");
        case A_AND:
            return formatTemplate(binaryOp, "D&M");
        case A_OR:
            return formatTemplate(binaryOp, "D|M");
        case A_NEG:
            return formatTemplate(unaryOp, "-");
        case A_NOT:
            return formatTemplate(unaryOp, "!");
        case A_EQ:
            return emitCompareOp("JEQ");
        case A_GT:
            return emitCompareOp("JGT");
        case A_LT:
            return emitCompareOp("JLT");
        default:
            return NULL;
        }
    }

    if (vmPacket->type == BRANCH) {
        const char* scope = currentFunction ? currentFunction : fileName;
        const char* name = vmPacket->toks->tokens[1];
        switch (vmPacket->commandContext.branching.branchCommand) {
        case B_LABEL:
            return formatTemplate(label, scope, name);
        case B_GOTO:
            return formatTemplate(vmGoto, scope, name);
        case B_IFGOTO:
            return formatTemplate(ifGoto, scope, name);
        }
    }

    if (vmPacket->type == MEMORY_CMD) {
        int off = vmPacket->commandContext.memManage.offset;
        memorySegments seg = vmPacket->commandContext.memManage.segment;
        int push = vmPacket->commandContext.memManage.stackInstr == PUSH;
        const char* base = NULL;
        switch (seg) {
        case MEM_CONSTANT:
            return push ? formatTemplate(pushConstant, off) : NULL;
        case MEM_TEMP:
            return formatTemplate(push ? pushTemp : popTemp, off);
        case MEM_POINTER:
            return formatTemplate(push ? pushPointer : popPointer, off == 0 ? "THIS" : "THAT");
        case MEM_STATIC:
            return formatTemplate(push ? pushStatic : popStatic, fileName, off);
        case MEM_LOCAL:
            base = "LCL";
            break;
        case MEM_ARG:
            base = "ARG";
            break;
        case MEM_THIS:
            base = "THIS";
            break;
        case MEM_THAT:
            base = "THAT";
            break;
        default:
            return NULL;
        }
        if (push) return formatTemplate(pushSegment, off, base);
        return formatTemplate(popSegment, base, off);
    }

    if (vmPacket->type == FUNC_CMD) {
        VMCommands cmd = vmPacket->commandContext.functionInfo.command;
        const char* name = vmPacket->commandContext.functionInfo.functionName;
        if (cmd == F_FUNCTION) {
            setCodegenFile(name);
            int id = functionIdx++;
            return formatTemplate(function, name, vmPacket->commandContext.functionInfo.nLocalVars, id, id, id, id);
        }
        if (cmd == F_CALL) return emitCallStr(name, vmPacket->commandContext.functionInfo.nArgs);
        if (cmd == F_RETURN) return formatTemplate(vmReturn);
    }
    return NULL;
}
