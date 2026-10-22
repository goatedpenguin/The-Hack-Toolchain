#include "preprocessor.h"
#include <string.h>
#include <ctype.h>

void stripComment(char* line) {
    if (!line) return;
    char* comment = strstr(line, "//");
    if (comment) *comment = '\0';
}

void stripWhiteSpace(char* line) {
    if (!line) return;
    char* src = line;
    while (*src && isspace((unsigned char)*src))
        src++;
    char* dst = line;
    int spacePending = 0;
    while (*src) {
        if (isspace((unsigned char)*src)) {
            spacePending = 1;
        } else {
            if (spacePending && dst != line) {
                *dst++ = ' ';
            }
            *dst++ = *src;
            spacePending = 0;
        }
        src++;
    }
    if (dst > line && isspace((unsigned char)dst[-1])) dst--;
    *dst = '\0';
}
