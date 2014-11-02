/*****************************************************************************/
/*                                                                           */
/*                                   bin.c                                   */
/*                                                                           */
/*         Binary file output for the sp65 sprite and bitmap utility         */
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



#include <errno.h>
#include <stdio.h>
#include <string.h>

/* common */
#include "attrib.h"

/* sp65 */
#include "attr.h"
#include "bin.h"
#include "error.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void WriteBinFile (const StrBuf* Data, const Collection* A, 
                   const Bitmap* B attribute ((unused)))
/* Write the contents of Data to the given file in binary format */
{
    unsigned Size;

    /* Get the file name */
    const char* Name = NeedAttrVal (A, "name", "write");

    /* Open the output file */
    FILE* F = fopen (Name, "wb");
    if (F == 0) {
        Error ("Cannot open output file `%s': %s", Name, strerror (errno));
    }

    /* Write to the file. We will use fwrite here instead of the fileio
    ** module, since it's just one call, so the latter doesn't have any
    ** advantages, and we can output a more readable error message in case of
    ** problems.
    */
    Size = SB_GetLen (Data);
    if (fwrite (SB_GetConstBuf (Data), 1, Size, F) != Size) {
        Error ("Error writing to output file `%s': %s", Name, strerror (errno));
    }

    /* Close the file */
    if (fclose (F) != 0) {
        Error ("Error closing output file `%s': %s", Name, strerror (errno));
    }
}
