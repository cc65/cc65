/*****************************************************************************/
/*                                                                           */
/*                                pathutil.c                                 */
/*                         Path manipulation utilities                       */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2003-2008 Ullrich von Bassewitz                                       */
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

#if defined(_WIN32)

#if !defined(_WIN32_WINNT) && !defined(NTDDI_VERSION)

/* Set minimum windows version for GetFinalPathNameByHandleA */
/* NTDDI_VISTA */
#define NTDDI_VERSION 0x06000000

/* _WIN32_WINNT_VISTA */
#define _WIN32_WINNT 0x600

#endif

#include "xmalloc.h"
#include <windows.h>
#include <fileapi.h>

#endif

#include <stdlib.h>



/*****************************************************************************/
/*                                   code                                    */
/*****************************************************************************/



#if defined(_WIN32)

char *FindRealPath (const char *Path)
/*
** Returns a malloced buffer containing the canonical path of the given path.
** If the path points to a non-existent file, or if any error occurs, NULL is returned.
** If the path points to a symlink, the resolved symlink path is returned.
** Note: The returned path's separator is system specific.
*/
{

    HANDLE Handle = CreateFileA (Path,
                                 FILE_READ_ATTRIBUTES,
                                 FILE_SHARE_READ |
                                 FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                                 NULL,
                                 OPEN_EXISTING,
                                 FILE_FLAG_BACKUP_SEMANTICS,
                                 NULL);

    if (Handle == INVALID_HANDLE_VALUE) {
        return NULL;
    }

    size_t BufferSize = MAX_PATH + 10;
    char* Buffer = xmalloc (BufferSize);

    DWORD Status = GetFinalPathNameByHandleA (Handle,
                                              Buffer,
                                              BufferSize,
                                              FILE_NAME_NORMALIZED
                                              | VOLUME_NAME_DOS);

    if (Status == 0) {
        xfree (Buffer);
        CloseHandle (Handle);
        return NULL;
    }

    CloseHandle (Handle);

    return Buffer;
}

#else

char* FindRealPath (const char* path)
/*
** Returns a malloced buffer containing the canonical path of the given path.
** If the path points to a non-existent file, or if any error occurs, NULL is returned.
** If the path points to a symlink, the resolved symlink path is returned.
** Note: The returned path's separator is system specific.
*/
{
    return realpath (path, NULL);
}

#endif
