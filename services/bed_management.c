#include "../services/report_service.h"
#include "../utils/file_handler.h"
#include "../utils/helper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void init_sample_data(BedManagementSystem* system);

BedManagementSystem* create_system() {
    BedManagementSystem* system = (BedManagementSystem*)malloc(sizeof(BedManagementSystem));
    if (system) {
        system->ward_list = NULL;
        system->patient_list = NULL;
        system->user_list = NULL;
        system->next_ward_id = 1;
        system->next_bed_id = 1;
        system->next_patient_id = 1;
        
        load_all_data(system);
        

        if (system->ward_list == NULL || system->patient_list == NULL || system->user_list == NULL) {
            init_sample_data(system);
            save_all_data(system);
        }
    }
    return system;
}

void free_system(BedManagementSystem* system) {
    if (!system) return;
    
    if (system->ward_list) {
        free_ward_list(system->ward_list);
        system->ward_list = NULL;
    }
    
    if (system->patient_list) {
        free_patient_list(system->patient_list);
        system->patient_list = NULL;
    }
    
    if (system->user_list) {
        free_user_list(system->user_list);
        system->user_list = NULL;
    }
    
    free(system);
    printf("\n=== Memory Cleanup Complete ===\n");
    printf("All wards, beds, patients, and user data have been freed.\n");
}
void load_all_data(BedManagementSystem* system) {
    
    FILE* file = open_file("data/wards.txt", "r");
    if (file) {
        char line[1024];
        while (read_line(file, line, sizeof(line))) {
            char** parts;
            int part_count;
            split_string(line, '|', &parts, &part_count);
            
            if (part_count >= 4) {
                int id = atoi(parts[0]);
                Ward* ward = create_ward(id, parts[1], parts[2], atoi(parts[3]));
                if (part_count > 4 && strlen(parts[4]) > 0) {
                    char** beds;
                    int bed_count;
                    split_string(parts[4], ',', &beds, &bed_count);
                    
                    for (int i = 0; i < bed_count; i++) {
                        char** bed_parts;
                        int bed_part_count;
                        split_string(beds[i], ':', &bed_parts, &bed_part_count);
                        
                        if (bed_part_count >= 3) {
                            int bed_id = atoi(bed_parts[0]);
                            BedStatus status = bed_string_to_status(bed_parts[1]);
                            int patient_id = atoi(bed_parts[2]);
                            Bed* bed = create_bed(bed_id, id, status, patient_id);
                            bed->next = ward->bed_list;
                            ward->bed_list = bed;
                            
                            if (bed_id >= system->next_bed_id)
                                system->next_bed_id = bed_id + 1;
                        }
                        free_split_result(bed_parts, bed_part_count);
                    }
                    free_split_result(beds, bed_count);
                }
                ward->next = system->ward_list;
                system->ward_list = ward;
                
                if (id >= system->next_ward_id)
                    system->next_ward_id = id + 1;
            }
            free_split_result(parts, part_count);
        }
        close_file(file);
    }
    file = open_file("data/patients.txt", "r");
    if (file) {
        char line[1024];
        while (read_line(file, line, sizeof(line))) {
            char** parts;
            int part_count;
            split_string(line, '|', &parts, &part_count);
            
            if (part_count >= 8) {
                int id = atoi(parts[0]);
                int bed_id = atoi(parts[6]);
                const char* discharge = (part_count > 8) ? parts[8] : "";
                
                Patient* patient = create_patient(id, parts[1], atoi(parts[2]), parts[3],
                                                  parts[4], parts[5], bed_id, parts[7], discharge);
                patient->next = system->patient_list;
                system->patient_list = patient;
                
                if (id >= system->next_patient_id)
                    system->next_patient_id = id + 1;
            }
            free_split_result(parts, part_count);
        }
        close_file(file);
    }
    file = open_file("data/users.txt", "r");
    if (file) {
        char line[1024];
        while (read_line(file, line, sizeof(line))) {
            char** parts;
            int part_count;
            split_string(line, '|', &parts, &part_count);
            
            if (part_count >= 4) {
                UserRole role = user_string_to_role(parts[2]);
                UserStatus status = user_string_to_status(parts[3]);
                User* user = create_user(parts[0], parts[1], role, status);
                user->next = system->user_list;
                system->user_list = user;
            }
            free_split_result(parts, part_count);
        }
        close_file(file);
    }
    if (system->user_list == NULL) {
        User* admin = create_user("admin", "admin", ROLE_ADMIN, STATUS_APPROVED);
        admin->next = system->user_list;
        system->user_list = admin;
    }
}

void save_all_data(BedManagementSystem* system) {
    FILE* file = open_file("data/wards.txt", "w");
    if (file) {
        Ward* current = system->ward_list;
        while (current != NULL) {
            char bed_str[4096] = "";
            Bed* bed_current = current->bed_list;
            while (bed_current != NULL) {
                char temp[256];
                snprintf(temp, sizeof(temp), "%d:%s:%d", 
                         bed_current->id, 
                         bed_status_to_string(bed_current->status),
                         bed_current->patient_id);
                if (strlen(bed_str) > 0) strcat(bed_str, ",");
                strcat(bed_str, temp);
                bed_current = bed_current->next;
            }
            
            fprintf(file, "%d|%s|%s|%d|%s\n", 
                    current->id, current->name, current->type, 
                    current->capacity, bed_str);
            current = current->next;
        }
        close_file(file);
    }
    file = open_file("data/patients.txt", "w");
    if (file) {
        Patient* current = system->patient_list;
        while (current != NULL) {
            fprintf(file, "%d|%s|%d|%s|%s|%s|%d|%s|%s\n",
                    current->id, current->name, current->age,
                    current->gender, current->contact, current->patient_id,
                    current->bed_id, current->admission_date, current->discharge_date);
            current = current->next;
        }
        close_file(file);
    }
    file = open_file("data/users.txt", "w");
    if (file) {
        User* current = system->user_list;
        while (current != NULL) {
            fprintf(file, "%s|%s|%s|%s\n",
                    current->username, current->password,
                    user_role_to_string(current->role),
                    user_status_to_string(current->status));
            current = current->next;
        }
        close_file(file);
    }
}
void init_sample_data(BedManagementSystem* system) {
    // Create 3 wards
    add_ward(system, "General Ward A", "General", 10);
    add_ward(system, "ICU Ward", "ICU", 5);
    add_ward(system, "Private Ward", "Private", 8);
    Ward* ward1 = find_ward(system, 1);
    if (ward1) {
        for (int i = 1; i <= 10; i++) {
            add_bed(system, 1, system->next_bed_id);
            system->next_bed_id++;
        }
    }
    
    Ward* ward2 = find_ward(system, 2);
    if (ward2) {
        for (int i = 1; i <= 5; i++) {
            add_bed(system, 2, system->next_bed_id);
            system->next_bed_id++;
        }
    }
    
    Ward* ward3 = find_ward(system, 3);
    if (ward3) {
        for (int i = 1; i <= 8; i++) {
            add_bed(system, 3, system->next_bed_id);
            system->next_bed_id++;
        }
    }
    char current_date[11];
    get_current_date(current_date, sizeof(current_date));
    
    Patient* p1 = create_patient(system->next_patient_id++, "John Doe", 45, "Male", 
                                  "1234567890", "P001", 1, current_date, "");
    admit_patient(system, p1, 1, 1);
    free(p1);
    
    Patient* p2 = create_patient(system->next_patient_id++, "Jane Smith", 32, "Female",
                                  "1234567891", "P002", 2, current_date, "");
    admit_patient(system, p2, 1, 2);
    free(p2);
    
    Patient* p3 = create_patient(system->next_patient_id++, "Bob Johnson", 67, "Male",
                                  "1234567892", "P003", 3, current_date, "");
    admit_patient(system, p3, 2, 1);
    free(p3);
}
int add_ward(BedManagementSystem* system, const char* name, const char* type, int capacity) {
    Ward* new_ward = create_ward(system->next_ward_id++, name, type, capacity);
    new_ward->next = system->ward_list;
    system->ward_list = new_ward;
    return 1;
}

int remove_ward(BedManagementSystem* system, int ward_id) {
    Ward* current = system->ward_list;
    Ward* prev = NULL;
    
    while (current != NULL) {
        if (current->id == ward_id) {
            if (prev == NULL) {
                system->ward_list = current->next;
            } else {
                prev->next = current->next;
            }
            if (current->bed_list) free_bed_list(current->bed_list);
            free(current);
            return 1;
        }
        prev = current;
        current = current->next;
    }
    return 0;
}

Ward* find_ward(BedManagementSystem* system, int ward_id) {
    return find_ward_by_id(system->ward_list, ward_id);
}

void display_all_wards(BedManagementSystem* system) {
    printf("\n=== All Wards ===\n");
    Ward* current = system->ward_list;
    while (current != NULL) {
        display_ward(current);
        display_ward_beds(current);
        current = current->next;
    }
}
int add_bed(BedManagementSystem* system, int ward_id, int bed_id) {
    Ward* ward = find_ward(system, ward_id);
    if (!ward) return 0;
    return add_bed_to_ward(ward, bed_id);
}

int remove_bed(BedManagementSystem* system, int ward_id, int bed_id) {
    Ward* ward = find_ward(system, ward_id);
    if (!ward) return 0;
    return remove_bed_from_ward(ward, bed_id);
}

Bed* find_bed(BedManagementSystem* system, int ward_id, int bed_id) {
    Ward* ward = find_ward(system, ward_id);
    if (!ward) return NULL;
    return find_bed_in_ward(ward, bed_id);
}
int admit_patient(BedManagementSystem* system, const Patient* patient, int ward_id, int bed_id) {
    Ward* ward = find_ward(system, ward_id);
    if (!ward) return 0;
    
    Bed* bed = find_bed_in_ward(ward, bed_id);
    if (!bed || bed->status != BED_AVAILABLE) return 0;
    
    
    bed->status = BED_OCCUPIED;
    bed->patient_id = patient->id;
    
    
    Patient* new_patient = create_patient(patient->id, patient->name, patient->age,
                                          patient->gender, patient->contact, patient->patient_id,
                                          bed_id, patient->admission_date, "");
    new_patient->next = system->patient_list;
    system->patient_list = new_patient;
    
    return 1;
}

int transfer_patient(BedManagementSystem* system, int patient_id, int new_ward_id, int new_bed_id) {
    Patient* patient = find_patient(system, patient_id);
    if (!patient) return 0;
    
    int old_bed_id = patient->bed_id;
    Ward* old_ward = NULL;
    Ward* current = system->ward_list;
    while (current != NULL) {
        if (find_bed_in_ward(current, old_bed_id)) {
            old_ward = current;
            break;
        }
        current = current->next;
    }
    
    Ward* new_ward = find_ward(system, new_ward_id);
    if (!new_ward) return 0;
    
    Bed* new_bed = find_bed_in_ward(new_ward, new_bed_id);
    if (!new_bed || new_bed->status != BED_AVAILABLE) return 0;
    
    if (old_ward) {
        Bed* old_bed = find_bed_in_ward(old_ward, old_bed_id);
        if (old_bed) {
            old_bed->status = BED_AVAILABLE;
            old_bed->patient_id = 0;
        }
    }
    
    // Assign new bed
    new_bed->status = BED_OCCUPIED;
    new_bed->patient_id = patient_id;
    patient->bed_id = new_bed_id;
    
    return 1;
}

int discharge_patient(BedManagementSystem* system, int patient_id) {
    Patient* patient = find_patient(system, patient_id);
    if (!patient) return 0;
    
    int bed_id = patient->bed_id;
    Ward* current = system->ward_list;
    while (current != NULL) {
        Bed* bed = find_bed_in_ward(current, bed_id);
        if (bed) {
            bed->status = BED_AVAILABLE;
            bed->patient_id = 0;
            break;
        }
        current = current->next;
    }
    
    char current_date[11];
    get_current_date(current_date, sizeof(current_date));
    strcpy(patient->discharge_date, current_date);
    patient->bed_id = 0;
    
    return 1;
}

Patient* find_patient(BedManagementSystem* system, int patient_id) {
    return find_patient_by_id(system->patient_list, patient_id);
}

void display_all_patients(BedManagementSystem* system) {
    printf("\n=== All Patients ===\n");
    Patient* current = system->patient_list;
    while (current != NULL) {
        display_patient(current);
        current = current->next;
    }
}
void generate_bed_occupancy_report(BedManagementSystem* system) {
    printf("\n=== Bed Occupancy Report ===\n");
    int total_beds = 0, available_beds = 0, occupied_beds = 0, maintenance_beds = 0;
    
    Ward* current = system->ward_list;
    while (current != NULL) {
        printf("\nWard: %s (%s)\n", current->name, current->type);
        printf("  Total Beds: %d\n", current->capacity);
        printf("  Available: %d\n", get_available_beds_count(current));
        printf("  Occupied: %d\n", get_occupied_beds_count(current));
        
        total_beds += current->capacity;
        available_beds += get_available_beds_count(current);
        occupied_beds += get_occupied_beds_count(current);
        
        current = current->next;
    }
    
    maintenance_beds = total_beds - available_beds - occupied_beds;
    
    printf("\n=== Overall Statistics ===\n");
    printf("Total Beds: %d\n", total_beds);
    printf("Available Beds: %d (%.1f%%)\n", available_beds, 
           (available_beds * 100.0 / total_beds));
    printf("Occupied Beds: %d (%.1f%%)\n", occupied_beds,
           (occupied_beds * 100.0 / total_beds));
    printf("Maintenance Beds: %d (%.1f%%)\n", maintenance_beds,
           (maintenance_beds * 100.0 / total_beds));
}

void generate_ward_statistics_report(BedManagementSystem* system) {
    printf("\n=== Ward Statistics Report ===\n");
    Ward* current = system->ward_list;
    while (current != NULL) {
        printf("\nWard ID: %d\n", current->id);
        printf("Name: %s\n", current->name);
        printf("Type: %s\n", current->type);
        printf("Capacity: %d\n", current->capacity);
        printf("Occupancy Rate: %.1f%%\n", 
               (get_occupied_beds_count(current) * 100.0 / current->capacity));
        printf("Available Beds: %d\n", get_available_beds_count(current));
        current = current->next;
    }
}

void generate_patient_report(BedManagementSystem* system) {
    printf("\n=== Patient Report ===\n");
    int total = 0, admitted = 0;
    Patient* current = system->patient_list;
    while (current != NULL) {
        total++;
        if (current->bed_id != 0 && strlen(current->discharge_date) == 0) admitted++;
        current = current->next;
    }
    
    printf("Total Patients: %d\n", total);
    printf("Currently Admitted: %d\n", admitted);
    printf("Discharged: %d\n\n", total - admitted);
    
    printf("Current Inpatients:\n");
    current = system->patient_list;
    while (current != NULL) {
        if (current->bed_id != 0 && strlen(current->discharge_date) == 0) {
            display_patient(current);
        }
        current = current->next;
    }
}

void export_report_to_file(const char* filename, const char* content) {
    FILE* file = fopen(filename, "w");
    if (file) {
        fprintf(file, "%s", content);
        fclose(file);
        printf("Report exported to %s\n", filename);
    } else {
        printf("Failed to export report.\n");
    }
}


int register_user(BedManagementSystem* system, const char* username, const char* password, const char* role_str) {
    // Check if username exists
    User* current = system->user_list;
    while (current != NULL) {
        if (strcmp(current->username, username) == 0) return 0;
        current = current->next;
    }
    
    UserRole role = user_string_to_role(role_str);
    User* new_user = create_user(username, password, role, STATUS_PENDING);
    new_user->next = system->user_list;
    system->user_list = new_user;
    
    return 1;
}

int approve_user(BedManagementSystem* system, const char* username) {
    User* current = system->user_list;
    while (current != NULL) {
        if (strcmp(current->username, username) == 0) {
            current->status = STATUS_APPROVED;
            return 1;
        }
        current = current->next;
    }
    return 0;
}

int authenticate(BedManagementSystem* system, const char* username, const char* password, User** user) {
    User* current = system->user_list;
    while (current != NULL) {
        if (strcmp(current->username, username) == 0 && 
            strcmp(current->password, password) == 0) {
            *user = current;
            return 1;
        }
        current = current->next;
    }
    return 0;
}

User* get_pending_users(BedManagementSystem* system) {
    User* pending_head = NULL;
    User* current = system->user_list;
    
    while (current != NULL) {
        if (current->status == STATUS_PENDING) {
            User* copy = create_user(current->username, current->password, 
                                     current->role, current->status);
            copy->next = pending_head;
            pending_head = copy;
        }
        current = current->next;
    }
    return pending_head;
}

int get_pending_users_count(BedManagementSystem* system) {
    int count = 0;
    User* current = system->user_list;
    while (current != NULL) {
        if (current->status == STATUS_PENDING) count++;
        current = current->next;
    }
    return count;
}