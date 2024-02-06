/*****************************************************************************/
/*                                                                           */
/*                                filetime.c                                 */
/*                                                                           */
/*                       Replacement for Windows code                        */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2012,      Ullrich von Bassewitz                                      */
/*                Roemerstrasse 52                                           */
/*                D-70794 Filderstadt                                        */
/* EMail:         uz@cc65.org                                                */
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



/* This module works around bugs in the time conversion code supplied by
** Microsoft. The problem described here:
**   http://www.codeproject.com/KB/datetime/dstbugs.aspx
** is also true when setting file times via utime(), so we need a
** replacement
*/



#if defined(_WIN32)
#  include <errno.h>
#  include <windows.h>
#else
#  include <sys/types.h>                          /* FreeBSD needs this */
#  include <utime.h>
#endif


/* common */
#include "filetime.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



#if defined(_WIN32)



static FILETIME* UnixTimeToFileTime (time_t T, FILETIME* FT)
/* Calculate a FILETIME value from a time_t. FILETIME contains a 64 bit
** value with point zero at 1600-01-01 00:00:00 and counting 100ns intervals.
** time_t is in seconds since 1970-01-01 00:00:00.
*/
{
    /* Offset between 1600-01-01 and the Epoch in seconds. Watcom C has no
    ** way to express a number > 32 bit (known to me) but is able to do
    ** calculations with 64 bit integers, so we need to do it this way.
    */
    static const ULARGE_INTEGER Offs = { { 0xB6109100UL, 0x00000020UL } };
    ULARGE_INTEGER V;
    V.QuadPart = ((unsigned __int64) T + Offs.QuadPart) * 10000000U;
    FT->dwLowDateTime  = V.LowPart;
    FT->dwHighDateTime = V.HighPart;
    return FT;
}



int SetFileTimes (const char* Path, time_t T)
/* Set the time of last modification and the time of last access of a file to
** the given time T. This calls utime() for system where it works, and applies
** workarounds for all others (which in fact means "WINDOWS").
*/
{
    HANDLE   H;
    FILETIME FileTime;
    int      Error = EACCES;                    /* Assume an error */


    /* Open the file */
    H = CreateFile (Path,
                    GENERIC_WRITE,
                    FILE_SHARE_READ,
                    0,                          /* Security attributes */
                    OPEN_EXISTING,
                    0,                          /* File flags */
                    0);                         /* Template file */
    if (H != INVALID_HANDLE_VALUE) {
        /* Set access and modification time */
        UnixTimeToFileTime (T, &FileTime);
        if (SetFileTime (H, 0, &FileTime, &FileTime)) {
            /* Done */
            Error = 0;
        }

        /* Close the handle */
        (void) CloseHandle (H);
    }

    /* Return the error code */
    return Error;
}



#else



int SetFileTimes (const char* Path, time_t T)
/* Set the time of last modification and the time of last access of a file to
** the given time T. This calls utime() for system where it works, and applies
** workarounds for all others (which in fact means "WINDOWS").
*/
{
    struct utimbuf U;

    /* Set access and modification time */
    U.actime  = T;
    U.modtime = T;
    return utime (Path, &U);
}



#endif
