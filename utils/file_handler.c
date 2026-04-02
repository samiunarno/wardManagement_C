#include "file_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE* open_file(const char* filename, const char* mode) {
    return fopen(filename, mode);
}

void close_file(FILE* file) {
    if (file) fclose(file);
}

int read_line(FILE* file, char* buffer, int size) {
    if (!file || !buffer) return 0;
    return fgets(buffer, size, file) != NULL;
}

int write_line(FILE* file, const char* line) {
    if (!file || !line) return 0;
    return fprintf(file, "%s\n", line) >= 0;
}

int file_exists(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file) {
        fclose(file);
        return 1;
    }
    return 0;
}