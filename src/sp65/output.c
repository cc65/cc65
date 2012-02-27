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



/* common */
#include "fileid.h"

/* sp65 */
#include "asm.h"
#include "bin.h"
#include "error.h"
#include "output.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



typedef struct OutputFormatDesc OutputFormatDesc;
struct OutputFormatDesc {

    /* Write routine */
    void (*Write) (const char* Name, const StrBuf* Data);

};

/* Table with Output formats */
static OutputFormatDesc OutputFormatTable[ofCount] = {
    {   WriteAsmFile    },
    {   WriteBinFile    },
};

/* Table that maps extensions to Output formats. Must be sorted alphabetically */
static const FileId FormatTable[] = {
    /* Upper case stuff for obsolete operating systems */
    {   "A",    ofAsm           },
    {   "ASM",  ofAsm           },
    {   "BIN",  ofBin           },
    {   "INC",  ofAsm           },
    {   "S",    ofAsm           },

    {   "a",    ofAsm           },
    {   "asm",  ofAsm           },
    {   "bin",  ofBin           },
    {   "inc",  ofAsm           },
    {   "s",    ofAsm           },
};



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void WriteOutputFile (const char* Name, const StrBuf* Data, OutputFormat Format)
/* Write the contents of Data to the given file in the format specified. If
 * the format is ofAuto, it is determined by the file extension.
 */
{
    /* If the format is Auto, try to determine it from the file name */
    if (Format == ofAuto) {
        /* Search for the entry in the table */
        const FileId* F = GetFileId (Name, FormatTable,
                                     sizeof (FormatTable) / sizeof (FormatTable[0]));
        /* Found? */
        if (F == 0) {
            Error ("Cannot determine file format of output file `%s'", Name);
        }
        Format = F->Id;
    }

    /* Check the format just for safety */
    CHECK (Format >= 0 && Format < ofCount);

    /* Call the format specific write */
    OutputFormatTable[Format].Write (Name, Data);
}



