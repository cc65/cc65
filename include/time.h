/*
 * time.h
 *
 * Ullrich von Bassewitz, 17.06.1998
 *
 */



#ifndef _TIME_H
#define _TIME_H



#include <stddef.h>



typedef unsigned long time_t;
typedef unsigned long clock_t;

/* Structure for broken down time */
struct tm {
    int	tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
};

/* The 610 gets its clock from the AC current */
#ifdef __CBM__
#  ifdef __CBM610__
#    define CLK_TCK    	       	50 	/* POSIX */
#    define CLOCKS_PER_TICK	50	/* ANSI */
#  else
#    define CLK_TCK		60	/* POSIX */
#    define CLOCKS_PER_TICK	60     	/* ANSI */
#  endif
#endif



/* Function prototypes */
clock_t clock (void);



/* End of time.h */

#endif



