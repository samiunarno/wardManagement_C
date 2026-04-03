#include "bed_management.h"
#include "../utils/file_handler.h"
#include "../utils/helper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifdef _WIN32
    #include <direct.h>
    #define mkdir(dir, mode) _mkdir(dir)
#endif

void init_sample_data(BedManagementSystem* sys);

BedManagementSystem* create_system() {
    BedManagementSystem* sys = (BedManagementSystem*)malloc(sizeof(BedManagementSystem));
    if (sys) {
        sys->ward_list = NULL;
        sys->patient_list = NULL;
        sys->user_list = NULL;
        sys->next_ward_id = 1;
        sys->next_bed_id = 1;
        sys->next_patient_id = 1;
        sys->next_user_id = 1;
        
        load_all_data(sys);
        
        
        if (sys->ward_list == NULL || sys->user_list == NULL) {
            printf("[*] No existing data found. Creating sample data...\n");
            init_sample_data(sys);
            save_all_data(sys);
        }
        
 
        User* admin = find_user_by_username(sys->user_list, "admin");
        if (admin == NULL) {
            printf("[*] Admin not found. Creating admin user...\n");
            admin = create_user(sys->next_user_id++, "admin", "admin", ROLE_ADMIN, STATUS_APPROVED);
            admin->next = sys->user_list;
            sys->user_list = admin;
            save_all_data(sys);
        }
    }
    return sys;
}

void free_system(BedManagementSystem* sys) {
    if (!sys) return;
    
    if (sys->ward_list) {
        free_ward_list(sys->ward_list);
        sys->ward_list = NULL;
    }
    
    if (sys->patient_list) {
        free_patient_list(sys->patient_list);
        sys->patient_list = NULL;
    }
    
    if (sys->user_list) {
        free_user_list(sys->user_list);
        sys->user_list = NULL;
    }
    
    free(sys);
    printf("\n=== Memory Cleanup Complete ===\n");
}

void load_all_data(BedManagementSystem* sys) {
   
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
                            
                            if (bed_id >= sys->next_bed_id)
                                sys->next_bed_id = bed_id + 1;
                        }
                        free_split_result(bed_parts, bed_part_count);
                    }
                    free_split_result(beds, bed_count);
                }
                
                ward->next = sys->ward_list;
                sys->ward_list = ward;
                
                if (id >= sys->next_ward_id)
                    sys->next_ward_id = id + 1;
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
                patient->next = sys->patient_list;
                sys->patient_list = patient;
                
                if (id >= sys->next_patient_id)
                    sys->next_patient_id = id + 1;
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
            
            if (part_count >= 5) {
                int id = atoi(parts[0]);
                char* username = parts[1];
                char* password = parts[2];
                UserRole role = user_string_to_role(parts[3]);
                UserStatus status = user_string_to_status(parts[4]);
                User* user = create_user(id, username, password, role, status);
                user->next = sys->user_list;
                sys->user_list = user;
                
                if (id >= sys->next_user_id)
                    sys->next_user_id = id + 1;
                
                printf("[DEBUG] Loaded user: %s (ID: %d, Role: %s, Status: %s)\n", 
                       username, id, user_role_to_string(role), user_status_to_string(status));
            }
            free_split_result(parts, part_count);
        }
        close_file(file);
    }
}

void save_all_data(BedManagementSystem* sys) {
    
    mkdir("data", 0755);
    
    
    FILE* file = open_file("data/wards.txt", "w");
    if (file) {
        Ward* current = sys->ward_list;
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
        Patient* current = sys->patient_list;
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
        User* current = sys->user_list;
        while (current != NULL) {
            fprintf(file, "%d|%s|%s|%s|%s\n",
                    current->id, current->username, current->password,
                    user_role_to_string(current->role),
                    user_status_to_string(current->status));
            current = current->next;
        }
        close_file(file);
        printf("[DEBUG] Saved %d users to file\n", get_pending_users_count(sys));
    }
}

void init_sample_data(BedManagementSystem* sys) {
    printf("[DEBUG] Initializing sample data...\n");
    
    
    add_ward(sys, "General Ward A", "General", 10);
    add_ward(sys, "ICU Ward", "ICU", 5);
    add_ward(sys, "Private Ward", "Private", 8);
    
    
    Ward* ward1 = find_ward(sys, 1);
    if (ward1) {
        for (int i = 1; i <= 10; i++) {
            add_bed(sys, 1, sys->next_bed_id);
            sys->next_bed_id++;
        }
    }
    
    Ward* ward2 = find_ward(sys, 2);
    if (ward2) {
        for (int i = 1; i <= 5; i++) {
            add_bed(sys, 2, sys->next_bed_id);
            sys->next_bed_id++;
        }
    }
    
    Ward* ward3 = find_ward(sys, 3);
    if (ward3) {
        for (int i = 1; i <= 8; i++) {
            add_bed(sys, 3, sys->next_bed_id);
            sys->next_bed_id++;
        }
    }
    
    // Get current date
    char current_date[11];
    get_current_date(current_date, sizeof(current_date));
    
    // Add sample patients
    Patient* p1 = create_patient(sys->next_patient_id++, "John Doe", 45, "Male", 
                                  "1234567890", "P001", 0, current_date, "");
    admit_patient(sys, p1, 1, 1);
    free(p1);
    
    Patient* p2 = create_patient(sys->next_patient_id++, "Jane Smith", 32, "Female",
                                  "1234567891", "P002", 0, current_date, "");
    admit_patient(sys, p2, 1, 2);
    free(p2);
    
    Patient* p3 = create_patient(sys->next_patient_id++, "Bob Johnson", 67, "Male",
                                  "1234567892", "P003", 0, current_date, "");
    admit_patient(sys, p3, 2, 1);
    free(p3);
    
    // Create admin user - MAKE SURE IT'S CREATED PROPERLY
    User* admin = create_user(sys->next_user_id++, "admin", "admin", ROLE_ADMIN, STATUS_APPROVED);
    admin->next = sys->user_list;
    sys->user_list = admin;
    printf("[DEBUG] Created admin user: %s (ID: %d, Role: Admin, Status: Approved)\n", 
           admin->username, admin->id);
    
    // Also create a test staff user for testing
    User* staff = create_user(sys->next_user_id++, "staff", "staff", ROLE_STAFF, STATUS_APPROVED);
    staff->next = sys->user_list;
    sys->user_list = staff;
    printf("[DEBUG] Created staff user: %s (ID: %d, Role: Staff, Status: Approved)\n", 
           staff->username, staff->id);
}

int add_ward(BedManagementSystem* sys, const char* name, const char* type, int capacity) {
    Ward* new_ward = create_ward(sys->next_ward_id++, name, type, capacity);
    new_ward->next = sys->ward_list;
    sys->ward_list = new_ward;
    save_all_data(sys);
    return 1;
}

int remove_ward(BedManagementSystem* sys, int ward_id) {
    Ward* current = sys->ward_list;
    Ward* prev = NULL;
    
    while (current != NULL) {
        if (current->id == ward_id) {
            if (prev == NULL) {
                sys->ward_list = current->next;
            } else {
                prev->next = current->next;
            }
            if (current->bed_list) free_bed_list(current->bed_list);
            free(current);
            save_all_data(sys);
            return 1;
        }
        prev = current;
        current = current->next;
    }
    return 0;
}

Ward* find_ward(BedManagementSystem* sys, int ward_id) {
    return find_ward_by_id(sys->ward_list, ward_id);
}

void display_all_wards(BedManagementSystem* sys) {
    printf("\n=== All Wards ===\n");
    Ward* current = sys->ward_list;
    if (current == NULL) {
        printf("No wards found.\n");
        return;
    }
    while (current != NULL) {
        display_ward(current);
        display_ward_beds(current);
        current = current->next;
    }
}

int add_bed(BedManagementSystem* sys, int ward_id, int bed_id) {
    Ward* ward = find_ward(sys, ward_id);
    if (!ward) return 0;
    int result = add_bed_to_ward(ward, bed_id);
    if (result) save_all_data(sys);
    return result;
}

int remove_bed(BedManagementSystem* sys, int ward_id, int bed_id) {
    Ward* ward = find_ward(sys, ward_id);
    if (!ward) return 0;
    int result = remove_bed_from_ward(ward, bed_id);
    if (result) save_all_data(sys);
    return result;
}

Bed* find_bed(BedManagementSystem* sys, int ward_id, int bed_id) {
    Ward* ward = find_ward(sys, ward_id);
    if (!ward) return NULL;
    return find_bed_in_ward(ward, bed_id);
}

int admit_patient(BedManagementSystem* sys, const Patient* patient, int ward_id, int bed_id) {
    Ward* ward = find_ward(sys, ward_id);
    if (!ward) return 0;
    
    Bed* bed = find_bed_in_ward(ward, bed_id);
    if (!bed || bed->status != BED_AVAILABLE) return 0;
    
    bed->status = BED_OCCUPIED;
    bed->patient_id = patient->id;
    
    Patient* new_patient = create_patient(patient->id, patient->name, patient->age,
                                          patient->gender, patient->contact, patient->patient_id,
                                          bed_id, patient->admission_date, "");
    new_patient->next = sys->patient_list;
    sys->patient_list = new_patient;
    
    save_all_data(sys);
    return 1;
}

int transfer_patient(BedManagementSystem* sys, int patient_id, int new_ward_id, int new_bed_id) {
    Patient* patient = find_patient(sys, patient_id);
    if (!patient) return 0;
    
    int old_bed_id = patient->bed_id;
    Ward* old_ward = NULL;
    Ward* current = sys->ward_list;
    while (current != NULL) {
        if (find_bed_in_ward(current, old_bed_id)) {
            old_ward = current;
            break;
        }
        current = current->next;
    }
    
    Ward* new_ward = find_ward(sys, new_ward_id);
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
    
    new_bed->status = BED_OCCUPIED;
    new_bed->patient_id = patient_id;
    patient->bed_id = new_bed_id;
    
    save_all_data(sys);
    return 1;
}

int discharge_patient(BedManagementSystem* sys, int patient_id) {
    Patient* patient = find_patient(sys, patient_id);
    if (!patient) return 0;
    
    int bed_id = patient->bed_id;
    if (bed_id == 0) return 0;
    
    Ward* current = sys->ward_list;
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
    
    save_all_data(sys);
    return 1;
}

Patient* find_patient(BedManagementSystem* sys, int patient_id) {
    return find_patient_by_id(sys->patient_list, patient_id);
}

void display_all_patients(BedManagementSystem* sys) {
    printf("\n=== All Patients ===\n");
    Patient* current = sys->patient_list;
    if (current == NULL) {
        printf("No patients found.\n");
        return;
    }
    while (current != NULL) {
        display_patient(current);
        current = current->next;
    }
}

void generate_bed_occupancy_report(BedManagementSystem* sys) {
    printf("\n========== BED OCCUPANCY REPORT ==========\n");
    
    Ward* current = sys->ward_list;
    if (current == NULL) {
        printf("No wards available.\n");
        return;
    }
    
    while (current != NULL) {
        printf("\nWard: %s (%s)\n", current->name, current->type);
        printf("  Total Beds: %d\n", current->capacity);
        printf("  Available Beds: %d\n", get_available_beds_count(current));
        printf("  Occupied Beds: %d\n", get_occupied_beds_count(current));
        printf("  Occupancy Rate: %.1f%%\n", 
               (get_occupied_beds_count(current) * 100.0 / current->capacity));
        current = current->next;
    }
    
    int total_beds = get_total_beds(sys->ward_list);
    int available = get_total_available_beds(sys->ward_list);
    int occupied = get_total_occupied_beds(sys->ward_list);
    
    printf("\n========== OVERALL STATISTICS ==========\n");
    printf("Total Beds: %d\n", total_beds);
    if (total_beds > 0) {
        printf("Available Beds: %d (%.1f%%)\n", available, 
               (available * 100.0 / total_beds));
        printf("Occupied Beds: %d (%.1f%%)\n", occupied,
               (occupied * 100.0 / total_beds));
    }
}

void generate_ward_statistics_report(BedManagementSystem* sys) {
    printf("\n========== WARD STATISTICS REPORT ==========\n");
    Ward* current = sys->ward_list;
    if (current == NULL) {
        printf("No wards available.\n");
        return;
    }
    
    while (current != NULL) {
        printf("\nWard ID: %d\n", current->id);
        printf("Name: %s\n", current->name);
        printf("Type: %s\n", current->type);
        printf("Capacity: %d\n", current->capacity);
        if (current->capacity > 0) {
            printf("Available Beds: %d\n", get_available_beds_count(current));
            printf("Occupied Beds: %d\n", get_occupied_beds_count(current));
            printf("Occupancy Rate: %.1f%%\n", 
                   (get_occupied_beds_count(current) * 100.0 / current->capacity));
        }
        current = current->next;
    }
}

void generate_patient_report(BedManagementSystem* sys) {
    printf("\n========== PATIENT REPORT ==========\n");
    int total = get_patient_count(sys->patient_list);
    int admitted = get_admitted_count(sys->patient_list);
    
    printf("Total Patients: %d\n", total);
    printf("Currently Admitted: %d\n", admitted);
    printf("Discharged: %d\n\n", total - admitted);
    
    printf("Current Inpatients:\n");
    Patient* current = sys->patient_list;
    int found = 0;
    while (current != NULL) {
        if (current->bed_id != 0 && strlen(current->discharge_date) == 0) {
            display_patient(current);
            found = 1;
        }
        current = current->next;
    }
    if (!found) {
        printf("  No inpatients currently.\n");
    }
}

int register_user(BedManagementSystem* sys, const char* username, const char* password, const char* role_str) {
    if (find_user_by_username(sys->user_list, username) != NULL) return 0;
    
    UserRole role = user_string_to_role(role_str);
    User* new_user = create_user(sys->next_user_id++, username, password, role, STATUS_PENDING);
    new_user->next = sys->user_list;
    sys->user_list = new_user;
    save_all_data(sys);
    return 1;
}

int approve_user(BedManagementSystem* sys, const char* username) {
    User* user = find_user_by_username(sys->user_list, username);
    if (!user) return 0;
    user->status = STATUS_APPROVED;
    save_all_data(sys);
    return 1;
}

int authenticate(BedManagementSystem* sys, const char* username, const char* password, User** user) {
    User* found = find_user_by_username(sys->user_list, username);
    if (found) {
        printf("[DEBUG] Found user: %s, Password: %s, Stored: %s\n", 
               username, password, found->password);
        if (strcmp(found->password, password) == 0) {
            *user = found;
            return 1;
        }
    }
    return 0;
}

User* get_pending_users(BedManagementSystem* sys) {
    User* pending_head = NULL;
    User* current = sys->user_list;
    
    while (current != NULL) {
        if (current->status == STATUS_PENDING) {
            User* copy = create_user(current->id, current->username, current->password, 
                                     current->role, current->status);
            copy->next = pending_head;
            pending_head = copy;
        }
        current = current->next;
    }
    return pending_head;
}

int get_pending_users_count(BedManagementSystem* sys) {
    int count = 0;
    User* current = sys->user_list;
    while (current != NULL) {
        if (current->status == STATUS_PENDING) count++;
        current = current->next;
    }
    return count;
}