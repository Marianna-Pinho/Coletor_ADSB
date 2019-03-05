#ifndef ADSB_CREATELOG_H
#define ADSB_CREATELOG_H

/*===============================
These functions are responsible
for log operations.
=================================*/

#define LOG_FILE    "adsb_log.log"

int LOG_add(char *source, char *content);

#endif