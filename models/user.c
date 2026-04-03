#include "user.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

User* create_user(int id, const char* username, const char* password, UserRole role, UserStatus status) {
    User* user = (User*)malloc(sizeof(User));
    if (user) {
        user->id = id;
        strncpy(user->username, username, MAX_USERNAME - 1);
        user->username[MAX_USERNAME - 1] = '\0';
        strncpy(user->password, password, MAX_PASSWORD - 1);
        user->password[MAX_PASSWORD - 1] = '\0';
        user->role = role;
        user->status = status;
        user->next = NULL;
    }
    return user;
}

void free_user_list(User* head) {
    User* current = head;
    while (current != NULL) {
        User* temp = current;
        current = current->next;
        free(temp);
    }
}

void display_user(const User* user) {
    printf("ID: %d, Username: %s, Role: %s, Status: %s\n", 
           user->id, user->username, 
           user_role_to_string(user->role),
           user_status_to_string(user->status));
}

const char* user_role_to_string(UserRole role) {
    switch(role) {
        case ROLE_ADMIN: return "Admin";
        case ROLE_STAFF: return "Staff";
        default: return "Unknown";
    }
}

UserRole user_string_to_role(const char* str) {
    if (strcmp(str, "Admin") == 0) return ROLE_ADMIN;
    if (strcmp(str, "Staff") == 0) return ROLE_STAFF;
    return ROLE_STAFF;
}

const char* user_status_to_string(UserStatus status) {
    switch(status) {
        case STATUS_PENDING: return "Pending";
        case STATUS_APPROVED: return "Approved";
        case STATUS_REJECTED: return "Rejected";
        default: return "Unknown";
    }
}

UserStatus user_string_to_status(const char* str) {
    if (strcmp(str, "Approved") == 0) return STATUS_APPROVED;
    if (strcmp(str, "Rejected") == 0) return STATUS_REJECTED;
    return STATUS_PENDING;
}

User* find_user_by_username(User* head, const char* username) {
    User* current = head;
    while (current != NULL) {
        if (strcmp(current->username, username) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}