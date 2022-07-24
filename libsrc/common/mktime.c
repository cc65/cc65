/*****************************************************************************/
/*                                                                           */
/*                                 mktime.c                                  */
/*                                                                           */
/*           Make calendar time from broken down time and cleanup            */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2002      Ullrich von Bassewitz                                       */
/*               Wacholderweg 14                                             */
/*               D-70597 Stuttgart                                           */
/* EMail:        uz@musoftware.de                                            */
/*                                                                           */
/*                                                                           */
/* This software is provided 'as-is', without any expressed or implied       */
/* warranty.  In no event will the authors be held liable for any damages    */
/* arising from the use of this software.                                    */
/*                                                                           */
/* Permission is granted to anyone to use this software for any purpose,     */
/* including commercial applications, and to alter it and redistribute it    */
/* freely, subject to the following restrictions:                            */
/*                                                                           */
/* 1. The origin of this software must not be misrepresented; you must not   */
/*    claim that you wrote the original software. If you use this software   */
/*    in a product, an acknowledgment in the product documentation would be  */
/*    appreciated but is not required.                                       */
/* 2. Altered source versions must be plainly marked as such, and must not   */
/*    be misrepresented as being the original software.                      */
/* 3. This notice may not be removed or altered from any source              */
/*    distribution.                                                          */
/*                                                                           */
/*****************************************************************************/



#include <limits.h>
#include <stdlib.h>
#include <time.h>



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



#define JANUARY         0
#define FEBRUARY        1
#define DECEMBER       11
#define JAN_1_1970      4               /* 1/1/1970 is a thursday */



static const unsigned char MonthLength [] = {
    31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};
static const unsigned MonthDays [] = {
    0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334
};



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static unsigned char __fastcall__ IsLeapYear (unsigned Year)
/* Returns 1 if the given year is a leap year */
{
    return (((Year % 4) == 0) && ((Year % 100) != 0 || (Year % 400) == 0));
}



time_t __fastcall__ mktime (register struct tm* TM)
/* Make a time in seconds since 1/1/1970 from the broken down time in TM.
** A call to mktime does also correct the time in TM to contain correct
** values.
*/
{
    register div_t D;
    int Max;
    unsigned DayCount;

    /* Check if TM is valid */
    if (TM == 0) {
        /* Invalid data */
        goto Error;
    }

    /* Adjust seconds. */
    D = div (TM->tm_sec, 60);
    TM->tm_sec = D.rem;

    /* Adjust minutes */
    if (TM->tm_min + D.quot < 0) {
        goto Error;
    }
    TM->tm_min += D.quot;
    D = div (TM->tm_min, 60);
    TM->tm_min = D.rem;

    /* Adjust hours */
    if (TM->tm_hour + D.quot < 0) {
        goto Error;
    }
    TM->tm_hour += D.quot;
    D = div (TM->tm_hour, 24);
    TM->tm_hour = D.rem;

    /* Adjust days */
    if (TM->tm_mday + D.quot < 0) {
        goto Error;
    }
    TM->tm_mday += D.quot;

    /* Adjust month and year. This is an iterative process, since changing
    ** the month will change the allowed days for this month.
    */
    while (1) {

        /* Make sure, month is in the range 0..11 */
        D = div (TM->tm_mon, 12);
        TM->tm_mon = D.rem;
        if (TM->tm_year + D.quot < 0) {
            goto Error;
        }
        TM->tm_year += D.quot;

        /* Now check if mday is in the correct range, if not, correct month
        ** and eventually year and repeat the process.
        */
        if (TM->tm_mon == FEBRUARY && IsLeapYear (TM->tm_year + 1900)) {
            Max = 29;
        } else {
            Max = MonthLength[TM->tm_mon];
        }
        if (TM->tm_mday > Max) {
            /* Must correct month and eventually, year */
            if (TM->tm_mon == DECEMBER) {
                TM->tm_mon = JANUARY;
                ++TM->tm_year;
            } else {
                ++TM->tm_mon;
            }
            TM->tm_mday -= Max;
        } else {
            /* Done */
            break;
        }
    }

    /* Ok, all time/date fields are now correct. Calculate the days in this
    ** year.
    */
    TM->tm_yday = MonthDays[TM->tm_mon] + TM->tm_mday - 1;
    if (TM->tm_mon > FEBRUARY && IsLeapYear (TM->tm_year + 1900)) {
        ++TM->tm_yday;
    }

    /* Calculate days since 1/1/1970. In the complete epoch (1/1/1970 to
    ** somewhere in 2038) all years dividable by 4 are leap years, so
    ** dividing by 4 gives the days that must be added cause of leap years.
    ** (and the last leap year before 1970 was 1968)
    */
    DayCount = ((unsigned) (TM->tm_year-70)) * 365U +
               (((unsigned) (TM->tm_year-(68+1))) / 4) +
               TM->tm_yday;

    /* Calculate the weekday */
    TM->tm_wday = (JAN_1_1970 + DayCount) % 7;

    /* No (US) daylight saving (for now) */
    TM->tm_isdst = 0;

    /* Return seconds since 1970 */
    return DayCount * 86400UL +
           ((unsigned) TM->tm_hour) * 3600UL +
           ((unsigned) TM->tm_min) * 60U +
           ((unsigned) TM->tm_sec) -
           _tz.timezone;

Error:
    /* Error exit */
    return (time_t) -1L;
}



