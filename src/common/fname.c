/*****************************************************************************/
/*                                                                           */
/*				    fname.c				     */
/*                                                                           */
/*			 File name handling utilities			     */
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



#include <string.h>

#include "xmalloc.h"
#include "fname.h"



/*****************************************************************************/
/*     	       	       	       	     Code				     */
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



char* MakeFilename (const char* Origin, const char* Ext)
/* Make a new file name from Origin and Ext. If Origin has an extension, it
 * is removed and Ext is appended. If Origin has no extension, Ext is simply
 * appended. The result is placed in a malloc'ed buffer and returned.
 * The function may be used to create "foo.o" from "foo.s".
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



