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

adsbMsg *messagesList = NULL;

void suddenStop(){

 	printf("\nCtrl+C...\nClosing Colector...\n");

 	LIST_removeAll(&messagesList);
    
    printf("Colector closed!\n");

 	exit(1);
}

int main(){
    int serialPort = 0;
    char buffer[29]; buffer[0] = '\0';
    adsbMsg *node = NULL;

    signal(2, suddenStop);
    serialPort = SERIAL_start();

    while(1){   //Polling method
        if(SERIAL_communicate(&serialPort, buffer) == 10){
            continue;
        }

        messagesList = decodeMessage(buffer, messagesList, &node);

        if(isNodeComplete(node) != NULL){
            if(DB_saveADSBInfo(node) != -1){
                printf("Aircraft %s information saved succesfully!\n", node->ICAO);
            }else{
                printf("The aircraft information couldn't be saved!\n");
            }
        }else{
            printf("Information is not complete!\n");
        }

        node = NULL;
		memset(buffer, 0x0, 29);
    }

    return 0;
}