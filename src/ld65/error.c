/*****************************************************************************/
/*                                                                           */
/*                                 error.c                                   */
/*                                                                           */
/*                    Error handling for the ld65 linker                     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2008 Ullrich von Bassewitz                                       */
/*               Roemerstrasse 52                                            */
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



#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

/* common */
#include "cmdline.h"
#include "coll.h"
#include "consprop.h"
#include "strbuf.h"

/* ld65 */
#include "error.h"
#include "spool.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Statistics */
unsigned WarningCount   = 0;

/* Diagnostic category. */
typedef enum { DC_NOTE, DC_WARN, DC_ERR, DC_INT, DC_COUNT } DiagCat;

/* Descriptions for diagnostic categories */
const char* DiagCatDesc[DC_COUNT] = {
    "Note", "Warning", "Error", "Internal error"
};

/* An empty file position used when diagnostics aren't related to a file */
static FilePos  NoFile = STATIC_FILEPOS_INITIALIZER;

/* Notifications. They are remembered here and output with the next call to
** Error() or Warning().
*/
static Collection Notes = STATIC_COLLECTION_INITIALIZER;



/*****************************************************************************/
/*                                   Code                                    */
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
*/
{
    /* UTF-8 characters for single quotes */
    static const char QuoteStart[] = "\xE2\x80\x98";
    static const char QuoteEnd[]   = "\xE2\x80\x99";

    /* ANSI color sequences */
    const char* ColorStart = CP_BrightGreen ();
    const char* ColorEnd   = CP_White ();

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
                if (!InQuote) {
                    InQuote = 1;
                    if (IsUTF8) {
                        SB_AppendStr (&T, QuoteStart);
                    } else {
                        /* ca65 uses \' for opening and closing quotes */
                        SB_AppendChar (&T, '\'');
                    }
                    SB_AppendStr (&T, ColorStart);
                } else {
                    /* Found two ` without closing quote - don't replace */
                    SB_AppendChar (&T, '`');
                }
                break;
            case '\'':
                if (InQuote) {
                    InQuote = 0;
                    SB_AppendStr (&T, ColorEnd);
                    if (IsUTF8) {
                        SB_AppendStr (&T, QuoteEnd);
                    } else {
                        SB_AppendChar (&T, C);
                    }
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



static void VPrepMsg (StrBuf* S, const FilePos* Pos, DiagCat Cat,
                      const char* Format, va_list ap)
/* Prepare an error/warning/notification message in S. */
{
    StrBuf Msg = AUTO_STRBUF_INITIALIZER;
    StrBuf Loc = AUTO_STRBUF_INITIALIZER;

    /* Determine the description for the category and its color */
    const char* Desc = DiagCatDesc[Cat];
    const char* Color;
    switch (Cat) {
        case DC_NOTE:   Color = CP_Cyan ();             break;
        case DC_WARN:   Color = CP_Yellow ();           break;
        case DC_ERR:    Color = CP_BrightRed ();        break;
        case DC_INT:    Color = CP_BrightRed ();        break;
        default:        FAIL ("Unexpected Cat value");  break;
    }

    /* Format the actual message, then replace quotes */
    SB_VPrintf (&Msg, Format, ap);
    ReplaceQuotes (&Msg);

    /* Format the location. If the file position is valid, we use the file
    ** position, otherwise the program name. This allows to print fatal
    ** errors in the startup phase.
    */
    if (Pos->Name == INVALID_STRING_ID) {
        SB_CopyStr (&Loc, ProgName);
    } else {
        SB_Printf (&Loc, "%s:%u", GetString (Pos->Name), Pos->Line);
    }
    SB_Terminate (&Loc);

    /* Format the full message */
    SB_Printf (S, "%s%s: %s%s:%s %s%s",
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
    SB_AppendChar (S, '\n');
    SB_Terminate (S);
}



static void VPrintMsg (const FilePos* Pos, DiagCat Cat, const char* Format,
                       va_list ap)
/* Format and output an error/warning message. */
{
    /* Format the message */
    StrBuf S = AUTO_STRBUF_INITIALIZER;
    VPrepMsg (&S, Pos, Cat, Format, ap);

    /* Output the full message */
    fputs (SB_GetConstBuf (&S), stderr);

    /* Delete the buffer for the full message */
    SB_Done (&S);
}



static void OutputNotes (void)
/* Output all stored notification messages, then delete them */
{
    unsigned I;
    for (I = 0; I < CollCount (&Notes); ++I) {
        StrBuf* S = CollAtUnchecked (&Notes, I);
        fputs (SB_GetConstBuf (S), stderr);
        FreeStrBuf (S);
    }
    CollDeleteAll (&Notes);
}



void AddNote (const char* Format, ...)
/* Add a notification message that will be output after the next error or
** warning. There cannot be a Notification() function since Error() will
** always terminate.
*/
{
    va_list ap;

    /* Create a new string buffer and add it to the notes */
    StrBuf* S = NewStrBuf ();
    CollAppend (&Notes, S);

    /* Create the message in the string buffer */
    va_start (ap, Format);
    VPrepMsg (S, &NoFile, DC_NOTE, Format, ap);
    va_end (ap);
}



void Warning (const char* Format, ...)
/* Print a warning message */
{
    /* Output the message */
    va_list ap;
    va_start (ap, Format);
    VPrintMsg (&NoFile, DC_WARN, Format, ap);
    va_end (ap);

    /* Output all stored notes */
    OutputNotes ();

    /* Count warnings */
    ++WarningCount;
}



void Error (const char* Format, ...)
/* Print an error message and die */
{
    /* Output the message */
    va_list ap;
    va_start (ap, Format);
    VPrintMsg (&NoFile, DC_ERR, Format, ap);
    va_end (ap);

    /* Output all stored notes */
    OutputNotes ();

    /* Terminate after errors */
    exit (EXIT_FAILURE);
}



void Internal (const char* Format, ...)
/* Print an internal error message and die */
{
    /* Output the message */
    va_list ap;
    va_start (ap, Format);
    VPrintMsg (&NoFile, DC_INT, Format, ap);
    va_end (ap);

    /* Terminate after errors */
    exit (EXIT_FAILURE);
}



void AddCfgNote (const FilePos* Pos, const char* Format, ...)
/* Add a notifcation message using file name and line number of the config file.
** See comment for AddNote() above.
*/
{
    va_list ap;

    /* Create a new string buffer and add it to the notes */
    StrBuf* S = NewStrBuf ();
    CollAppend (&Notes, S);

    /* Create the message in the string buffer */
    va_start (ap, Format);
    VPrepMsg (S, Pos, DC_NOTE, Format, ap);
    va_end (ap);
}



void PWarning (const FilePos* Pos, const char* Format, ...)
/* Print a warning message adding file name and line number of a given file */
{
    /* Output the message */
    va_list ap;
    va_start (ap, Format);
    VPrintMsg (Pos, DC_WARN, Format, ap);
    va_end (ap);

    /* Output all stored notes */
    OutputNotes ();

    /* Count warnings */
    ++WarningCount;
}



void PError (const FilePos* Pos, const char* Format, ...)
/* Print an error message adding file name and line number of a given file */
{
    /* Output the message */
    va_list ap;
    va_start (ap, Format);
    VPrintMsg (Pos, DC_ERR, Format, ap);
    va_end (ap);

    /* Output all stored notes */
    OutputNotes ();

    /* Terminate after errors */
    exit (EXIT_FAILURE);
}
