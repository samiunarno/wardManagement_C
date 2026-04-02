#include "helper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

int is_valid_name(const char* name) {
    if (!name || strlen(name) == 0 || strlen(name) > 50) return 0;
    
    for (int i = 0; name[i]; i++) {
        if (!isalpha(name[i]) && name[i] != ' ' && name[i] != '.') return 0;
    }
    return 1;
}

int is_valid_age(int age) {
    return age >= 0 && age <= 120;
}

int is_valid_date(const char* date) {
    if (!date || strlen(date) != 10) return 0;
    if (date[4] != '-' || date[7] != '-') return 0;
    
    int year = atoi(date);
    int month = atoi(date + 5);
    int day = atoi(date + 8);
    
    if (year < 1900 || year > 2100) return 0;
    if (month < 1 || month > 12) return 0;
    if (day < 1 || day > 31) return 0;
    
    return 1;
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
    
    token = strtok(str_copy, &delimiter);
    while (token != NULL) {
        tokens = realloc(tokens, (token_count + 1) * sizeof(char*));
        tokens[token_count] = strdup(token);
        token_count++;
        token = strtok(NULL, &delimiter);
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