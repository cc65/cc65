/*****************************************************************************/
/*                                                                           */
/*                                localtime.c                                */
/*                                                                           */
/*            Convert calendar time into broken down local time              */
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



struct tm* __fastcall__ localtime (const time_t* timep)
{
    time_t t;

    /* Check for a valid time spec */
    if (timep == 0) {
        return 0;
    }

    /* Get the time and correct for the time zone offset */
    t = *timep + _tz.timezone;

    /* Use gmtime for conversion */
    return gmtime (&t);
}
