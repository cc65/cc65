/*****************************************************************************/
/*                                                                           */
/*                                  time.h                                   */
/*                                                                           */
/*                               Date and time                               */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2012 Ullrich von Bassewitz                                       */
/*               Roemerstrasse 52                                            */
/*               D-70794 Filderstadt                                         */
/* EMail:        uz@cc65.org                                                 */
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



#ifndef _TIME_H
#define _TIME_H



/* NULL pointer */
#ifndef _HAVE_NULL
#define NULL    0
#define _HAVE_NULL
#endif

/* size_t is needed */
#ifndef _HAVE_size_t
#define _HAVE_size_t
typedef unsigned size_t;
#endif

typedef unsigned long time_t;
typedef unsigned long clock_t;

/* Structure for broken down time */
struct tm {     
    int     tm_sec;
    int     tm_min;
    int     tm_hour;
    int     tm_mday;
    int     tm_mon;
    int     tm_year;
    int     tm_wday;
    int     tm_yday;
    int     tm_isdst;
};

/* Timezone representation, default is UTC */
extern struct _timezone {
    char    daylight;   /* True if daylight savings time active */
    long    timezone;   /* Number of seconds behind UTC */
    char    tzname[5];  /* Name of timezone, e.g. CET */
    char    dstname[5]; /* Name when daylight true, e.g. CEST */
} _tz;



#if defined(__ATARI__)
/* The clock depends on the video standard, so read it at runtime */
unsigned _clocks_per_sec (void);
#  define CLK_TCK               _clocks_per_sec()
#  define CLOCKS_PER_SEC        _clocks_per_sec()
#elif defined(__ATARI5200__)
#  define CLK_TCK               60      /* POSIX */
#  define CLOCKS_PER_SEC        60      /* ANSI */
#elif defined(__ATMOS__)
#  define CLK_TCK               100     /* POSIX */
#  define CLOCKS_PER_SEC        100     /* ANSI */
#elif defined(__CBM__)
#  if defined(__CBM510__) || defined(__CBM610__)
/* The 510/610 gets its clock from the AC current */
#    define CLK_TCK             50      /* POSIX */
#    define CLOCKS_PER_SEC      50      /* ANSI */
#  else
#    define CLK_TCK             60      /* POSIX */
#    define CLOCKS_PER_SEC      60      /* ANSI */
#  endif
#elif defined(__NES__)
#  define CLK_TCK               50      /* POSIX */
#  define CLOCKS_PER_SEC        50      /* ANSI */
#elif defined(__PCE__)
#  define CLK_TCK               60      /* POSIX */
#  define CLOCKS_PER_SEC        60      /* ANSI */
#elif  defined(__GEOS__)
#  define CLK_TCK               1       /* POSIX */
#  define CLOCKS_PER_SEC        1       /* ANSI */
#elif defined(__LYNX__)
/* The clock-rate depends on the video scan-rate;
** so, read it at run-time.
*/
extern clock_t _clk_tck (void);
#  define CLK_TCK               _clk_tck()
#  define CLOCKS_PER_SEC        _clk_tck()
#endif



time_t _systime (void);
/* Similar to time(), but:
**   - Is not ISO C
**   - Does not take the additional pointer
**   - Does not set errno when returning -1
*/

/* ISO C function prototypes */
char* __fastcall__ asctime (const struct tm* timep);
clock_t clock (void);
char* __fastcall__ ctime (const time_t* timep);
struct tm* __fastcall__ gmtime (const time_t* timep);
struct tm* __fastcall__ localtime (const time_t* timep);
time_t __fastcall__ mktime (struct tm* timep);
size_t __fastcall__ strftime (char* buf, size_t bufsize, const char* format, const struct tm* tm);
time_t __fastcall__ time (time_t* t);



/* End of time.h */

#endif



