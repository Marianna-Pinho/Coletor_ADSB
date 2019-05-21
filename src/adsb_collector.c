#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include "adsb_auxiliars.h"
#include "adsb_lists.h"
#include "adsb_time.h"
#include "adsb_decoding.h"
#include "adsb_serial.h"
#include "adsb_db.h"
#include "adsb_createLog.h"

adsbMsg *messagesList = NULL;

volatile int timer_flag = 0;

static void timerHandler(int sig, siginfo_t *si, void *uc){
    timer_flag = 1;
}

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
    //Starting Serial
    serialPort = SERIAL_start();

    //Starting Timer
    TIMER_setSignalHandler(timerHandler, SIG);
    timer_t timerid = TIMER_create(CLOCKID, SIG);
    TIMER_setTimeout(TIMEOUT, timerid);

    while(1){   //Polling method
    
        SERIAL_communicate(&serialPort, buffer);

        //If CRC returns 1, the message is correct. Otherwise, we don't do anything with the message.
        if(CRC_verifyMsg(buffer)){
            
            messagesList = decodeMessage(buffer, messagesList, &node);

            if(isNodeComplete(node) != NULL){
                if(DB_saveData(node) != 0){
                    printf("The aircraft information couldn't be saved!\n");
                }else{
                    printf("Aircraft %s information saved succesfully!\n", node->ICAO);
                    clearMinimalInfo(node);
                }
            }else{
                printf("Information is not complete!\n");
            }
        }

        node = NULL;
		memset(buffer, 0x0, 29);

        // //It cleans the old nodes in the messages list
        if(timer_flag){
            messagesList = LIST_delOldNodes(messagesList);
            timer_flag = 0;
        }

    }
    return 0;
}