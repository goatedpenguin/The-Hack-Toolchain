#define _POSIX_C_SOURCE 200809L
#include "InputManager.h"
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

static int hasVMExtension(const char* name) {
    size_t len = strlen(name);
    return len > 3 && strcmp(name + len - 3, ".vm") == 0;
}

void freeInputSet(VMInputSet* set) {
    if (!set) return;
    for (int i = 0; i < set->numFiles; i++) {
        fclose(set->fpArray[i].fp);
        free(set->fpArray[i].fileName);
    }
    free(set->fpArray);
    free(set);
}

static int openVMFile(VMInputSet* set, const char* path, const char* name) {
    FILE* fp = fopen(path, "r");
    if (!fp) {
        perror(path);
        return 0;
    }
    set->fpArray = realloc(set->fpArray, (set->numFiles + 1) * sizeof(VMFile));
    VMFile* file = &set->fpArray[set->numFiles++];
    file->fp = fp;
    file->fileName = strdup(name);
    file->fileName[strlen(name) - 3] = '\0';
    return 1;
}

static int compareFiles(const void* a, const void* b) {
    const VMFile* left = a;
    const VMFile* right = b;
    return strcmp(left->fileName, right->fileName);
}

VMInputSet* manageFiles(const char* path) {
    struct stat st;
    if (stat(path, &st) != 0) {
        perror(path);
        return NULL;
    }
    VMInputSet* set = calloc(1, sizeof(VMInputSet));
    if (S_ISREG(st.st_mode)) {
        const char* base = strrchr(path, '/');
        base = base ? base + 1 : path;
        if (!hasVMExtension(base)) {
            fprintf(stderr, "expected a .vm file: %s\n", path);
            freeInputSet(set);
            return NULL;
        }
        if (!openVMFile(set, path, base)) {
            freeInputSet(set);
            return NULL;
        }
        return set;
    }
    if (!S_ISDIR(st.st_mode)) {
        fprintf(stderr, "expected a file or directory: %s\n", path);
        freeInputSet(set);
        return NULL;
    }

    DIR* dir = opendir(path);
    if (!dir) {
        perror(path);
        freeInputSet(set);
        return NULL;
    }
    struct dirent* d;
    while ((d = readdir(dir)) != NULL) {
        if (!hasVMExtension(d->d_name)) continue;
        char* filePath = malloc(strlen(path) + strlen(d->d_name) + 2);
        sprintf(filePath, "%s/%s", path, d->d_name);
        if (stat(filePath, &st) != 0) {
            perror(filePath);
            free(filePath);
            closedir(dir);
            freeInputSet(set);
            return NULL;
        }
        if (S_ISREG(st.st_mode) && !openVMFile(set, filePath, d->d_name)) {
            free(filePath);
            closedir(dir);
            freeInputSet(set);
            return NULL;
        }
        free(filePath);
    }
    closedir(dir);
    if (!set->numFiles) {
        fprintf(stderr, "no .vm files in %s\n", path);
        freeInputSet(set);
        return NULL;
    }
    qsort(set->fpArray, set->numFiles, sizeof(VMFile), compareFiles);
    return set;
}
