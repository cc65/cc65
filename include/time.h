/*****************************************************************************/
/*                                                                           */
/*				    time.h				     */
/*                                                                           */
/*				 Date and time				     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2000 Ullrich von Bassewitz                                       */
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



#ifndef _TIME_H
#define _TIME_H



/* NULL pointer */
#ifdef NULL
#  undef NULL
#endif
#define NULL	0

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

#if defined(__CBM__)
#  if defined(__CBM610__)
/* The 610 gets its clock from the AC current */
#    define CLK_TCK    	       	50 	/* POSIX */
#    define CLOCKS_PER_SEC	50	/* ANSI */
#  else
#    define CLK_TCK	  	60	/* POSIX */
#    define CLOCKS_PER_SEC	60     	/* ANSI */
#  endif
#endif

#if defined(__ATARI__)
/* The clock depends on the video standard, so read it at runtime */
unsigned _clocks_per_sec (void);
#  define CLK_TCK		_clocks_per_sec()
#  define CLOCKS_PER_SEC	_clocks_per_sec()
#endif



/* Function prototypes */
clock_t clock (void);



/* End of time.h */

#endif



