#include <time.h>
#include <stdio.h>

int main(void)
{
  int failures = 0;

  struct tm timeinfo;
  time_t rawtime;
  struct tm *p_timeinfo;

  timeinfo.tm_year  = 2020 - 1900;
  timeinfo.tm_mon   = 12 - 1;
  timeinfo.tm_mday  = 24;
  timeinfo.tm_hour  = 10;
  timeinfo.tm_min   = 30;
  timeinfo.tm_sec   = 50;
  timeinfo.tm_isdst = 0;

  rawtime = mktime(&timeinfo);

  failures += !(rawtime == 1608805850);

  p_timeinfo = localtime(&rawtime);

  failures += !(p_timeinfo->tm_year == timeinfo.tm_year);
  failures += !(p_timeinfo->tm_mon  == timeinfo.tm_mon);
  failures += !(p_timeinfo->tm_mday == timeinfo.tm_mday);
  failures += !(p_timeinfo->tm_hour == timeinfo.tm_hour);
  failures += !(p_timeinfo->tm_min  == timeinfo.tm_min);
  failures += !(p_timeinfo->tm_sec  == timeinfo.tm_sec);

  printf("%lu\n%s%d\n", rawtime, asctime(p_timeinfo), failures);

  return failures;
}
