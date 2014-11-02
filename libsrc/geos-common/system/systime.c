/*
** systime.c
**
** Maciej 'YTM/Elysium' Witkowiak, 22.11.2002
*/

#include <time.h>
#include <geos.h>

time_t _systime(void)
{
    struct tm currentTime;

    currentTime.tm_sec = system_date.s_seconds;
    currentTime.tm_min = system_date.s_minutes;
    currentTime.tm_hour = system_date.s_hour;
    currentTime.tm_mday = system_date.s_day;
    currentTime.tm_mon = system_date.s_month;
    currentTime.tm_year = system_date.s_year;
    if (system_date.s_year < 87) {
        currentTime.tm_year+=100;
    }
    currentTime.tm_isdst = -1;

    return mktime(&currentTime);
}

clock_t clock(void)
{
    return _systime();
}
