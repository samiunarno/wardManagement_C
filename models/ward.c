#include "ward.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Ward* create_ward(int id, const char* name, const char* type, int capacity) {
    Ward* ward = (Ward*)malloc(sizeof(Ward));
    if (ward) {
        ward->id = id;
        strncpy(ward->name, name, MAX_WARD_NAME - 1);
        ward->name[MAX_WARD_NAME - 1] = '\0';
        strncpy(ward->type, type, MAX_WARD_TYPE - 1);
        ward->type[MAX_WARD_TYPE - 1] = '\0';
        ward->capacity = capacity;
        ward->bed_list = NULL;
        ward->next = NULL;
    }
    return ward;
}

void free_ward_list(Ward* head) {
    Ward* current = head;
    while (current != NULL) {
        Ward* temp = current;
        if (current->bed_list) {
            free_bed_list(current->bed_list);
        }
        current = current->next;
        free(temp);
    }
}

int add_bed_to_ward(Ward* ward, int bed_id) {
    if (!ward) return 0;
    
    // Check if bed already exists
    if (find_bed_in_ward(ward, bed_id)) return 0;
    
    Bed* new_bed = create_bed(bed_id, ward->id, BED_AVAILABLE, 0);
    new_bed->next = ward->bed_list;
    ward->bed_list = new_bed;
    return 1;
}

int remove_bed_from_ward(Ward* ward, int bed_id) {
    if (!ward || !ward->bed_list) return 0;
    
    Bed* current = ward->bed_list;
    Bed* prev = NULL;
    
    while (current != NULL) {
        if (current->id == bed_id) {
            if (prev == NULL) {
                ward->bed_list = current->next;
            } else {
                prev->next = current->next;
            }
            free(current);
            return 1;
        }
        prev = current;
        current = current->next;
    }
    return 0;
}

Bed* find_bed_in_ward(Ward* ward, int bed_id) {
    if (!ward) return NULL;
    
    Bed* current = ward->bed_list;
    while (current != NULL) {
        if (current->id == bed_id) return current;
        current = current->next;
    }
    return NULL;
}

int get_available_beds_count(Ward* ward) {
    if (!ward) return 0;
    
    int count = 0;
    Bed* current = ward->bed_list;
    while (current != NULL) {
        if (current->status == BED_AVAILABLE) count++;
        current = current->next;
    }
    return count;
}

int get_occupied_beds_count(Ward* ward) {
    if (!ward) return 0;
    
    int count = 0;
    Bed* current = ward->bed_list;
    while (current != NULL) {
        if (current->status == BED_OCCUPIED) count++;
        current = current->next;
    }
    return count;
}

void display_ward(const Ward* ward) {
    printf("WardID: %d, Name: %s, Type: %s, Capacity: %d, Available: %d, Occupied: %d\n",
           ward->id, ward->name, ward->type, ward->capacity,
           get_available_beds_count((Ward*)ward), 
           get_occupied_beds_count((Ward*)ward));
}

void display_ward_beds(const Ward* ward) {
    Bed* current = ward->bed_list;
    while (current != NULL) {
        display_bed(current);
        current = current->next;
    }
}

Ward* find_ward_by_id(Ward* head, int ward_id) {
    Ward* current = head;
    while (current != NULL) {
        if (current->id == ward_id) return current;
        current = current->next;
    }
    return NULL;
}