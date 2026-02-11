/*****************************************************************************/
/*                                                                           */
/*                                xsprintf.c                                 */
/*                                                                           */
/*                       Replacement sprintf function                        */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2004 Ullrich von Bassewitz                                       */
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



#include <stdio.h>
#include <stdarg.h>

/* common */
#include "check.h"
#include "xsprintf.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



int xvsnprintf (char* Buf, size_t Size, const char* Format, va_list ap)
/* A basic vsnprintf implementation. Does currently only support integer
** formats.
*/
{
    return vsnprintf (Buf, Size, Format, ap);
}



int xsnprintf (char* Buf, size_t Size, const char* Format, ...)
/* A basic snprintf implementation. Does currently only support integer
** formats.
*/
{
    int Res;
    va_list ap;

    va_start (ap, Format);
    Res = vsnprintf (Buf, Size, Format, ap);
    va_end (ap);

    return Res;
}



int xsprintf (char* Buf, size_t BufSize, const char* Format, ...)
/* Replacement function for sprintf */
{
    int Res;
    va_list ap;

    va_start (ap, Format);
    Res = xvsprintf (Buf, BufSize, Format, ap);
    va_end (ap);

    return Res;
}



int xvsprintf (char* Buf, size_t BufSize, const char* Format, va_list ap)
/* Replacement function for sprintf */
{
    int Res = vsnprintf (Buf, BufSize, Format, ap);
    CHECK (Res >= 0 && (unsigned) (Res+1) < BufSize);
    return Res;
}
