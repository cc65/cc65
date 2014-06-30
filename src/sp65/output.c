/*****************************************************************************/
/*                                                                           */
/*                                 output.c                                  */
/*                                                                           */
/*   Output format/file definitions for the sp65 sprite and bitmap utility   */
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
#include "asm.h"
#include "attr.h"
#include "bin.h"
#include "c.h"
#include "error.h"
#include "output.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Different types of output formats */
enum OutputFormat {
    ofAsm,                      /* Output assembler source */
    ofBin,                      /* Output raw binary format */
    ofC,                        /* Output C code */

    ofCount                     /* Number of output formats without ofAuto */
};

typedef struct OutputFormatDesc OutputFormatDesc;
struct OutputFormatDesc {

    /* Write routine */
    void (*Write) (const StrBuf*, const Collection*, const Bitmap*);

};

/* Table with Output formats */
static OutputFormatDesc OutputFormatTable[ofCount] = {
    {   WriteAsmFile    },
    {   WriteBinFile    },
    {   WriteCFile      },
};

/* Table that maps extensions to Output formats. Must be sorted alphabetically */
static const FileId FormatTable[] = {
    /* Upper case stuff for obsolete operating systems */
    {   "A",    ofAsm           },
    {   "ASM",  ofAsm           },
    {   "BIN",  ofBin           },
    {   "C",    ofC             },
    {   "INC",  ofAsm           },
    {   "S",    ofAsm           },

    {   "a",    ofAsm           },
    {   "asm",  ofAsm           },
    {   "bin",  ofBin           },
    {   "c",    ofC             },
    {   "inc",  ofAsm           },
    {   "s",    ofAsm           },
};



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void WriteOutputFile (const StrBuf* Data, const Collection* A, const Bitmap* B)
/* Write the contents of Data to a file. Format, file name etc. must be given
** as attributes in A. If no format is given, the function tries to autodetect
** it by using the extension of the file name. The bitmap passed to the
** function is the bitmap used as source of the conversion. It may be used to
** determine the bitmap properties for documentation purposes.
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
            Error ("Unknown output format `%s'", Format);
        }
    } else {
        /* No format given, use file name extension */
        const char* Name = NeedAttrVal (A, "name", "write");
        F = GetFileId (Name, FormatTable,
                       sizeof (FormatTable) / sizeof (FormatTable[0]));
        /* Found? */
        if (F == 0) {
            Error ("Cannot determine file format of output file `%s'", Name);
        }
    }

    /* Call the format specific write */
    OutputFormatTable[F->Id].Write (Data, A, B);
}
