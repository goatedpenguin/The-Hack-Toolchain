#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <stdio.h>

typedef struct VMFile {
    FILE* fp;
    char* fileName;
} VMFile;

typedef struct VMInputSet {
    VMFile* fpArray;
    int numFiles;
} VMInputSet;

VMInputSet* manageFiles(const char* name);
void freeInputSet(VMInputSet* set);

#endif
