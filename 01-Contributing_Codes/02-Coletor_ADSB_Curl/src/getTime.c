#include <stdio.h>
#include <time.h>
#include "getTime.h"

double getCurrentTime(){
  time_t timer;
  struct tm time_aux;
  double seconds;

  time_aux.tm_hour = 0;   time_aux.tm_min = 0; time_aux.tm_sec = 0;  time_aux.tm_wday = 6; time_aux.tm_isdst = 0;
  time_aux.tm_year = 100; time_aux.tm_mon = 0; time_aux.tm_mday = 1; time_aux.tm_yday = 0;

  time(&timer);  /* get current time; same as: timer = time(NULL)  */

  seconds = difftime(timer,mktime(&time_aux));

  printf ("TIME: %.f\n", seconds);

  return seconds;
}


/*void main(){
	printf ("%.f seconds since January 1, 2000 in the current timezone\n", getCurrentTime());	
}*/