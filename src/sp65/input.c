/*****************************************************************************/
/*                                                                           */
/*                                  input.c                                  */
/*                                                                           */
/*   Input format/file definitions for the sp65 sprite and bitmap utility    */
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



#include <stdlib.h>

/* common */
#include "fileid.h"

/* sp65 */
#include "attr.h"
#include "error.h"
#include "input.h"
#include "pcx.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Possible input formats */
enum InputFormat {
    ifPCX,                      /* PCX */
    ifCount                     /* Number of actual input formats w/o ifAuto*/
};

typedef struct InputFormatDesc InputFormatDesc;
struct InputFormatDesc {
    /* Read routine */
    Bitmap* (*Read) (const Collection*);
};

/* Table with input formats indexed by InputFormat */
static InputFormatDesc InputFormatTable[ifCount] = {
    {   ReadPCXFile     },
};

/* Table that maps extensions to input formats. Must be sorted alphabetically */
static const FileId FormatTable[] = {
    /* Upper case stuff for obsolete operating systems */
    {   "PCX",  ifPCX           },

    {   "pcx",  ifPCX           },
};



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



Bitmap* ReadInputFile (const Collection* A)
/* Read a bitmap from a file and return it. Format, file name etc. must be
** given as attributes in A. If no format is given, the function tries to
** autodetect it by using the extension of the file name.
*/
{
    const FileId* F;

    /* Get the file format from the command line */
    const char* Format = GetAttrVal (A, "format");
    if (Format != 0) {
        /* Format is given, search for it in the table. */
        F = bsearch (Format,
                     FormatTable,
                     sizeof (FormatTable) / sizeof (FormatTable[0]),
                     sizeof (FormatTable[0]),
                     CompareFileId);
        if (F == 0) {
            Error ("Unknown input format `%s'", Format);
        }
    } else {
        /* No format given, use file name extension */
        const char* Name = NeedAttrVal (A, "name", "write");
        F = GetFileId (Name, FormatTable,
                       sizeof (FormatTable) / sizeof (FormatTable[0]));
        /* Found? */
        if (F == 0) {
            Error ("Cannot determine file format of input file `%s'", Name);
        }
    }

    /* Call the format specific read */
    return InputFormatTable[F->Id].Read (A);
}
