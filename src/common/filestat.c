/*****************************************************************************/
/*                                                                           */
/*                                filestat.c                                 */
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
** Microsoft. See here for a description of the problem:
**   http://www.codeproject.com/KB/datetime/dstbugs.aspx
** Please let me note that I find it absolutely unacceptable to just declare
** buggy behaviour like this "works as designed" as Microsoft does. The
** problems did even make it into .NET, where the DateTime builtin data type
** has exactly the same problems as described in the article above.
*/



#include <sys/types.h>
#include <sys/stat.h>
#if defined(_WIN32)
#  include <errno.h>
#  include <windows.h>
#endif

/* common */
#include "filestat.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



#if defined(_WIN32)



static time_t FileTimeToUnixTime (const FILETIME* T)
/* Calculate a unix time_t value from a FILETIME. FILETIME contains a 64 bit
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
    V.LowPart  = T->dwLowDateTime;
    V.HighPart = T->dwHighDateTime;
    return (V.QuadPart / 10000000U) - Offs.QuadPart;
}



int FileStat (const char* Path, struct stat* Buf)
/* Replacement function for stat() */
{

    HANDLE                      H;
    BY_HANDLE_FILE_INFORMATION  Info;

    /* First call stat() */
    int Error = stat (Path, Buf);
    if (Error != 0) {
        return Error;
    }

    /* Open the file using backup semantics, so we won't change atime. Then
    ** retrieve the correct times in UTC and replace the ones in Buf. Return
    ** EACCES in case of errors to avoid the hassle of translating windows
    ** error codes to standard ones.
    */
    H = CreateFile (Path,
                    GENERIC_READ,
                    FILE_SHARE_READ,
                    0,                          /* Security attributes */
                    OPEN_EXISTING,
                    FILE_FLAG_BACKUP_SEMANTICS,
                    0);                         /* Template file */
    if (H != INVALID_HANDLE_VALUE) {
        if (GetFileInformationByHandle (H, &Info)) {
            Buf->st_ctime = FileTimeToUnixTime (&Info.ftCreationTime);
            Buf->st_atime = FileTimeToUnixTime (&Info.ftLastAccessTime);
            Buf->st_mtime = FileTimeToUnixTime (&Info.ftLastWriteTime);
        } else {
            Error = EACCES;
        }
        (void) CloseHandle (H);
    } else {
        Error = EACCES;
    }

    /* Done */
    return Error;
}



#else



int FileStat (const char* Path, struct stat* Buf)
/* Replacement function for stat() */
{
    /* Just call the function which works without errors */
    return stat (Path, Buf);
}



#endif
