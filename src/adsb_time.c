#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>
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

/*==============================================
FUNCTION: TIMER_setSignalHandler
INPUT: an pointer to a function and an integer
OUTPUT: an integer
DESCRIPTION: this function receives a function 
that is responsible for handler a signal, the
signal that needs to be handled and configures
the system for that combination of signal and
handler.
================================================*/
int TIMER_setSignalHandler(void (*handler)(int, siginfo_t*, void*), int timer_signal){
    struct sigaction sa;

    sa.sa_flags = SA_SIGINFO; // | SA_RESTART;
    sa.sa_sigaction = handler;
    sigemptyset(&sa.sa_mask);

    if (sigaction(timer_signal, &sa, NULL) == TIMER_ERROR){
        perror("The handler coudn't be configured!");
        return TIMER_ERROR;
    }

    return TIMER_OK;
}

/*==============================================
FUNCTION: TIMER_create
INPUT: two integers
OUTPUT: a timer_t structure
DESCRIPTION: this function receives a clock id and
a signal, configures how the system should be 
notified when this signal appears and creates a
timer, based on the clock passed in the function,
that generates this signal once the timer expires.
================================================*/
timer_t TIMER_create(int clockid, int timer_signal){
    timer_t timerid;
    struct sigevent sev;
   
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = timer_signal;
    sev.sigev_value.sival_ptr = &timerid;

    if (timer_create(clockid, &sev, &timerid) == TIMER_ERROR){
        perror("The timer couldn't be configured!");
        return (timer_t)(TIMER_ERROR);
    }

    return timerid;
}

/*==============================================
FUNCTION: TIMER_setTimeout
INPUT: a vector of chars and a time_t structure
OUTPUT: an integer
DESCRIPTION: this function receives a timeout, in
nanoseconds, and a timer id and configures the timer
described by the timer id to expires, periodically,
after the nanoseconds passed in the function. The 
nanoseconds are passed as a vector of chars because
we can write larger values.
================================================*/
int TIMER_setTimeout(char *timeout_nanosec, timer_t timerid){
    long long timeout = atoll(timeout_nanosec);
    struct itimerspec its;

    its.it_value.tv_sec = timeout / 1000000000;
    its.it_value.tv_nsec = timeout % 1000000000;
    its.it_interval.tv_sec = its.it_value.tv_sec;
    its.it_interval.tv_nsec = its.it_value.tv_nsec;

    if (timer_settime(timerid, 0, &its, NULL) == TIMER_ERROR){
        perror("Timeout coudn't be setted!");
        return TIMER_ERROR;
    }
    return TIMER_OK;
}