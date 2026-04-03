#ifndef HELPER_H
#define HELPER_H

void trim(char* str);
void get_current_date(char* buffer, int size);
void split_string(const char* str, char delimiter, char*** result, int* count);
void free_split_result(char** result, int count);

#endif