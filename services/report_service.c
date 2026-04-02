#include "report_service.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void generate_and_display_reports(BedManagementSystem* system) {
    generate_bed_occupancy_report(system);
    generate_ward_statistics_report(system);
    generate_patient_report(system);
}

void export_all_reports(BedManagementSystem* system) {
    char* content = (char*)malloc(10000);
    if (!content) return;
    

    FILE* file = fopen("full_report.txt", "w");
    if (file) {
        
        FILE* old_stdout = stdout;
        
        
        stdout = file;
        
        
        generate_bed_occupancy_report(system);
        generate_ward_statistics_report(system);
        generate_patient_report(system);
        
        
        stdout = old_stdout;
        fclose(file);
        
        printf("Full report exported to full_report.txt\n");
    }
    
    free(content);
}