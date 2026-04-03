#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include <stdio.h>

FILE* open_file(const char* filename, const char* mode);
void close_file(FILE* file);
int read_line(FILE* file, char* buffer, int size);
int write_line(FILE* file, const char* line);

#endif