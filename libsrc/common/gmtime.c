/*****************************************************************************/
/*                                                                           */
/*                                 gmtime.c                                  */
/*                                                                           */
/*            Convert calendar time into broken down time in UTC             */
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



#include <time.h>



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



struct tm* __fastcall__ gmtime (const time_t* timep)
{
    static struct tm timebuf;
    time_t t;

    /* Check the argument */
    if (timep == 0 || (long) (t = *timep) < 0) {
        /* Invalid arg */
        return 0;
    }

    /* Since our ints are just 16 bits, split the given time into seconds,
    ** hours and days. Each of the values will fit in a 16 bit variable.
    ** The mktime routine will then do the rest.
    */
    timebuf.tm_sec  = t % 3600;
    timebuf.tm_min  = 0;
    timebuf.tm_hour = (t / 3600) % 24;
    timebuf.tm_mday = (t / (3600UL * 24UL)) + 1;
    timebuf.tm_mon  = 0;
    timebuf.tm_year = 70;       /* Base value is 1/1/1970 */

    /* Call mktime to do the final conversion */
    mktime (&timebuf);

    /* Return the result */
    return &timebuf;
}
