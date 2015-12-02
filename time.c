/* hien thi thoi gian hien tai */
#include <stdio.h>
#include <string.h>
#include <time.h>

int main ()
{
  time_t rawtime;
  struct tm * timeinfo;
  char currenttime[30];

  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
  strcpy(currenttime,asctime (timeinfo));
  printf ( "Current local time and date: %s", currenttime );

  return 0;
}