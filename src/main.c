#define _XOPEN_SOURCE 700
#include "InputManager.h"
#include "codegen.h"
#include "parser.h"
#include "preprocessor.h"
#include "table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

static int translateFile(VMFile* file, FILE* program, table* hashTable) {
    char* line = NULL;
    size_t len = 0;
    int lineNum = 0;
    int ok = 1;
    setCodegenFile(file->fileName);
    while (getline(&line, &len, file->fp) != -1) {
        lineNum++;
        stripComment(line);
        stripWhiteSpace(line);
        if (!line[0]) continue;
        VMPacket* packet = buildVMPacket(line, hashTable);
        if (!packet) {
            fprintf(stderr, "%s.vm:%d: invalid command: %s\n", file->fileName, lineNum, line);
            ok = 0;
            break;
        }
        char* asmStr = codeGen(packet, file->fileName);
        freeVMPacket(packet);
        if (!asmStr || fputs(asmStr, program) == EOF) ok = 0;
        free(asmStr);
        if (!ok) break;
    }
    if (ferror(file->fp)) {
        fprintf(stderr, "could not read %s.vm\n", file->fileName);
        ok = 0;
    }
    free(line);
    setCodegenFile(NULL);
    return ok;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s <file.vm|directory>\n", argv[0]);
        return 1;
    }
    char* inputPath = realpath(argv[1], NULL);
    if (!inputPath) {
        perror(argv[1]);
        return 1;
    }
    VMInputSet* set = manageFiles(inputPath);
    if (!set) {
        free(inputPath);
        return 1;
    }
    struct stat st;
    if (stat(inputPath, &st) != 0) {
        perror(inputPath);
        freeInputSet(set);
        free(inputPath);
        return 1;
    }
    int directory = S_ISDIR(st.st_mode);
    char* outputPath;
    if (directory) {
        const char* base = strrchr(inputPath, '/') + 1;
        outputPath = malloc(strlen(inputPath) + strlen(base) + 6);
        sprintf(outputPath, "%s/%s.asm", inputPath, base);
    } else {
        outputPath = malloc(strlen(inputPath) + 2);
        strcpy(outputPath, inputPath);
        strcpy(outputPath + strlen(outputPath) - 3, ".asm");
    }
    FILE* program = fopen(outputPath, "w");
    if (!program) {
        perror(outputPath);
        free(outputPath);
        freeInputSet(set);
        free(inputPath);
        return 1;
    }

    table* hashTable = initTable();
    buildVMTable(hashTable);
    int ok = 1;
    if (directory) {
        char* init = bootstrapCode();
        if (!init || fputs(init, program) == EOF) ok = 0;
        free(init);
    }
    for (int i = 0; ok && i < set->numFiles; i++)
        ok = translateFile(&set->fpArray[i], program, hashTable);
    if (fclose(program) != 0) {
        perror(outputPath);
        ok = 0;
    }
    if (!ok) remove(outputPath);
    freeTable(hashTable);
    freeInputSet(set);
    free(outputPath);
    free(inputPath);
    return ok ? 0 : 1;
}
