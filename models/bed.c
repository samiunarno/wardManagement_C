#include "bed.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Bed* create_bed(int id, int ward_id, BedStatus status, int patient_id) {
    Bed* bed = (Bed*)malloc(sizeof(Bed));
    if (bed) {
        bed->id = id;
        bed->ward_id = ward_id;
        bed->status = status;
        bed->patient_id = patient_id;
        bed->next = NULL;
    }
    return bed;
}

void free_bed_list(Bed* head) {
    Bed* current = head;
    while (current != NULL) {
        Bed* temp = current;
        current = current->next;
        free(temp);
    }
}

const char* bed_status_to_string(BedStatus status) {
    switch(status) {
        case BED_AVAILABLE: return "Available";
        case BED_OCCUPIED: return "Occupied";
        case BED_MAINTENANCE: return "Maintenance";
        default: return "Unknown";
    }
}

BedStatus bed_string_to_status(const char* str) {
    if (strcmp(str, "Occupied") == 0) return BED_OCCUPIED;
    if (strcmp(str, "Maintenance") == 0) return BED_MAINTENANCE;
    return BED_AVAILABLE;
}

void display_bed(const Bed* bed) {
    printf("  BedID: %d, Status: %s, PatientID: %d\n",
           bed->id, bed_status_to_string(bed->status), bed->patient_id);
}

Bed* find_bed_by_id(Bed* head, int bed_id) {
    Bed* current = head;
    while (current != NULL) {
        if (current->id == bed_id) return current;
        current = current->next;
    }
    return NULL;
}

int count_beds_by_status(Bed* head, BedStatus status) {
    int count = 0;
    Bed* current = head;
    while (current != NULL) {
        if (current->status == status) count++;
        current = current->next;
    }
    return count;
}