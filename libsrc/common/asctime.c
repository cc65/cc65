/*****************************************************************************/
/*                                                                           */
/*                                   asctime.c                               */
/*                                                                           */
/*                 Convert a broken down time into a string                  */
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



#include <stdio.h>
#include <time.h>



char* __fastcall__ asctime (const struct tm* timep)
{
    static const char days[7][4]  = {
        "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
    };
    static const char months[12][4] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };
    static char buf[26];

    /* Create a copy of the given data and make sure it is valid */
    struct tm t;
    t = *timep;
    mktime (&t);

    /* Format into given buffer */
    sprintf(buf,
            "%s %s%3d %02d:%02d:%02d %d\n",
     	    days[t.tm_wday],
            months[t.tm_mon],
            t.tm_mday,
     	    t.tm_hour,
            t.tm_min,
            t.tm_sec,
            t.tm_year + 1900);

    /* Return the result */
    return buf;
}



