#include <stdio.h>
#include <sqlite3.h>
#include "adsb_lists.h"
#include "adsb_time.h"
#include "adsb_db.h"

/*==============================================
FUNCTION: DB_open
INPUT: a char vector
OUTPUT: a pointer for a sqlite3 structure
DESCRIPTION: this function receives a database name
and tries to open the database with that name. In
success, the function returns the database handler
pointer. Otherwise, it returns NULL.
================================================*/
sqlite3 * DB_open(char *db_name){
    sqlite3 *db_handler = NULL;
    int status = -1;
    
    status = sqlite3_open((const char *)db_name, &db_handler);
    
    if(status == SQLITE_OK){
        printf("Database successfully opened!\n");
        return db_handler;
    }else{
        printf("Database not opened: %s\n", sqlite3_errmsg(db_handler));
        return NULL;
    }
}

/*==============================================
FUNCTION: DB_sabeADSBInfo
INPUT: a char vector
OUTPUT: an integer
DESCRIPTION: this function receives an adsbMsg node
and tries to save its information into the radar_
livre_adsbinfo table. In success, the status returned
contains 0 (SQLITE_OK). Otherwise, contains the value
of the constant that represents the error.
================================================*/
int DB_saveADSBInfo(adsbMsg *msg){
    int status = 0;
    char *sqlText = NULL, *errmsg = NULL;
    sqlite3 *db_handler = NULL;
    
    db_handler = DB_open(DATABASE);

    if(db_handler == NULL){
        return -1;  //database couldn't be opened.
    }

    sqlText = sqlite3_mprintf(
        "INSERT INTO radarlivre_api_adsbinfo(collectorKey, modeSCode, callsign, latitude, longitude," \
        "altitude, verticalVelocity, horizontalVelocity, groundTrackHeading, timestamp, timestampSent," \
        "messageDataId, messageDataPositionEven, messageDataPositionOdd, messageDataVelocity)"\
        " VALUES (\"%q\", \"%q\", \"%q\", %f, %f, %d, %d, %f, %f, %lf, %lf, \"%q\", \"%q\", \"%q\", \"%q\");", 
        msg->COLLECTOR_ID, msg->ICAO, msg->callsign, msg->Latitude, msg->Longitude, msg->Altitude,
        msg->verticalVelocity, msg->horizontalVelocity, msg->groundTrackHeading, msg->oeTimestamp[msg->lastTime], 
        msg->oeTimestamp[msg->lastTime], msg->messageID, msg->oeMSG[0], msg->oeMSG[1], msg->mensagemVEL
    );

    status = sqlite3_exec(db_handler, sqlText, NULL, NULL, &errmsg); //It tries to execute the sql statement
    
    if(status ==  SQLITE_OK){
        printf("Data was saved successfully!\n");
    }else{
        printf("Data couldn't be saved: %s\n", errmsg);
    }

    sqlite3_free(errmsg);
    
    if(sqlite3_close_v2(db_handler) != SQLITE_OK){
        printf("It couldn't close the database!\n");
    }

    return status;
}