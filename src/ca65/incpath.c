/*****************************************************************************/
/*                                                                           */
/*				   incpath.c				     */
/*                                                                           */
/*	      Include path handling for the ca65 macro assembler	     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000      Ullrich von Bassewitz                                       */
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
#include <string.h>
#if defined(_MSC_VER)
/* Microsoft compiler */
#  include <io.h>
#else
/* Anyone else */
#  include <unistd.h>
#endif

#include "../common/xmalloc.h"

#include "incpath.h"



/*****************************************************************************/
/*	      	     		     Data		     		     */
/*****************************************************************************/



static char* IncludePath  = 0;



/*****************************************************************************/
/*     	     	    		     Code   				     */
/*****************************************************************************/



static char* Add (char* Orig, const char* New)
/* Create a new path from Orig and New, delete Orig, return the result */
{
    unsigned OrigLen, NewLen;
    char* NewPath;

    /* Get the length of the original string */
    OrigLen = Orig? strlen (Orig) : 0;

    /* Get the length of the new path */
    NewLen = strlen (New);

    /* Check for a trailing path separator and remove it */
    if (NewLen > 0 && (New [NewLen-1] == '\\' || New [NewLen-1] == '/')) {
    	--NewLen;
    }

    /* Allocate memory for the new string */
    NewPath = xmalloc (OrigLen + NewLen + 2);

    /* Copy the strings */
    memcpy (NewPath, Orig, OrigLen);
    memcpy (NewPath+OrigLen, New, NewLen);
    NewPath [OrigLen+NewLen+0] = ';';
    NewPath [OrigLen+NewLen+1] = '\0';

    /* Delete the original path */
    xfree (Orig);

    /* Return the new path */
    return NewPath;
}



static char* Find (const char* Path, const char* File)
/* Search for a file in a list of directories. If found, return the complete
 * name including the path in a malloced data area, if not found, return 0.
 */
{
    const char* P;
    int Max;
    char PathName [FILENAME_MAX];

    /* Initialize variables */
    Max = sizeof (PathName) - strlen (File) - 2;
    if (Max < 0) {
     	return 0;
    }
    P = Path;

    /* Handle a NULL pointer as replacement for an empty string */
    if (P == 0) {
     	P = "";
    }

    /* Start the search */
    while (*P) {
        /* Copy the next path element into the buffer */
     	int Count = 0;
     	while (*P != '\0' && *P != ';' && Count < Max) {
     	    PathName [Count++] = *P++;
     	}

     	/* Add a path separator and the filename */
     	if (Count) {
     	    PathName [Count++] = '/';
     	}
     	strcpy (PathName + Count, File);

     	/* Check if this file exists */
     	if (access (PathName, 0) == 0) {
     	    /* The file exists */
     	    return xstrdup (PathName);
     	}

     	/* Skip a list separator if we have one */
     	if (*P == ';') {
     	    ++P;
     	}
    }

    /* Not found */
    return 0;
}



void AddIncludePath (const char* NewPath)
/* Add a new include path to the existing one */
{
    /* Allow a NULL path */
    if (NewPath) {
	IncludePath = Add (IncludePath, NewPath);
    }
}



char* FindInclude (const char* Name)
/* Find an include file. Return a pointer to a malloced area that contains
 * the complete path, if found, return 0 otherwise.
 */
{
    /* Search in the include directories */
    return Find (IncludePath, Name);
}



