/*****************************************************************************/
/*                                                                           */
/*                                filetype.c                                 */
/*                                                                           */
/*                       Determine the type of a file                        */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2003      Ullrich von Bassewitz                                       */
/*               Römerstrasse 52                                             */
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

/* common */
#include "filetype.h"
#include "fname.h"



/*****************************************************************************/
/*	      	     	      	     Data		     		     */
/*****************************************************************************/



/* Table that maps extensions to file types */
static const struct {
    const char	Ext[4];
    unsigned	Type;
} FileTypes [] = {
    {   "c",	FILETYPE_C	},
    {   "s",	FILETYPE_ASM	},
    {   "asm",	FILETYPE_ASM	},
    {   "a65",	FILETYPE_ASM	},
    {   "o",	FILETYPE_OBJ	},
    {   "obj",	FILETYPE_OBJ	},
    {   "a",	FILETYPE_LIB	},
    {   "lib",	FILETYPE_LIB	},
    {   "grc",	FILETYPE_GR	},
    {   "o65",  FILETYPE_O65    },
    {   "emd",  FILETYPE_O65    },
    {   "joy",  FILETYPE_O65    },
    {   "tgi",  FILETYPE_O65    },
};



/*****************************************************************************/
/*	       	     	     	     Code		     		     */
/*****************************************************************************/



int GetFileType (const char* Name)
/* Determine the type of the given file by looking at the name. If the file
 * type could not be determined, the function returns FILETYPE_UNKOWN.
 */
{
    unsigned I;

    /* Determine the file type by the extension */
    const char* Ext = FindExt (Name);

    /* Do we have an extension? */
    if (Ext == 0) {
	return FILETYPE_UNKNOWN;
    }
                
    /* Skip the dot */
    ++Ext;

    /* Check for known extensions */
    for (I = 0; I < sizeof (FileTypes) / sizeof (FileTypes [0]); ++I) {
	if (strcmp (FileTypes [I].Ext, Ext) == 0) {
	    /* Found */
	    return FileTypes [I].Type;
	}
    }

    /* Not found, return the default */
    return FILETYPE_UNKNOWN;
}



