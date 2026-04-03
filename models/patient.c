#include "patient.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Patient* create_patient(int id, const char* name, int age, const char* gender,
                        const char* contact, const char* patient_id, int bed_id,
                        const char* admission_date, const char* discharge_date) {
    Patient* patient = (Patient*)malloc(sizeof(Patient));
    if (patient) {
        patient->id = id;
        strncpy(patient->name, name, MAX_NAME - 1);
        patient->name[MAX_NAME - 1] = '\0';
        patient->age = age;
        strncpy(patient->gender, gender, MAX_GENDER - 1);
        patient->gender[MAX_GENDER - 1] = '\0';
        strncpy(patient->contact, contact, MAX_CONTACT - 1);
        patient->contact[MAX_CONTACT - 1] = '\0';
        strncpy(patient->patient_id, patient_id, MAX_PATIENT_ID - 1);
        patient->patient_id[MAX_PATIENT_ID - 1] = '\0';
        patient->bed_id = bed_id;
        strncpy(patient->admission_date, admission_date, MAX_DATE - 1);
        patient->admission_date[MAX_DATE - 1] = '\0';
        strncpy(patient->discharge_date, discharge_date, MAX_DATE - 1);
        patient->discharge_date[MAX_DATE - 1] = '\0';
        patient->next = NULL;
    }
    return patient;
}

void free_patient_list(Patient* head) {
    Patient* current = head;
    while (current != NULL) {
        Patient* temp = current;
        current = current->next;
        free(temp);
    }
}

void display_patient(const Patient* patient) {
    printf("ID: %d, Name: %s, Age: %d, Gender: %s, Contact: %s\n", 
           patient->id, patient->name, patient->age, 
           patient->gender, patient->contact);
    printf("  PatientID: %s, BedID: %d, Admission: %s", 
           patient->patient_id, patient->bed_id, patient->admission_date);
    if (strlen(patient->discharge_date) > 0) {
        printf(", Discharge: %s", patient->discharge_date);
    }
    printf("\n");
}

Patient* find_patient_by_id(Patient* head, int id) {
    Patient* current = head;
    while (current != NULL) {
        if (current->id == id) return current;
        current = current->next;
    }
    return NULL;
}

int get_patient_count(Patient* head) {
    int count = 0;
    Patient* current = head;
    while (current != NULL) {
        count++;
        current = current->next;
    }
    return count;
}

int get_admitted_count(Patient* head) {
    int count = 0;
    Patient* current = head;
    while (current != NULL) {
        if (current->bed_id != 0 && strlen(current->discharge_date) == 0) {
            count++;
        }
        current = current->next;
    }
    return count;
}