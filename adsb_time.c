#include <stdio.h>
#include <time.h>
#include "adsb_time.h"

/*==============================================
FUNCTION: getCurrentTime
INPUT: void
OUTPUT: a double value
DESCRIPTION: this function gets the current time
and returns it in seconds.
================================================*/
double getCurrentTime(){
  time_t timer;
  struct tm time_aux;
  double seconds;

  time_aux.tm_hour = 0;   time_aux.tm_min = 0; time_aux.tm_sec = 0;  time_aux.tm_wday = 6; time_aux.tm_isdst = 0;
  time_aux.tm_year = 100; time_aux.tm_mon = 0; time_aux.tm_mday = 1; time_aux.tm_yday = 0;

  time(&timer);

  seconds = difftime(timer,mktime(&time_aux));

  return seconds;
}

/*==============================================
FUNCTION: getFormatedTime
INPUT: void
OUTPUT: a char vector pointer
DESCRIPTION: this function gets the current time
and returns it in the Www Mmm dd hh:mm:ss yyyy
format, where "w" represents the week days and "m"
represents the month names.
================================================*/
char* getFormatedTime(){
  time_t timer = 0;

  time(&timer);
  return asctime(localtime(&timer));

}