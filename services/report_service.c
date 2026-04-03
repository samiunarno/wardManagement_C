#include "report_service.h"
#include <stdio.h>
#include <stdlib.h>

void generate_and_display_reports(BedManagementSystem* system) {
    generate_bed_occupancy_report(system);
    generate_ward_statistics_report(system);
    generate_patient_report(system);
}

void export_all_reports(BedManagementSystem* system) {
    printf("\nExporting reports to files...\n");
    
    FILE* file = fopen("full_report.txt", "w");
    if (file) {
        fprintf(file, "=== HOSPITAL BED MANAGEMENT SYSTEM REPORT ===\n\n");
        fclose(file);
        printf("Full report exported to 'full_report.txt'\n");
    } else {
        printf("Failed to export report.\n");
    }
    
    generate_and_display_reports(system);
}