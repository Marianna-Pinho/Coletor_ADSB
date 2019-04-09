#ifndef ADSB_DB_H
#define ADSB_DB_H

/*===============================
These functions are responsible
for the database operations.
=================================*/
typedef struct sqlite3 sqlite3;
typedef struct msg adsbMsg;

#define DATABASE "radarlivre_v4.db"
#define DATABASE_ERROR -1

sqlite3 * DB_open(char *db_name);
int DB_saveADSBInfo(adsbMsg *msg);
int DB_saveAirline(adsbMsg *msg);
int DB_saveData(adsbMsg *msg);
void DB_close(sqlite3 **db_handler, char**errmsg, char**sqlText);

#endif