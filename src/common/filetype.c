/*****************************************************************************/
/*                                                                           */
/*                                filetype.c                                 */
/*                                                                           */
/*                       Determine the type of a file                        */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2003-2012, Ullrich von Bassewitz                                      */
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



#include <stdlib.h>
#include <string.h>

/* common */
#include "fileid.h"
#include "filetype.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



static const FileId TypeTable[] = {
    /* Upper case stuff for obsolete operating systems */
    {   "A",    FILETYPE_LIB    },
    {   "A65",  FILETYPE_ASM    },
    {   "ASM",  FILETYPE_ASM    },
    {   "C",    FILETYPE_C      },
    {   "EMD",  FILETYPE_O65    },
    {   "GRC",  FILETYPE_GR     },
    {   "JOY",  FILETYPE_O65    },
    {   "LIB",  FILETYPE_LIB    },
    {   "MOU",  FILETYPE_O65    },
    {   "O",    FILETYPE_OBJ    },
    {   "O65",  FILETYPE_O65    },
    {   "OBJ",  FILETYPE_OBJ    },
    {   "S",    FILETYPE_ASM    },
    {   "SER",  FILETYPE_O65    },
    {   "TGI",  FILETYPE_O65    },

    {   "a",    FILETYPE_LIB    },
    {   "a65",  FILETYPE_ASM    },
    {   "asm",  FILETYPE_ASM    },
    {   "c",    FILETYPE_C      },
    {   "emd",  FILETYPE_O65    },
    {   "grc",  FILETYPE_GR     },
    {   "joy",  FILETYPE_O65    },
    {   "lib",  FILETYPE_LIB    },
    {   "mou",  FILETYPE_O65    },
    {   "o",    FILETYPE_OBJ    },
    {   "o65",  FILETYPE_O65    },
    {   "obj",  FILETYPE_OBJ    },
    {   "s",    FILETYPE_ASM    },
    {   "ser",  FILETYPE_O65    },
    {   "tgi",  FILETYPE_O65    },
};

#define FILETYPE_COUNT (sizeof (TypeTable) / sizeof (TypeTable[0]))



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



FILETYPE GetFileType (const char* Name)
/* Determine the type of the given file by looking at the name. If the file
** type could not be determined, the function returns FILETYPE_UNKOWN.
*/
{
    /* Search for a table entry */
    const FileId* F = GetFileId (Name, TypeTable, FILETYPE_COUNT);

    /* Return the result */
    return F? F->Id : FILETYPE_UNKNOWN;
}
