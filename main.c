#include "services/bed_management.h"
#include "services/report_service.h"
#include "utils/helper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void admin_menu(BedManagementSystem* system);
void staff_menu(BedManagementSystem* system);
void registration_menu(BedManagementSystem* system);
void admin_approval_menu(BedManagementSystem* system);

int main() {
    BedManagementSystem* system = create_system();
    User* current_user = NULL;
    int choice;
    
    printf("=== Hospital Bed Management System ===\n");
    
    while (1) {
        printf("\n1. Login\n2. Register\n3. Exit\nChoice: ");
        scanf("%d", &choice);
        getchar(); 
        
        if (choice == 1) {
            char username[MAX_USERNAME], password[MAX_PASSWORD];
            printf("Username: ");
            fgets(username, MAX_USERNAME, stdin);
            username[strcspn(username, "\n")] = '\0';
            
            printf("Password: ");
            fgets(password, MAX_PASSWORD, stdin);
            password[strcspn(password, "\n")] = '\0';
            
            if (authenticate(system, username, password, &current_user)) {
                if (current_user->status != STATUS_APPROVED) {
                    printf("Account not approved yet. Please wait for admin approval.\n");
                    continue;
                }
                
                printf("Welcome, %s (%s)!\n", current_user->username, 
                       user_role_to_string(current_user->role));
                
                if (current_user->role == ROLE_ADMIN) {
                    admin_menu(system);
                } else if (current_user->role == ROLE_STAFF) {
                    staff_menu(system);
                }
            } else {
                printf("Invalid credentials.\n");
            }
        } else if (choice == 2) {
            registration_menu(system);
        } else {
            printf("\n=== Exiting System ===\n");
            free_system(system);
            printf("Memory cleanup completed. Goodbye!\n");
            break;
        }
    }
    
    return 0;
}

void registration_menu(BedManagementSystem* system) {
    char username[MAX_USERNAME], password[MAX_PASSWORD], role[20];
    
    printf("\n--- Registration ---\n");
    printf("Username: ");
    fgets(username, MAX_USERNAME, stdin);
    username[strcspn(username, "\n")] = '\0';
    
    printf("Password: ");
    fgets(password, MAX_PASSWORD, stdin);
    password[strcspn(password, "\n")] = '\0';
    
    printf("Role (Admin/Staff): ");
    fgets(role, sizeof(role), stdin);
    role[strcspn(role, "\n")] = '\0';
    
    if (register_user(system, username, password, role)) {
        printf("Registration successful! Awaiting admin approval.\n");
    } else {
        printf("Registration failed. Username may already exist.\n");
    }
}

void admin_approval_menu(BedManagementSystem* system) {
    int count = get_pending_users_count(system);
    if (count == 0) {
        printf("No pending registrations.\n");
        return;
    }
    
    printf("\n--- Pending Approvals ---\n");
    User* pending = get_pending_users(system);
    User* current = pending;
    
    while (current != NULL) {
        display_user(current);
        char choice;
        printf("Approve (y/n)? ");
        scanf(" %c", &choice);
        getchar();
        
        if (choice == 'y' || choice == 'Y') {
            approve_user(system, current->username);
            printf("User approved.\n");
        } else {
            printf("User rejected.\n");
        }
        current = current->next;
    }
    
    free_user_list(pending);
}

void admin_menu(BedManagementSystem* system) {
    int choice;
    do {
        printf("\n--- Admin Menu ---\n");
        printf("1. Add Ward\n2. Remove Ward\n3. Add Bed\n4. Remove Bed\n");
        printf("5. Admit Patient\n6. Transfer Patient\n7. Discharge Patient\n");
        printf("8. View All Wards\n9. View All Patients\n");
        printf("10. Generate Reports\n11. Export Reports to File\n");
        printf("12. Approve User Registrations\n13. Exit (with cleanup)\nChoice: ");
        scanf("%d", &choice);
        getchar();
        
        switch (choice) {
            case 1: {
                char name[MAX_WARD_NAME], type[MAX_WARD_TYPE];
                int capacity;
                printf("Ward Name: ");
                fgets(name, MAX_WARD_NAME, stdin);
                name[strcspn(name, "\n")] = '\0';
                printf("Ward Type (General/ICU/Private): ");
                fgets(type, MAX_WARD_TYPE, stdin);
                type[strcspn(type, "\n")] = '\0';
                printf("Capacity: ");
                scanf("%d", &capacity);
                getchar();
                
                if (add_ward(system, name, type, capacity)) {
                    printf("Ward added successfully.\n");
                } else {
                    printf("Failed to add ward.\n");
                }
                break;
            }
            case 2: {
                int ward_id;
                printf("Ward ID: ");
                scanf("%d", &ward_id);
                getchar();
                if (remove_ward(system, ward_id)) {
                    printf("Ward removed successfully.\n");
                } else {
                    printf("Ward not found.\n");
                }
                break;
            }
            case 3: {
                int ward_id, bed_id;
                printf("Ward ID: ");
                scanf("%d", &ward_id);
                printf("Bed ID: ");
                scanf("%d", &bed_id);
                getchar();
                if (add_bed(system, ward_id, bed_id)) {
                    printf("Bed added successfully.\n");
                } else {
                    printf("Failed to add bed.\n");
                }
                break;
            }
            case 4: {
                int ward_id, bed_id;
                printf("Ward ID: ");
                scanf("%d", &ward_id);
                printf("Bed ID: ");
                scanf("%d", &bed_id);
                getchar();
                if (remove_bed(system, ward_id, bed_id)) {
                    printf("Bed removed successfully.\n");
                } else {
                    printf("Bed not found.\n");
                }
                break;
            }
            case 5: {
                char name[MAX_NAME], gender[MAX_GENDER], contact[MAX_CONTACT], patient_id[MAX_PATIENT_ID];
                int age, ward_id, bed_id;
                char current_date[11];
                get_current_date(current_date, sizeof(current_date));
                
                printf("Patient Name: ");
                fgets(name, MAX_NAME, stdin);
                name[strcspn(name, "\n")] = '\0';
                printf("Age: ");
                scanf("%d", &age);
                getchar();
                printf("Gender: ");
                fgets(gender, MAX_GENDER, stdin);
                gender[strcspn(gender, "\n")] = '\0';
                printf("Contact: ");
                fgets(contact, MAX_CONTACT, stdin);
                contact[strcspn(contact, "\n")] = '\0';
                printf("Patient ID: ");
                fgets(patient_id, MAX_PATIENT_ID, stdin);
                patient_id[strcspn(patient_id, "\n")] = '\0';
                printf("Ward ID: ");
                scanf("%d", &ward_id);
                printf("Bed ID: ");
                scanf("%d", &bed_id);
                getchar();
                
                Patient new_patient;
                new_patient.id = system->next_patient_id;
                strcpy(new_patient.name, name);
                new_patient.age = age;
                strcpy(new_patient.gender, gender);
                strcpy(new_patient.contact, contact);
                strcpy(new_patient.patient_id, patient_id);
                new_patient.bed_id = 0;
                strcpy(new_patient.admission_date, current_date);
                new_patient.discharge_date[0] = '\0';
                
                if (admit_patient(system, &new_patient, ward_id, bed_id)) {
                    system->next_patient_id++;
                    printf("Patient admitted successfully.\n");
                } else {
                    printf("Admission failed. Bed may be occupied.\n");
                }
                break;
            }
            case 6: {
                int patient_id, new_ward_id, new_bed_id;
                printf("Patient ID: ");
                scanf("%d", &patient_id);
                printf("New Ward ID: ");
                scanf("%d", &new_ward_id);
                printf("New Bed ID: ");
                scanf("%d", &new_bed_id);
                getchar();
                if (transfer_patient(system, patient_id, new_ward_id, new_bed_id)) {
                    printf("Patient transferred successfully.\n");
                } else {
                    printf("Transfer failed.\n");
                }
                break;
            }
            case 7: {
                int patient_id;
                printf("Patient ID: ");
                scanf("%d", &patient_id);
                getchar();
                if (discharge_patient(system, patient_id)) {
                    printf("Patient discharged successfully.\n");
                } else {
                    printf("Discharge failed.\n");
                }
                break;
            }
            case 8:
                display_all_wards(system);
                break;
            case 9:
                display_all_patients(system);
                break;
            case 10:
                generate_and_display_reports(system);
                break;
            case 11:
                export_all_reports(system);
                break;
            case 12:
                admin_approval_menu(system);
                break;
            case 13:
                printf("\n=== Exiting Admin Menu ===\n");
                save_all_data(system);
                return;
            default:
                printf("Invalid choice.\n");
        }
    } while (1);
}

void staff_menu(BedManagementSystem* system) {
    int choice;
    do {
        printf("\n--- Staff Menu ---\n");
        printf("1. View All Wards\n2. View All Patients\n3. Admit Patient\n");
        printf("4. Discharge Patient\n5. View Reports\n6. Exit\nChoice: ");
        scanf("%d", &choice);
        getchar();
        
        switch (choice) {
            case 1:
                display_all_wards(system);
                break;
            case 2:
                display_all_patients(system);
                break;
            case 3: {
                char name[MAX_NAME], gender[MAX_GENDER], contact[MAX_CONTACT], patient_id[MAX_PATIENT_ID];
                int age, ward_id, bed_id;
                char current_date[11];
                get_current_date(current_date, sizeof(current_date));
                
                printf("Patient Name: ");
                fgets(name, MAX_NAME, stdin);
                name[strcspn(name, "\n")] = '\0';
                printf("Age: ");
                scanf("%d", &age);
                getchar();
                printf("Gender: ");
                fgets(gender, MAX_GENDER, stdin);
                gender[strcspn(gender, "\n")] = '\0';
                printf("Contact: ");
                fgets(contact, MAX_CONTACT, stdin);
                contact[strcspn(contact, "\n")] = '\0';
                printf("Patient ID: ");
                fgets(patient_id, MAX_PATIENT_ID, stdin);
                patient_id[strcspn(patient_id, "\n")] = '\0';
                printf("Ward ID: ");
                scanf("%d", &ward_id);
                printf("Bed ID: ");
                scanf("%d", &bed_id);
                getchar();
                
                Patient new_patient;
                new_patient.id = system->next_patient_id;
                strcpy(new_patient.name, name);
                new_patient.age = age;
                strcpy(new_patient.gender, gender);
                strcpy(new_patient.contact, contact);
                strcpy(new_patient.patient_id, patient_id);
                new_patient.bed_id = 0;
                strcpy(new_patient.admission_date, current_date);
                new_patient.discharge_date[0] = '\0';
                
                if (admit_patient(system, &new_patient, ward_id, bed_id)) {
                    system->next_patient_id++;
                    printf("Patient admitted successfully.\n");
                } else {
                    printf("Admission failed.\n");
                }
                break;
            }
            case 4: {
                int patient_id;
                printf("Patient ID: ");
                scanf("%d", &patient_id);
                getchar();
                if (discharge_patient(system, patient_id)) {
                    printf("Patient discharged successfully.\n");
                } else {
                    printf("Discharge failed.\n");
                }
                break;
            }
            case 5:
                generate_and_display_reports(system);
                break;
            case 6:
                return;
            default:
                printf("Invalid choice.\n");
        }
    } while (1);
}