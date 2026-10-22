#ifndef MNEMONICS_H
#define MNEMONICS_H
#include <stddef.h>

static const char* pushConstant[] = {
    "@%d",
    "D=A",
    "@SP",
    "A=M",
    "M=D",
    "@SP",
    "M=M+1",
    NULL
};

static const char* pushSegment[] = {
    "@%d",
    "D=A",
    "@%s",
    "A=D+M",
    "D=M",
    "@SP",
    "A=M",
    "M=D",
    "@SP",
    "M=M+1",
    NULL
};

static const char* pushTemp[] = {
    "@R5",
    "D=A",
    "@%d",
    "A=D+A",
    "D=M",
    "@SP",
    "A=M",
    "M=D",
    "@SP",
    "M=M+1",
    NULL
};

static const char* pushPointer[] = {
    "@%s",
    "D=M",
    "@SP",
    "A=M",
    "M=D",
    "@SP",
    "M=M+1",
    NULL
};

static const char* pushStatic[] = {
    "@%s.%d",
    "D=M",
    "@SP",
    "A=M",
    "M=D",
    "@SP",
    "M=M+1",
    NULL
};

static const char* popSegment[] = {
    "@%s",
    "D=M",
    "@%d",
    "D=D+A",
    "@R13",
    "M=D",
    "@SP",
    "M=M-1",
    "A=M",
    "D=M",
    "@R13",
    "A=M",
    "M=D",
    NULL
};

static const char* popTemp[] = {
    "@R5",
    "D=A",
    "@%d",
    "D=D+A",
    "@R13",
    "M=D",
    "@SP",
    "AM=M-1",
    "D=M",
    "@R13",
    "A=M",
    "M=D",
    NULL
};

static const char* popPointer[] = {
    "@SP",
    "M=M-1",
    "A=M",
    "D=M",
    "@%s",
    "M=D",
    NULL
};

static const char* popStatic[] = {
    "@SP",
    "M=M-1",
    "A=M",
    "D=M",
    "@%s.%d",
    "M=D",
    NULL
};

static const char* binaryOp[] = {
    "@SP",
    "AM=M-1",
    "D=M",
    "@SP",
    "AM=M-1",
    "D=%s",
    "M=D",
    "@SP",
    "M=M+1",
    NULL
};

static const char* unaryOp[] = {
    "@SP",
    "AM=M-1",
    "M=%sM",
    "@SP",
    "M=M+1",
    NULL
};

static const char* compareOp[] = {
    "@SP",
    "AM=M-1",
    "D=M",
    "@SP",
    "AM=M-1",
    "D=M-D",
    "@__VM$CMP_TRUE.%d",
    "D;%s",
    "D=0",
    "@SP",
    "A=M",
    "M=D",
    "@__VM$CMP_END.%d",
    "0;JMP",
    "(__VM$CMP_TRUE.%d)",
    "D=-1",
    "@SP",
    "A=M",
    "M=D",
    "(__VM$CMP_END.%d)",
    "@SP",
    "M=M+1",
    NULL
};

static const char* compareSigned[] = {
    "@SP",
    "AM=M-1",
    "D=M",
    "@R13",
    "M=D",
    "@SP",
    "A=M-1",
    "D=M",
    "@__VM$CMP_XNEG.%d",
    "D;JLT",
    "@R13",
    "D=M",
    "@__VM$CMP_%s.%d",
    "D;JLT",
    "@__VM$CMP_SAME.%d",
    "0;JMP",
    "(__VM$CMP_XNEG.%d)",
    "@R13",
    "D=M",
    "@__VM$CMP_%s.%d",
    "D;JGE",
    "(__VM$CMP_SAME.%d)",
    "@SP",
    "A=M-1",
    "D=M",
    "@R13",
    "D=D-M",
    "@__VM$CMP_TRUE.%d",
    "D;%s",
    "(__VM$CMP_FALSE.%d)",
    "@SP",
    "A=M-1",
    "M=0",
    "@__VM$CMP_END.%d",
    "0;JMP",
    "(__VM$CMP_TRUE.%d)",
    "@SP",
    "A=M-1",
    "M=-1",
    "(__VM$CMP_END.%d)",
    NULL
};

static const char* label[] = {
    "(%s$%s)",
    NULL
};

static const char* vmGoto[] = {
    "@%s$%s",
    "0;JMP",
    NULL
};

static const char* ifGoto[] = {
    "@SP",
    "M=M-1",
    "A=M",
    "D=M",
    "@%s$%s",
    "D;JNE",
    NULL
};

static const char* call[] = {
    "@__VM$RET.%d",
    "D=A",
    "@SP",
    "A=M",
    "M=D",
    "@SP",
    "M=M+1",
    "@LCL",
    "D=M",
    "@SP",
    "A=M",
    "M=D",
    "@SP",
    "M=M+1",
    "@ARG",
    "D=M",
    "@SP",
    "A=M",
    "M=D",
    "@SP",
    "M=M+1",
    "@THIS",
    "D=M",
    "@SP",
    "A=M",
    "M=D",
    "@SP",
    "M=M+1",
    "@THAT",
    "D=M",
    "@SP",
    "A=M",
    "M=D",
    "@SP",
    "M=M+1",
    "@%d",
    "D=A",
    "@5",
    "D=D+A",
    "@SP",
    "D=M-D",
    "@ARG",
    "M=D",
    "@SP",
    "D=M",
    "@LCL",
    "M=D",
    "@%s",
    "0;JMP",
    "(__VM$RET.%d)",
    NULL
};

static const char* function[] = {
    "(%s)",
    "@%d",
    "D=A",
    "(__VM$LOCAL_LOOP.%d)",
    "@__VM$LOCAL_END.%d",
    "D;JEQ",
    "@SP",
    "A=M",
    "M=0",
    "@SP",
    "M=M+1",
    "D=D-1",
    "@__VM$LOCAL_LOOP.%d",
    "0;JMP",
    "(__VM$LOCAL_END.%d)",
    NULL
};

static const char* vmReturn[] = {
    "@LCL",
    "D=M",
    "@R13",
    "M=D",
    "@5",
    "A=D-A",
    "D=M",
    "@R14",
    "M=D",
    "@SP",
    "AM=M-1",
    "D=M",
    "@ARG",
    "A=M",
    "M=D",
    "@ARG",
    "D=M",
    "D=D+1",
    "@SP",
    "M=D",
    "@R13",
    "AM=M-1",
    "D=M",
    "@THAT",
    "M=D",
    "@R13",
    "AM=M-1",
    "D=M",
    "@THIS",
    "M=D",
    "@R13",
    "AM=M-1",
    "D=M",
    "@ARG",
    "M=D",
    "@R13",
    "AM=M-1",
    "D=M",
    "@LCL",
    "M=D",
    "@R14",
    "A=M",
    "0;JMP",
    NULL
};

#endif
