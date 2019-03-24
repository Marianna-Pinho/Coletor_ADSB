#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "adsb_auxiliars.h"
#include "adsb_lists.h"
#include "adsb_time.h"
#include "adsb_decoding.h"
#include "adsb_serial.h"
#include "adsb_db.h"
#include "adsb_createLog.h"

adsbMsg *messagesList = NULL;

void suddenStop(){

 	printf("\nCtrl+C...\nClosing Collector...\n");

 	LIST_removeAll(&messagesList);
 
    printf("Collector closed!\n");
    LOG_add("suddenStop", "Collector was closed");

 	exit(1);
}

int main(){
    int serialPort = 0;
    char buffer[29]; buffer[0] = '\0';
    adsbMsg *node = NULL;

    signal(2, suddenStop);
    serialPort = SERIAL_start();

    while(1){   //Polling method
    
        SERIAL_communicate(&serialPort, buffer);
        messagesList = decodeMessage(buffer, messagesList, &node);

        if(isNodeComplete(node) != NULL){
            if(DB_saveData(node) != 0){
                printf("The aircraft information couldn't be saved!\n");
            }else{
                printf("Aircraft %s information saved succesfully!\n", node->ICAO);
            }
        }else{
            printf("Information is not complete!\n");
        }

        node = NULL;
		memset(buffer, 0x0, 29);
    }
    return 0;
}