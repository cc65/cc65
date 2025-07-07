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
#include "cmdline.h"
#include "consprop.h"
#include "strbuf.h"

/* ca65 */
#include "error.h"
#include "filetab.h"
#include "lineinfo.h"
#include "nexttok.h"
#include "spool.h"



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

/* Diagnostic category */
typedef enum { DC_NOTE, DC_WARN, DC_ERR, DC_FATAL, DC_COUNT } DiagCat;

/* Descriptions for diagnostic categories */
const char* DiagCatDesc[DC_COUNT] = {
    "Note", "Warning", "Error", "Fatal error"
};



/*****************************************************************************/
/*                             Helper functions                              */
/*****************************************************************************/



static void ReplaceQuotes (StrBuf* Msg)
/* Replace opening and closing single quotes in Msg by their typographically
** correct UTF-8 counterparts for better readbility. A closing quote will
** only get replaced if an opening quote has been seen before.
** To handle some special cases, the function will also treat \xF0 as
** opening and \xF1 as closing quote. These are replaced without the need for
** correct ordering (open/close).
** The function will change the quotes in place, so after the call Msg will
** contain the changed string. If UTF-8 is not available, the function will
** replace '`' by '\'' since that was the default behavior before. It will
** also replace \xF0 and \xF1 by '\''.
**/
{
    /* UTF-8 characters for single quotes */
    static const char QuoteStart[] = "\xE2\x80\x98";
    static const char QuoteEnd[]   = "\xE2\x80\x99";

    /* Remember a few things */
    int IsUTF8 = CP_IsUTF8 ();

    /* We create a new string in T and will later copy it back to Msg */
    StrBuf T = AUTO_STRBUF_INITIALIZER;

    /* Parse the string and create a modified copy */
    SB_Reset (Msg);
    int InQuote = 0;
    while (1) {
        char C = SB_Get (Msg);
        switch (C) {
            case '`':
                if (!InQuote && IsUTF8) {
                    SB_AppendStr (&T, QuoteStart);
                    InQuote = 1;
                } else {
                    /* ca65 uses \' for opening and closing quotes */
                    SB_AppendChar (&T, '\'');
                }
                break;
            case '\'':
                if (InQuote && IsUTF8) {
                    SB_AppendStr (&T, QuoteEnd);
                    InQuote = 0;
                } else {
                    SB_AppendChar (&T, C);
                }
                break;
            case '\xF0':
                if (IsUTF8) {
                    SB_AppendStr (&T, QuoteStart);
                } else {
                    SB_AppendChar (&T, '\'');
                }
                break;
            case '\xF1':
                if (IsUTF8) {
                    SB_AppendStr (&T, QuoteEnd);
                } else {
                    SB_AppendChar (&T, '\'');
                }
                break;
            case '\0':
                goto Done;
            default:
                SB_AppendChar (&T, C);
                break;
        }
    }

Done:
    /* Copy the string back, then terminate it */
    SB_Move (Msg, &T);
    SB_Terminate (Msg);
}



static void VPrintMsg (const FilePos* Pos, DiagCat Cat, const char* Format,
                       va_list ap)
/* Format and output an error/warning message. */
{
    StrBuf S   = AUTO_STRBUF_INITIALIZER;
    StrBuf Msg = AUTO_STRBUF_INITIALIZER;
    StrBuf Loc = AUTO_STRBUF_INITIALIZER;

    /* Determine the description for the category and its color */
    const char* Desc = DiagCatDesc[Cat];
    const char* Color;
    switch (Cat) {
        case DC_NOTE:   Color = CP_Cyan ();             break;
        case DC_WARN:   Color = CP_Yellow ();           break;
        case DC_ERR:    Color = CP_BrightRed ();        break;
        case DC_FATAL:  Color = CP_BrightRed ();        break;
        default:        FAIL ("Unexpected Cat value");  break;
    }

    /* Format the actual message, then replace quotes */
    SB_VPrintf (&Msg, Format, ap);
    ReplaceQuotes (&Msg);

    /* Format the location. If the file position is valid, we use the file
    ** position, otherwise the program name. This allows to print fatal
    ** errors in the startup phase.
    */
    if (Pos->Name == EMPTY_STRING_ID) {
        SB_CopyStr (&Loc, ProgName);
    } else {
        SB_Printf (&Loc, "%s:%u", SB_GetConstBuf (GetFileName (Pos->Name)),
                   Pos->Line);
    }
    SB_Terminate (&Loc);

    /* Format the full message */
    SB_Printf (&S, "%s%s: %s%s:%s %s%s",
               CP_White (),
               SB_GetConstBuf (&Loc),
               Color,
               Desc,
               CP_White (),
               SB_GetConstBuf (&Msg),
               CP_Reset ());

    /* Delete the formatted message and the location string */
    SB_Done (&Loc);
    SB_Done (&Msg);

    /* Add a new line and terminate the generated full message */
    SB_AppendChar (&S, '\n');
    SB_Terminate (&S);

    /* Output the full message */
    fputs (SB_GetConstBuf (&S), stderr);

    /* Delete the buffer for the full message */
    SB_Done (&S);
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
                Msg = "Assembly code generated from this line";
                break;

            case LI_TYPE_MACRO:
                Msg = "Expanded from macro here";
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
                PNotification (GetSourcePos (LI), "%s", Msg);
                ++Output;
            } else {
                ++Skipped;
            }
        }
    }

    /* Add a note if we have more stuff that we won't output */
    if (Skipped > 0) {
        const LineInfo* LI = CollConstAt (LineInfos, 0);
        PNotification (GetSourcePos (LI), "Dropping %u additional line infos",
                       Skipped);
    }
}



/*****************************************************************************/
/*                               Notifications                               */
/*****************************************************************************/



void Notification (const char* Format, ...)
/* Print a notification message. */
{
    /* Output the message */
    va_list ap;
    va_start (ap, Format);
    VPrintMsg (&CurTok.Pos, DC_NOTE, Format, ap);
    va_end (ap);
}



void PNotification (const FilePos* Pos, const char* Format, ...)
/* Print a notification message. */
{
    /* Output the message */
    va_list ap;
    va_start (ap, Format);
    VPrintMsg (Pos, DC_NOTE, Format, ap);
    va_end (ap);
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
    VPrintMsg (GetSourcePos (LI), DC_WARN, Format, ap);

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
        VPrintMsg (Pos, DC_WARN, Format, ap);
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
    VPrintMsg (GetSourcePos (LI), DC_ERR, Format, ap);

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
    VPrintMsg (Pos, DC_ERR, Format, ap);
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
    /* Output the message ... */
    va_list ap;
    va_start (ap, Format);
    VPrintMsg (&CurTok.Pos, DC_FATAL, Format, ap);
    va_end (ap);

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
