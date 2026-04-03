#include "file_handler.h"
#include <stdio.h>
#include <stdlib.h>

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