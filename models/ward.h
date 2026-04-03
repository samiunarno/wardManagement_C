#ifndef WARD_H
#define WARD_H

#include "bed.h"

#define MAX_WARD_NAME 50
#define MAX_WARD_TYPE 20

typedef struct Ward {
    int id;
    char name[MAX_WARD_NAME];
    char type[MAX_WARD_TYPE];
    int capacity;
    Bed* bed_list;
    struct Ward* next;
} Ward;

Ward* create_ward(int id, const char* name, const char* type, int capacity);
void free_ward_list(Ward* head);
int add_bed_to_ward(Ward* ward, int bed_id);
int remove_bed_from_ward(Ward* ward, int bed_id);
Bed* find_bed_in_ward(Ward* ward, int bed_id);
int get_available_beds_count(Ward* ward);
int get_occupied_beds_count(Ward* ward);
void display_ward(const Ward* ward);
void display_ward_beds(const Ward* ward);
Ward* find_ward_by_id(Ward* head, int ward_id);
int get_total_beds(Ward* head);
int get_total_available_beds(Ward* head);
int get_total_occupied_beds(Ward* head);

#endif