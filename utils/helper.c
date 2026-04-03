#include "helper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

void trim(char* str) {
    if (!str) return;
    
    char* start = str;
    char* end;
    
    while (isspace((unsigned char)*start)) start++;
    
    if (*start == 0) {
        str[0] = '\0';
        return;
    }
    
    end = start + strlen(start) - 1;
    while (end > start && isspace((unsigned char)*end)) end--;
    
    end[1] = '\0';
    
    if (start != str) {
        memmove(str, start, (end - start + 2));
    }
}

void get_current_date(char* buffer, int size) {
    time_t t = time(NULL);
    struct tm* tm = localtime(&t);
    snprintf(buffer, size, "%04d-%02d-%02d", 
             tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday);
}

void split_string(const char* str, char delimiter, char*** result, int* count) {
    if (!str) {
        *result = NULL;
        *count = 0;
        return;
    }
    
    char* str_copy = strdup(str);
    char* token;
    char** tokens = NULL;
    int token_count = 0;
    char delim[2] = {delimiter, '\0'};
    
    token = strtok(str_copy, delim);
    while (token != NULL) {
        tokens = realloc(tokens, (token_count + 1) * sizeof(char*));
        tokens[token_count] = strdup(token);
        token_count++;
        token = strtok(NULL, delim);
    }
    
    free(str_copy);
    *result = tokens;
    *count = token_count;
}

void free_split_result(char** result, int count) {
    if (!result) return;
    for (int i = 0; i < count; i++) {
        free(result[i]);
    }
    free(result);
}