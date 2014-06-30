/*****************************************************************************/
/*                                                                           */
/*                                  fname.c                                  */
/*                                                                           */
/*                       File name handling utilities                        */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2003 Ullrich von Bassewitz                                       */
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



#include <string.h>

#include "xmalloc.h"
#include "fname.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



const char* FindExt (const char* Name)
/* Return a pointer to the file extension in Name or NULL if there is none */
{
    const char* S;

    /* Get the length of the name */
    unsigned Len = strlen (Name);
    if (Len < 2) {
        return 0;
    }

    /* Get a pointer to the last character */
    S = Name + Len - 1;

    /* Search for the dot, beware of subdirectories */
    while (S >= Name && *S != '.' && *S != '\\' && *S != '/') {
        --S;
    }

    /* Did we find an extension? */
    if (*S == '.') {
        return S;
    } else {
        return 0;
    }
}



const char* FindName (const char* Path)
/* Return a pointer to the file name in Path. If there is no path leading to
** the file, the function returns Path as name.
*/
{
    /* Get the length of the name */
    int Len = strlen (Path);

    /* Search for the path separator */
    while (Len > 0 && Path[Len-1] != '\\' && Path[Len-1] != '/') {
        --Len;
    }

    /* Return the name or path */
    return Path + Len;
}



char* MakeFilename (const char* Origin, const char* Ext)
/* Make a new file name from Origin and Ext. If Origin has an extension, it
** is removed and Ext is appended. If Origin has no extension, Ext is simply
** appended. The result is placed in a malloc'ed buffer and returned.
** The function may be used to create "foo.o" from "foo.s".
*/
{
    char* Out;
    const char* P = FindExt (Origin);
    if (P == 0) {
        /* No dot, add the extension */
        Out = xmalloc (strlen (Origin) + strlen (Ext) + 1);
        strcpy (Out, Origin);
        strcat (Out, Ext);
    } else {
        Out = xmalloc (P - Origin + strlen (Ext) + 1);
        memcpy (Out, Origin, P - Origin);
        strcpy (Out + (P - Origin), Ext);
    }
    return Out;
}
