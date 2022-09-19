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



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/

/*
    CAUTION: we need to reserve enough space to be able to hold the maximum
    length string:

     1234567890123456789012345678901234567
    "Wednesday September ..1 00:00:00 1970"
*/

char* __fastcall__ asctime (const struct tm* timep)
{
    static char buf[38];

    /* Format into given buffer and return the result */
    return strftime (buf, sizeof (buf), "%c\n", timep)? buf : 0;
}
