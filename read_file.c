// C program for the above approach
#include <stdio.h>
#include <string.h>
#include "read_file.h"
#include <stdbool.h>

void process_adw_string(char *buff, struct adw_s *adw)
{
 
        // Here we have taken size of
        // array 1024 you can modify it
	char *buffer = malloc(sizeof(char)*10000); 
        int column = 0;
	int i = 0; 
 
	strcpy(buffer, buff);
            // Splitting the data
            char* value = strtok(buffer, ", ");
	   
           	    
	
	    	
	    
            while (value) {
    		if (column%13 == 0 && column>0) {
                    i++;
                }
                // Column 1
                if (column%13 == 0) {
                    adw[0].SEG = strtobool(value);

                }
 
                // Column 2
                if (column%13 == 1) {
                    adw[0].USE_EXTENSION = strtobool(value);
                }
 
                // Column 3
                if (column%13 == 2) {
		    adw[0].SEG_INTERRUPT = strtobool(value);
                }

		if (column%13 == 3) {
			adw[0].IGNORE_ADW = strtobool(value);
		}

		if(column%13 == 4){
			adw[0].M3 = strtobool(value);
		}


		if(column%13 == 5){
			adw[0].M2 = strtobool(value);
		}
		
		if(column%13 == 6){
			adw[0].M1 = strtobool(value);
		}

		if(column%13 == 7){
			adw[0].FREQ_OFFSET = atoi(value);
		}

		if(column%13 == 8){
			adw[0].LEVEL_OFFSET = atoi(value); 
		}
		if (column%13 == 9) {
			adw[0].PHASE_OFFSET = atoi(value);
		}
		if (column%13 == 10) {
			adw[0].SEGMENT_IDX = atoi(value);
		}
		if (column%13 == 11) {
			adw[0].BURST_SRI = atoi(value);	
		}
		if (column%13 == 12) {
			adw[0].BURST_ADD_SEGMENTS = atoi(value);
		}
		
 
                value = strtok(NULL, ", ");
                column++;
	    }
 
}


bool strtobool(const char *str) {
    if (strcasecmp(str, "true") == 0 || strcasecmp(str, "1") == 0) {
        return true;
    } else if (strcasecmp(str, "false") == 0 || strcasecmp(str, "0") == 0) {
        return false;
    } else {
        // Handle invalid input, you can choose to return a default value or raise an error
        fprintf(stderr, "Invalid input: %s\n", str);
        return false; // Or any default value you prefer
    }
}


