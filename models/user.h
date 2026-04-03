#ifndef USER_H
#define USER_H

#define MAX_USERNAME 50
#define MAX_PASSWORD 50

typedef enum {
    ROLE_ADMIN,
    ROLE_STAFF
} UserRole;

typedef enum {
    STATUS_PENDING,
    STATUS_APPROVED,
    STATUS_REJECTED
} UserStatus;

typedef struct User {
    int id;
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];
    UserRole role;
    UserStatus status;
    struct User* next;
} User;

User* create_user(int id, const char* username, const char* password, UserRole role, UserStatus status);
void free_user_list(User* head);
void display_user(const User* user);
const char* user_role_to_string(UserRole role);
UserRole user_string_to_role(const char* str);
const char* user_status_to_string(UserStatus status);
UserStatus user_string_to_status(const char* str);
User* find_user_by_username(User* head, const char* username);

#endif