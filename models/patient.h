#ifndef PATIENT_H
#define PATIENT_H

#define MAX_NAME 100
#define MAX_GENDER 10
#define MAX_CONTACT 20
#define MAX_PATIENT_ID 20
#define MAX_DATE 11

typedef struct Patient {
    int id;
    char name[MAX_NAME];
    int age;
    char gender[MAX_GENDER];
    char contact[MAX_CONTACT];
    char patient_id[MAX_PATIENT_ID];
    int bed_id;
    char admission_date[MAX_DATE];
    char discharge_date[MAX_DATE];
    struct Patient* next;
} Patient;

Patient* create_patient(int id, const char* name, int age, const char* gender,
                        const char* contact, const char* patient_id, int bed_id,
                        const char* admission_date, const char* discharge_date);
void free_patient_list(Patient* head);
void display_patient(const Patient* patient);
Patient* find_patient_by_id(Patient* head, int id);
int get_patient_count(Patient* head);
int get_admitted_count(Patient* head);

#endif