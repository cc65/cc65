/*****************************************************************************/
/*                                                                           */
/*                                 output.c                                  */
/*                                                                           */
/*                           Output file handling                            */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2009-2012, Ullrich von Bassewitz                                      */
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



#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

/* common */
#include "check.h"
#include "fname.h"
#include "print.h"
#include "xmalloc.h"

/* cc65 */
#include "error.h"
#include "global.h"
#include "output.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Name of the output file. Dynamically allocated and read only. */
const char* OutputFilename = 0;

/* Output file handle */
FILE* OutputFile = 0;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void SetOutputName (const char* Name)
/* Sets the name of the output file. */
{
    OutputFilename = Name;
}



void MakeDefaultOutputName (const char* InputFilename)
/* If the name of the output file is empty or NULL, the name of the output
** file is derived from the input file by adjusting the file name extension.
*/
{
    if (OutputFilename == 0 || *OutputFilename == '\0') {
        /* We don't have an output file for now */
        const char* Ext = PreprocessOnly? ".i" : ".s";
        OutputFilename = MakeFilename (InputFilename, Ext);
    }
}



void OpenOutputFile ()
/* Open the output file. Will call Fatal() in case of failures. */
{
    /* Output file must not be open and we must have a name*/
    PRECONDITION (OutputFile == 0 && OutputFilename != 0);

    /* Open the file */
    OutputFile = fopen (OutputFilename, "w");
    if (OutputFile == 0) {
        Fatal ("Cannot open output file `%s': %s", OutputFilename, strerror (errno));
    }
    Print (stdout, 1, "Opened output file `%s'\n", OutputFilename);
}



void OpenDebugOutputFile (const char* Name)
/* Open an output file for debugging purposes. Will call Fatal() in case of
** failures.
*/
{
    /* Output file must not be open and we must have a name*/
    PRECONDITION (OutputFile == 0);

    /* Open the file */
    OutputFile = fopen (Name, "w");
    if (OutputFile == 0) {
        Fatal ("Cannot open debug output file `%s': %s", Name, strerror (errno));
    }
    Print (stdout, 1, "Opened debug output file `%s'\n", Name);
}



void CloseOutputFile ()
/* Close the output file. Will call Fatal() in case of failures. */
{
    /* Output file must be open */
    PRECONDITION (OutputFile != 0);

    /* Close the file, check for errors */
    if (fclose (OutputFile) != 0) {
        remove (OutputFilename);
        Fatal ("Cannot write to output file (disk full?)");
    }
    Print (stdout, 1, "Closed output file `%s'\n", OutputFilename);

    OutputFile = 0;
}



int WriteOutput (const char* Format, ...)
/* Write to the output file using printf like formatting. Returns the number
** of chars written.
*/
{
    va_list ap;
    int CharCount;

    /* Must have an output file */
    PRECONDITION (OutputFile != 0);

    /* Output formatted */
    va_start (ap, Format);
    CharCount = vfprintf (OutputFile, Format, ap);
    va_end (ap);

    /* Return the number of chars written */
    return CharCount;
}
