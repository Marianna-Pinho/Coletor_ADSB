#include <stdio.h>
#include <string.h>
#include "adsb_time.h"
#include "adsb_createLog.h"

/*==============================================
FUNCTION: LOG_add
INPUT: two char pointers
OUTPUT: an integer
DESCRIPTION: this function receives the source of
a log and the content to be reported and save these
information, along with the current time and date 
information, at the end of a log file. Source 
indicates who (which function) rised up a log.
Content describes why the log is being generated.
================================================*/
int LOG_add(char *source, char *content){

    FILE *lg = fopen(LOG_FILE, "a+");

    if(lg == NULL){
        printf("The log file couldn't be opened!\n");
        return -1;
    }

    char *timeString = getFormatedTime();
    timeString[strlen(timeString)-1] = '\0';

    fprintf(lg, "%s | %s | %s\n", timeString, source, content);
    fclose(lg);

    return 0;
}
