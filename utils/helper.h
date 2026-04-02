#ifndef HELPER_H
#define HELPER_H

#include <time.h>
void trim(char* str);
int is_valid_name(const char* name);
int is_valid_age(int age);
int is_valid_date(const char* date);
void get_current_date(char* buffer, int size);

// File utilities
int file_exists(const char* filename);
void split_string(const char* str, char delimiter, char*** result, int* count);
void free_split_result(char** result, int count);

#endif