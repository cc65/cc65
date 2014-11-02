/*****************************************************************************/
/*                                                                           */
/*                                  error.c                                  */
/*                                                                           */
/*                Error handling for the ca65 macroassembler                 */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2012, Ullrich von Bassewitz                                      */
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
#include <stdlib.h>
#include <stdarg.h>

/* common */
#include "strbuf.h"

/* ca65 */
#include "error.h"
#include "filetab.h"
#include "lineinfo.h"
#include "nexttok.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Warning level */
unsigned WarnLevel      = 1;

/* Statistics */
unsigned ErrorCount     = 0;
unsigned WarningCount   = 0;

/* Maximum number of additional notifications */
#define MAX_NOTES       8



/*****************************************************************************/
/*                             Helper functions                              */
/*****************************************************************************/



static void VPrintMsg (const FilePos* Pos, const char* Desc,
                       const char* Format, va_list ap)
/* Format and output an error/warning message. */
{
    StrBuf S = STATIC_STRBUF_INITIALIZER;

    /* Format the actual message */
    StrBuf Msg = STATIC_STRBUF_INITIALIZER;
    SB_VPrintf (&Msg, Format, ap);
    SB_Terminate (&Msg);

    /* Format the message header */
    SB_Printf (&S, "%s(%u): %s: ",
               SB_GetConstBuf (GetFileName (Pos->Name)),
               Pos->Line,
               Desc);

    /* Append the message to the message header */
    SB_Append (&S, &Msg);

    /* Delete the formatted message */
    SB_Done (&Msg);

    /* Add a new line and terminate the generated full message */
    SB_AppendChar (&S, '\n');
    SB_Terminate (&S);

    /* Output the full message */
    fputs (SB_GetConstBuf (&S), stderr);

    /* Delete the buffer for the full message */
    SB_Done (&S);
}



static void PrintMsg (const FilePos* Pos, const char* Desc,
                      const char* Format, ...)
/* Format and output an error/warning message. */
{
    va_list ap;
    va_start (ap, Format);
    VPrintMsg (Pos, Desc, Format, ap);
    va_end (ap);
}



static void AddNotifications (const Collection* LineInfos)
/* Output additional notifications for an error or warning */
{
    unsigned I;
    unsigned Output;
    unsigned Skipped;

    /* The basic line info is always in slot zero. It has been used to
    ** output the actual error or warning. The following slots may contain
    ** more information. Check them and print additional notifications if
    ** they're present, but limit the number to a reasonable value.
    */
    for (I = 1, Output = 0, Skipped = 0; I < CollCount (LineInfos); ++I) {
        /* Get next line info */
        const LineInfo* LI = CollConstAt (LineInfos, I);
        /* Check the type and output an appropriate note */
        const char* Msg;
        switch (GetLineInfoType (LI)) {

            case LI_TYPE_ASM:
                Msg = "Expanded from here";
                break;

            case LI_TYPE_EXT:
                Msg = "Assembler code generated from this line";
                break;

            case LI_TYPE_MACRO:
                Msg = "Macro was defined here";
                break;

            case LI_TYPE_MACPARAM:
                Msg = "Macro parameter came from here";
                break;

            default:
                /* No output */
                Msg = 0;
                break;

        }

        /* Output until an upper limit of messages is reached */
        if (Msg) {
            if (Output < MAX_NOTES) {
                PrintMsg (GetSourcePos (LI), "Note", "%s", Msg);
                ++Output;
            } else {
                ++Skipped;
            }
        }
    }

    /* Add a note if we have more stuff that we won't output */
    if (Skipped > 0) {
        const LineInfo* LI = CollConstAt (LineInfos, 0);
        PrintMsg (GetSourcePos (LI), "Note",
                  "Dropping %u additional line infos", Skipped);
    }
}



/*****************************************************************************/
/*                                 Warnings                                  */
/*****************************************************************************/



static void WarningMsg (const Collection* LineInfos, const char* Format, va_list ap)
/* Print warning message. */
{
    /* The first entry in the collection is that of the actual source pos */
    const LineInfo* LI = CollConstAt (LineInfos, 0);

    /* Output a warning for this position */
    VPrintMsg (GetSourcePos (LI), "Warning", Format, ap);

    /* Add additional notifications if necessary */
    AddNotifications (LineInfos);

    /* Count warnings */
    ++WarningCount;
}



void Warning (unsigned Level, const char* Format, ...)
/* Print warning message. */
{
    if (Level <= WarnLevel) {

        va_list ap;
        Collection LineInfos = STATIC_COLLECTION_INITIALIZER;

        /* Get line infos for the current position */
        GetFullLineInfo (&LineInfos);

        /* Output the message */
        va_start (ap, Format);
        WarningMsg (&LineInfos, Format, ap);
        va_end (ap);

        /* Free the line info list */
        ReleaseFullLineInfo (&LineInfos);
        DoneCollection (&LineInfos);
    }
}



void PWarning (const FilePos* Pos, unsigned Level, const char* Format, ...)
/* Print warning message giving an explicit file and position. */
{
    if (Level <= WarnLevel) {
        va_list ap;
        va_start (ap, Format);
        VPrintMsg (Pos, "Warning", Format, ap);
        va_end (ap);

        /* Count warnings */
        ++WarningCount;
    }
}



void LIWarning (const Collection* LineInfos, unsigned Level, const char* Format, ...)
/* Print warning message using the given line infos */
{
    if (Level <= WarnLevel) {
        /* Output the message */
        va_list ap;
        va_start (ap, Format);
        WarningMsg (LineInfos, Format, ap);
        va_end (ap);
    }
}



/*****************************************************************************/
/*                                  Errors                                   */
/*****************************************************************************/



void ErrorMsg (const Collection* LineInfos, const char* Format, va_list ap)
/* Print an error message */
{
    /* The first entry in the collection is that of the actual source pos */
    const LineInfo* LI = CollConstAt (LineInfos, 0);

    /* Output an error for this position */
    VPrintMsg (GetSourcePos (LI), "Error", Format, ap);

    /* Add additional notifications if necessary */
    AddNotifications (LineInfos);

    /* Count errors */
    ++ErrorCount;
}



void Error (const char* Format, ...)
/* Print an error message */
{
    va_list ap;
    Collection LineInfos = STATIC_COLLECTION_INITIALIZER;

    /* Get line infos for the current position */
    GetFullLineInfo (&LineInfos);

    /* Output the message */
    va_start (ap, Format);
    ErrorMsg (&LineInfos, Format, ap);
    va_end (ap);

    /* Free the line info list */
    ReleaseFullLineInfo (&LineInfos);
    DoneCollection (&LineInfos);
}



void PError (const FilePos* Pos, const char* Format, ...)
/* Print an error message giving an explicit file and position. */
{
    va_list ap;
    va_start (ap, Format);
    VPrintMsg (Pos, "Error", Format, ap);
    va_end (ap);

    /* Count errors */
    ++ErrorCount;
}



void LIError (const Collection* LineInfos, const char* Format, ...)
/* Print an error message using the given line infos. */
{
    /* Output an error for this position */
    va_list ap;
    va_start (ap, Format);
    ErrorMsg (LineInfos, Format, ap);
    va_end (ap);
}



void ErrorSkip (const char* Format, ...)
/* Print an error message and skip the rest of the line */
{
    va_list ap;
    Collection LineInfos = STATIC_COLLECTION_INITIALIZER;

    /* Get line infos for the current position */
    GetFullLineInfo (&LineInfos);

    /* Output the message */
    va_start (ap, Format);
    ErrorMsg (&LineInfos, Format, ap);
    va_end (ap);

    /* Free the line info list */
    ReleaseFullLineInfo (&LineInfos);
    DoneCollection (&LineInfos);

    /* Skip tokens until we reach the end of the line */
    SkipUntilSep ();
}



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void Fatal (const char* Format, ...)
/* Print a message about a fatal error and die */
{
    va_list ap;
    StrBuf S = STATIC_STRBUF_INITIALIZER;

    va_start (ap, Format);
    SB_VPrintf (&S, Format, ap);
    SB_Terminate (&S);
    va_end (ap);

    fprintf (stderr, "Fatal error: %s\n", SB_GetConstBuf (&S));

    SB_Done (&S);

    /* And die... */
    exit (EXIT_FAILURE);
}



void Internal (const char* Format, ...)
/* Print a message about an internal assembler error and die. */
{
    va_list ap;
    StrBuf S = STATIC_STRBUF_INITIALIZER;

    va_start (ap, Format);
    SB_VPrintf (&S, Format, ap);
    SB_Terminate (&S);
    va_end (ap);

    fprintf (stderr, "Internal assembler error: %s\n", SB_GetConstBuf (&S));

    SB_Done (&S);

    exit (EXIT_FAILURE);
}
