#include <stdio.h>
#include <sqlite3.h>
#include "adsb_lists.h"
#include "adsb_time.h"
#include "adsb_db.h"
#include "adsb_createLog.h"

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
        LOG_add("DB_open", "database couldn't be opened!");

        return NULL;
    }
}

/*==============================================
FUNCTION: DB_saveADSBInfo
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
        LOG_add("DB_saveADSBInfo", "Data was saved successfully");
    }else{
        printf("Data couldn't be saved: %s\n", errmsg);
        LOG_add("DB_saveADSBInfo", "Data couldn't be saved");
    }

    sqlite3_free(errmsg);
    
    if(sqlite3_close_v2(db_handler) != SQLITE_OK){
        printf("It couldn't close the database!\n");
        LOG_add("DB_saveADSBInfo", "Database couldn't be closed");
    }else{
        printf("Database successfully closed!\n");
    }

    return status;
}

/*==============================================
FUNCTION: DB_saveAirline
INPUT: a char vector
OUTPUT: an integer
DESCRIPTION: this function receives an adsbMsg node
and tries to save its information into the radar_
livre_airline table. In success, the status returned
contains 0 (SQLITE_OK). Otherwise, contains the value
of the constant that represents the error.
================================================*/
int DB_saveAirline(adsbMsg *msg){
    int status = 0;
    char *sqlText = NULL, *errmsg = NULL;
    sqlite3 *db_handler = NULL;

    db_handler = DB_open(DATABASE);

    if(db_handler == NULL){
        return -1;  //database couldn't be opened.
    }

    sqlText = sqlite3_mprintf(
        "INSERT INTO radarlivre_api_airline(icao, callsign) VALUES (\"%q\", \"%q\");", 
        msg->ICAO, msg->callsign
    );

    status = sqlite3_exec(db_handler, sqlText, NULL, NULL, &errmsg); //It tries to execute the sql statement

    if(status ==  SQLITE_OK){
        printf("Data was saved successfully!\n");
        LOG_add("DB_saveAirline", "Data was saved successfully");
    }else{
        printf("Data couldn't be saved: %s\n", errmsg);
        LOG_add("DB_saveAirline", "Data couldn't be saved");
    }

     sqlite3_free(errmsg);

    if(sqlite3_close_v2(db_handler) != SQLITE_OK){
        printf("It couldn't close the database!\n");
        LOG_add("DB_saveAirline", "Database couldn't be closed");
    }else{
        printf("Database successfully closed!\n");
    }

    return status;
}

/*==============================================
FUNCTION: DB_saveData
INPUT: a char vector
OUTPUT: an integer
DESCRIPTION: this function receives an adsbMsg node
and tries to save its information into the radar_
livre_adsbinfo and radarlivre_api_airline tables.
In success, the status returned contains 0 (SQLITE_OK).
Otherwise, contains the value of the constant that
represents the error.
================================================*/
int DB_saveData(adsbMsg *msg){
    int status1 = 0, status2 = 0;

    status1 = DB_saveADSBInfo(msg);
    status2 = DB_saveAirline(msg);

    if(status1 != 0){
        return status1;
    }
    if(status2 != 0){
        return status2;
    }

    return status1; //It's because status1 and status2 are equal to zero
}