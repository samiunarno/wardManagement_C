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

// Add this at the beginning of main()
// int verify_license() {
//     // Check for license file
//     FILE* license = fopen("license.key", "r");
//     if (!license) {
//         printf("License not found! Contact support.\n");
//         return 0;
//     }
//     // Verify license key...
//     return 1;
// }

int main() {

    BedManagementSystem* system = create_system();
    User* current_user = NULL;
    int choice;
    
    printf("========================================\n");
    printf("   HOSPITAL BED MANAGEMENT SYSTEM\n");
    printf("========================================\n");
    
    while (1) {
        printf("\n========== MAIN MENU ==========\n");
        printf("1. Login\n");
        printf("2. Register\n");
        printf("3. Exit\n");
        printf("Choice: ");
        scanf("%d", &choice);
        getchar();
        
        if (choice == 1) {
            char username[MAX_USERNAME], password[MAX_PASSWORD];
            printf("\n--- LOGIN ---\n");
            printf("Username: ");
            fgets(username, MAX_USERNAME, stdin);
            username[strcspn(username, "\n")] = '\0';
            
            printf("Password: ");
            fgets(password, MAX_PASSWORD, stdin);
            password[strcspn(password, "\n")] = '\0';
            
            if (authenticate(system, username, password, &current_user)) {
                if (current_user->status != STATUS_APPROVED) {
                    printf("\n[!] Account not approved yet. Please wait for admin approval.\n");
                    continue;
                }
                
                printf("\n[+] Welcome, %s (%s)!\n", current_user->username, 
                       user_role_to_string(current_user->role));
                
                if (current_user->role == ROLE_ADMIN) {
                    admin_menu(system);
                } else if (current_user->role == ROLE_STAFF) {
                    staff_menu(system);
                }
            } else {
                printf("\n[!] Invalid credentials.\n");
            }
        } else if (choice == 2) {
            registration_menu(system);
        } else {
            printf("\n========================================\n");
            printf("   EXITING SYSTEM...\n");
            printf("========================================\n");
            free_system(system);
            printf("Goodbye!\n");
            break;
        }
    }
    
    return 0;
}

void registration_menu(BedManagementSystem* system) {
    char username[MAX_USERNAME], password[MAX_PASSWORD], role[20];
    
    printf("\n--- REGISTRATION ---\n");
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
        printf("\n[+] Registration successful! Awaiting admin approval.\n");
    } else {
        printf("\n[!] Registration failed. Username may already exist.\n");
    }
}

void admin_approval_menu(BedManagementSystem* system) {
    int count = get_pending_users_count(system);
    if (count == 0) {
        printf("\n[+] No pending registrations.\n");
        return;
    }
    
    printf("\n--- PENDING APPROVALS ---\n");
    User* pending = get_pending_users(system);
    User* current = pending;
    
    while (current != NULL) {
        display_user(current);
        char choice;
        printf("Approve this user? (y/n): ");
        scanf(" %c", &choice);
        getchar();
        
        if (choice == 'y' || choice == 'Y') {
            if (approve_user(system, current->username)) {
                printf("[+] User approved.\n");
            } else {
                printf("[!] Failed to approve user.\n");
            }
        } else {
            printf("[-] User rejected.\n");
        }
        current = current->next;
    }
    
    free_user_list(pending);
}

void admin_menu(BedManagementSystem* system) {
    int choice;
    do {
        printf("\n========== ADMIN MENU ==========\n");
        printf("1. Add Ward\n");
        printf("2. Remove Ward\n");
        printf("3. Add Bed\n");
        printf("4. Remove Bed\n");
        printf("5. Admit Patient\n");
        printf("6. Transfer Patient\n");
        printf("7. Discharge Patient\n");
        printf("8. View All Wards\n");
        printf("9. View All Patients\n");
        printf("10. Generate Reports\n");
        printf("11. Export Reports\n");
        printf("12. Approve User Registrations\n");
        printf("13. Logout\n");
        printf("Choice: ");
        scanf("%d", &choice);
        getchar();
        
        switch (choice) {
            case 1: {
                char name[MAX_WARD_NAME], type[MAX_WARD_TYPE];
                int capacity;
                printf("\n--- ADD WARD ---\n");
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
                    printf("[+] Ward added successfully.\n");
                } else {
                    printf("[!] Failed to add ward.\n");
                }
                break;
            }
            case 2: {
                int ward_id;
                printf("\n--- REMOVE WARD ---\n");
                printf("Ward ID: ");
                scanf("%d", &ward_id);
                getchar();
                if (remove_ward(system, ward_id)) {
                    printf("[+] Ward removed successfully.\n");
                } else {
                    printf("[!] Ward not found.\n");
                }
                break;
            }
            case 3: {
                int ward_id, bed_id;
                printf("\n--- ADD BED ---\n");
                printf("Ward ID: ");
                scanf("%d", &ward_id);
                printf("Bed ID: ");
                scanf("%d", &bed_id);
                getchar();
                if (add_bed(system, ward_id, bed_id)) {
                    printf("[+] Bed added successfully.\n");
                } else {
                    printf("[!] Failed to add bed.\n");
                }
                break;
            }
            case 4: {
                int ward_id, bed_id;
                printf("\n--- REMOVE BED ---\n");
                printf("Ward ID: ");
                scanf("%d", &ward_id);
                printf("Bed ID: ");
                scanf("%d", &bed_id);
                getchar();
                if (remove_bed(system, ward_id, bed_id)) {
                    printf("[+] Bed removed successfully.\n");
                } else {
                    printf("[!] Bed not found.\n");
                }
                break;
            }
            case 5: {
                char name[MAX_NAME], gender[MAX_GENDER], contact[MAX_CONTACT], patient_id[MAX_PATIENT_ID];
                int age, ward_id, bed_id;
                char current_date[11];
                get_current_date(current_date, sizeof(current_date));
                
                printf("\n--- ADMIT PATIENT ---\n");
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
                    printf("[+] Patient admitted successfully.\n");
                } else {
                    printf("[!] Admission failed. Bed may be occupied or invalid.\n");
                }
                break;
            }
            case 6: {
                int patient_id, new_ward_id, new_bed_id;
                printf("\n--- TRANSFER PATIENT ---\n");
                printf("Patient ID: ");
                scanf("%d", &patient_id);
                printf("New Ward ID: ");
                scanf("%d", &new_ward_id);
                printf("New Bed ID: ");
                scanf("%d", &new_bed_id);
                getchar();
                if (transfer_patient(system, patient_id, new_ward_id, new_bed_id)) {
                    printf("[+] Patient transferred successfully.\n");
                } else {
                    printf("[!] Transfer failed.\n");
                }
                break;
            }
            case 7: {
                int patient_id;
                printf("\n--- DISCHARGE PATIENT ---\n");
                printf("Patient ID: ");
                scanf("%d", &patient_id);
                getchar();
                if (discharge_patient(system, patient_id)) {
                    printf("[+] Patient discharged successfully.\n");
                } else {
                    printf("[!] Discharge failed. Patient may not be admitted.\n");
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
                printf("\n[+] Logging out...\n");
                return;
            default:
                printf("[!] Invalid choice. Please try again.\n");
        }
    } while (1);
}

void staff_menu(BedManagementSystem* system) {
    int choice;
    do {
        printf("\n========== STAFF MENU ==========\n");
        printf("1. View All Wards\n");
        printf("2. View All Patients\n");
        printf("3. Admit Patient\n");
        printf("4. Discharge Patient\n");
        printf("5. View Reports\n");
        printf("6. Logout\n");
        printf("Choice: ");
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
                
                printf("\n--- ADMIT PATIENT ---\n");
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
                    printf("[+] Patient admitted successfully.\n");
                } else {
                    printf("[!] Admission failed.\n");
                }
                break;
            }
            case 4: {
                int patient_id;
                printf("\n--- DISCHARGE PATIENT ---\n");
                printf("Patient ID: ");
                scanf("%d", &patient_id);
                getchar();
                if (discharge_patient(system, patient_id)) {
                    printf("[+] Patient discharged successfully.\n");
                } else {
                    printf("[!] Discharge failed.\n");
                }
                break;
            }
            case 5:
                generate_and_display_reports(system);
                break;
            case 6:
                printf("\n[+] Logging out...\n");
                return;
            default:
                printf("[!] Invalid choice. Please try again.\n");
        }
    } while (1);
}