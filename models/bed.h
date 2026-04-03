#ifndef BED_H
#define BED_H

typedef enum {
    BED_AVAILABLE,
    BED_OCCUPIED,
    BED_MAINTENANCE
} BedStatus;

typedef struct Bed {
    int id;
    int ward_id;
    BedStatus status;
    int patient_id;
    struct Bed* next;
} Bed;

Bed* create_bed(int id, int ward_id, BedStatus status, int patient_id);
void free_bed_list(Bed* head);
const char* bed_status_to_string(BedStatus status);
BedStatus bed_string_to_status(const char* str);
void display_bed(const Bed* bed);
Bed* find_bed_by_id(Bed* head, int bed_id);
int count_beds_by_status(Bed* head, BedStatus status);

#endif