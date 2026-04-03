#ifndef BED_MANAGEMENT_H
#define BED_MANAGEMENT_H

#include "../models/ward.h"
#include "../models/patient.h"
#include "../models/user.h"

typedef struct {
    Ward* ward_list;
    Patient* patient_list;
    User* user_list;
    int next_ward_id;
    int next_bed_id;
    int next_patient_id;
    int next_user_id;
} BedManagementSystem;

BedManagementSystem* create_system();
void free_system(BedManagementSystem* system);
void load_all_data(BedManagementSystem* system);
void save_all_data(BedManagementSystem* system);

int add_ward(BedManagementSystem* system, const char* name, const char* type, int capacity);
int remove_ward(BedManagementSystem* system, int ward_id);
Ward* find_ward(BedManagementSystem* system, int ward_id);
void display_all_wards(BedManagementSystem* system);

int add_bed(BedManagementSystem* system, int ward_id, int bed_id);
int remove_bed(BedManagementSystem* system, int ward_id, int bed_id);
Bed* find_bed(BedManagementSystem* system, int ward_id, int bed_id);

int admit_patient(BedManagementSystem* system, const Patient* patient, int ward_id, int bed_id);
int transfer_patient(BedManagementSystem* system, int patient_id, int new_ward_id, int new_bed_id);
int discharge_patient(BedManagementSystem* system, int patient_id);
Patient* find_patient(BedManagementSystem* system, int patient_id);
void display_all_patients(BedManagementSystem* system);

void generate_bed_occupancy_report(BedManagementSystem* system);
void generate_ward_statistics_report(BedManagementSystem* system);
void generate_patient_report(BedManagementSystem* system);

int register_user(BedManagementSystem* system, const char* username, const char* password, const char* role);
int approve_user(BedManagementSystem* system, const char* username);
int authenticate(BedManagementSystem* system, const char* username, const char* password, User** user);
User* get_pending_users(BedManagementSystem* system);
int get_pending_users_count(BedManagementSystem* system);

#endif