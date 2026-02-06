/*****************************************************************************/
/*                                                                           */
/*                                  error.c                                  */
/*                                                                           */
/*                  Error handling for the cc65 C compiler                   */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2011, Ullrich von Bassewitz                                      */
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
#include "coll.h"
#include "consprop.h"
#include "debugflag.h"
#include "print.h"
#include "strbuf.h"

/* cc65 */
#include "global.h"
#include "input.h"
#include "lineinfo.h"
#include "scanner.h"
#include "stmt.h"
#include "error.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Structure that contains a position in the source file */
typedef struct SourcePos SourcePos;
struct SourcePos {
    const char* File;
    unsigned    Line;
};
#define SOURCEPOS(File, Line) ((SourcePos){ (File), (unsigned)(Line) })

/* Diagnostic category */
typedef enum {
    DC_NOTE, DC_PPWARN, DC_WARN, DC_PPERR, DC_ERR, DC_FATAL, DC_COUNT
} DiagCat;

/* Descriptions for diagnostic categories */
static const char* DiagCatDesc[DC_COUNT] = {
    "Note", "Warning", "Warning", "Error", "Error", "Fatal error"
};

/* Count of errors/warnings */
unsigned PPErrorCount       = 0; /* Pre-parser errors */
unsigned PPWarningCount     = 0; /* Pre-parser warnings */
unsigned ErrorCount         = 0; /* Errors occurred in parser and later translation phases */
unsigned WarningCount       = 0; /* Warnings occurred in parser and later translation phases */

/* Warning and error options */
IntStack WarnEnable         = INTSTACK(1);  /* Enable warnings */
IntStack WarningsAreErrors  = INTSTACK(0);  /* Treat warnings as errors */
                                            /* Warn about: */
IntStack WarnConstComparison= INTSTACK(1);  /* - constant comparison results */
IntStack WarnNoEffect       = INTSTACK(1);  /* - statements without an effect */
IntStack WarnPointerSign    = INTSTACK(1);  /* - pointer conversion to pointer differing in signedness */
IntStack WarnPointerTypes   = INTSTACK(1);  /* - pointer conversion to incompatible pointer type */
IntStack WarnRemapZero      = INTSTACK(1);  /* - remapping character code zero */
IntStack WarnReturnType     = INTSTACK(1);  /* - control reaches end of non-void function */
IntStack WarnStructParam    = INTSTACK(0);  /* - structs passed by val */
IntStack WarnUnknownPragma  = INTSTACK(1);  /* - unknown #pragmas */
IntStack WarnUnreachableCode= INTSTACK(1);  /* - unreachable code */
IntStack WarnUnusedLabel    = INTSTACK(1);  /* - unused labels */
IntStack WarnUnusedParam    = INTSTACK(1);  /* - unused parameters */
IntStack WarnUnusedVar      = INTSTACK(1);  /* - unused variables */
IntStack WarnUnusedFunc     = INTSTACK(1);  /* - unused functions */
IntStack WarnConstOverflow  = INTSTACK(0);  /* - overflow conversion of numerical constants */

/* Map the name of a warning to the intstack that holds its state */
typedef struct WarnMapEntry WarnMapEntry;
struct WarnMapEntry {
    IntStack*   Stack;
    const char* Name;
};
static WarnMapEntry WarnMap[] = {
    /* Keep names sorted, even if it isn't used for now */
    { &WarnConstComparison,     "const-comparison"      },
    { &WarnConstOverflow,       "const-overflow"        },
    { &WarningsAreErrors,       "error"                 },
    { &WarnNoEffect,            "no-effect"             },
    { &WarnPointerSign,         "pointer-sign"          },
    { &WarnPointerTypes,        "pointer-types"         },
    { &WarnRemapZero,           "remap-zero"            },
    { &WarnReturnType,          "return-type"           },
    { &WarnStructParam,         "struct-param"          },
    { &WarnUnknownPragma,       "unknown-pragma"        },
    { &WarnUnreachableCode,     "unreachable-code"      },
    { &WarnUnusedFunc,          "unused-func"           },
    { &WarnUnusedLabel,         "unused-label"          },
    { &WarnUnusedParam,         "unused-param"          },
    { &WarnUnusedVar,           "unused-var"            },
};



/*****************************************************************************/
/*                                  Helpers                                  */
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



static void VPrintMsg (SourcePos SP, const LineInfo* LI, DiagCat Cat,
                       const char* Format, va_list ap)
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
        case DC_PPWARN:
        case DC_WARN:   Color = CP_Yellow ();           break;
        case DC_PPERR:
        case DC_ERR:    Color = CP_BrightRed ();        break;
        case DC_FATAL:  Color = CP_BrightRed ();        break;
        default:        FAIL ("Unexpected Cat value");  break;
    }

    /* Format the actual message, then replace quotes */
    SB_VPrintf (&Msg, Format, ap);
    ReplaceQuotes (&Msg);

    /* Format the location. */
    if (LI == 0) {
        SB_CopyStr (&Loc, ProgName);
    } else {
        SB_Printf (&Loc, "%s:%u", GetPresumedFileName (LI), GetPresumedLineNum (LI));
    }
    SB_Terminate (&Loc);

    /* Format the full message. If debugging is enabled, we also output the
    ** source position from which the output was generated - for whatever it's
    ** worth.
    */
    if (Debug) {
       	SB_Printf (&S, "%s[%s:%u] %s: %s%s:%s %s%s",
	     	   CP_White (),
                   SP.File,
                   SP.Line,
    	     	   SB_GetConstBuf (&Loc),
	     	   Color,
	     	   Desc,
	     	   CP_White (),
	     	   SB_GetConstBuf (&Msg),
	     	   CP_Reset ());
    } else {
	SB_Printf (&S, "%s%s: %s%s:%s %s%s",
	     	   CP_White (),
	     	   SB_GetConstBuf (&Loc),
	     	   Color,
	     	   Desc,
	     	   CP_White (),
	     	   SB_GetConstBuf (&Msg),
	     	   CP_Reset ());
    }

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

    /* Bump error counters */
    switch (Cat) {
        case DC_PPWARN:	++PPWarningCount;       break;
        case DC_WARN:   ++WarningCount;         break;
        case DC_PPERR:  ++PPErrorCount;         break;
        case DC_ERR:    ++ErrorCount;           break;
        default:                                break;
    }
}



static void PrintFileInclusionInfo (const LineInfo* LI)
/* Print hierarchy of file inclusion */
{
    if (LI->IncFiles != 0) {
        unsigned FileCount = CollCount (LI->IncFiles);
        if (FileCount > 0) {
            const char* Str = "In file included from %s:%u%c\n";

            while (FileCount-- > 0) {
                LineInfoFile* LIF = CollAtUnchecked (LI->IncFiles, FileCount);
                char C = FileCount > 0 ? ',' : ':';

                fprintf (stderr, Str, LIF->Name, LIF->LineNum, C);
                Str = "                 from %s:%u%c\n";
            }
        }
    }
}



LineInfo* GetDiagnosticLI (void)
/* Get the line info where the diagnostic info refers to */
{
    if (CurTok.LI) {
        return CurTok.LI;
    } else {
        return GetCurLineInfo ();
    }
}



static const char* GetDiagnosticFileName (void)
/* Get the source file name where the diagnostic info refers to */
{
    if (CurTok.LI) {
        return GetPresumedFileName (CurTok.LI);
    } else {
        return GetCurrentFileName ();
    }
}



static unsigned GetDiagnosticLineNum (void)
/* Get the source line number where the diagnostic info refers to */
{
    if (CurTok.LI) {
        return GetPresumedLineNum (CurTok.LI);
    } else {
        return GetCurrentLineNum ();
    }
}



/*****************************************************************************/
/*                         Handling of fatal errors                          */
/*****************************************************************************/



void Fatal_ (const char* File, int Line, const char* Format, ...)
/* Print a message about a fatal error and die */
{
    SourcePos SP = SOURCEPOS (File, Line);

    va_list ap;
    va_start (ap, Format);
    VPrintMsg (SP, CurTok.LI, DC_FATAL, Format, ap);
    va_end (ap);

    exit (EXIT_FAILURE);
}



void Internal_ (const char* File, int LineNo, const char* Format, ...)
/* Print a message about an internal compiler error and die */
{
    va_list ap;

    if (Debug) {
        fprintf(stderr, "[%s:%d] ", File, LineNo);
    }

    fprintf (stderr, "%s:%u: Internal compiler error:\n",
             GetDiagnosticFileName (), GetDiagnosticLineNum ());

    va_start (ap, Format);
    vfprintf (stderr, Format, ap);
    va_end (ap);
    fprintf (stderr, "\n");

    if (Line) {
        fprintf (stderr, "\nInput: %.*s\n", (int) SB_GetLen (Line), SB_GetConstBuf (Line));
    }

    /* Use abort to create a core dump */
    abort ();
}



/*****************************************************************************/
/*                            Handling of errors                             */
/*****************************************************************************/



static void IntError (SourcePos SP, DiagCat Cat, LineInfo* LI,
                      const char* Msg, va_list ap)
/* Print an error message - internal function */
{
    static unsigned RecentErrorCount = 0;
    static unsigned RecentLine       = 0;

    unsigned Line = GetPresumedLineNum (LI);

    /* Print file inclusion if appropriate */
    if (HasFileInclusionChanged (LI)) {
        PrintFileInclusionInfo (LI);
    }
    RememberCheckedLI (LI);

    /* Output the formatted message */
    VPrintMsg (SP, LI, Cat, Msg, ap);

    /* Limit total errors and total errors per line */
    if (RecentLine != Line) {
        RecentLine = Line;
        RecentErrorCount = 0;
    } else {
        ++RecentErrorCount;
    }
    if (RecentErrorCount > 20 || GetTotalErrors () > 200) {
        Fatal ("Too many errors");
    }
}



void Error_ (const char* File, int Line, const char* Format, ...)
/* Print an error message */
{
    va_list ap;
    va_start (ap, Format);
    IntError (SOURCEPOS (File, Line), DC_ERR, GetDiagnosticLI (), Format, ap);
    va_end (ap);
}



void PPError_ (const char* File, int Line, const char* Format, ...)
/* Print an error message. For use within the preprocessor */
{
    va_list ap;
    va_start (ap, Format);
    IntError (SOURCEPOS (File, Line), DC_PPERR, GetCurLineInfo (), Format, ap);
    va_end (ap);
}



/*****************************************************************************/
/*                           Handling of warnings                            */
/*****************************************************************************/



static void IntWarning (SourcePos SP, DiagCat Cat, LineInfo* LI,
                        const char* Msg, va_list ap)
/* Print a warning message - internal function */
{
    if (IS_Get (&WarningsAreErrors)) {

        /* Treat the warning as an error */
        IntError (SP, Cat, LI, Msg, ap);

    } else if (IS_Get (&WarnEnable)) {

        /* Print file inclusion if appropriate */
        if (HasFileInclusionChanged (LI)) {
            PrintFileInclusionInfo (LI);
        }
        RememberCheckedLI (LI);

    	/* Output the formatted message */
       	VPrintMsg (SP, LI, Cat, Msg, ap);
    }
}



void Warning_ (const char* File, int Line, const char* Format, ...)
/* Print a warning message */
{
    va_list ap;
    va_start (ap, Format);
    IntWarning (SOURCEPOS (File, Line), DC_WARN, GetDiagnosticLI (), Format, ap);
    va_end (ap);
}



void PPWarning_ (const char* File, int Line, const char* Format, ...)
/* Print a warning message. For use within the preprocessor */
{
    va_list ap;
    va_start (ap, Format);
    IntWarning (SOURCEPOS (File, Line), DC_PPWARN, GetCurLineInfo (), Format, ap);
    va_end (ap);
}



static void LIWarning_ (const char* File, int Line, LineInfo* LI, const char* Format, ...)
/* Print a warning message specifying an explicit line info */
{
    va_list ap;
    va_start (ap, Format);
    IntWarning (SOURCEPOS (File, Line), DC_WARN, LI, Format, ap);
    va_end (ap);
}



void LIUnreachableCodeWarning (LineInfo* LI)
/* Print a warning about unreachable code at the given location if these
** warnings are enabled.
*/
{
    if (IS_Get (&WarnUnreachableCode)) {
        LIWarning_ (__FILE__, __LINE__, LI, "Unreachable code");
    }
}



void UnreachableCodeWarning (void)
/* Print a warning about unreachable code at the current location if these
** warnings are enabled.
*/
{
    LineInfo* LI;

    /* Add special handling for compound statements if the current token
    ** is from the source. Doing this here is a bit hacky but unfortunately
    ** there's no better place.
    */
    if (CurTok.LI && NextTok.LI) {
        if (CurTok.Tok == TOK_LCURLY) {
            /* Do not point to the compound statement but to the first
            ** statement within it. If the compound statement is empty
            ** do not even output a warning. This fails of course for
            ** nested compounds but will do the right thing in most cases.
            */
            if (NextTok.Tok == TOK_RCURLY) {
                return;
            }
            LI = NextTok.LI;
        } else {
            LI = CurTok.LI;
        }
    } else {
        LI = GetCurLineInfo ();
    }

    /* Now output the warning */
    LIUnreachableCodeWarning (LI);
}



IntStack* FindWarning (const char* Name)
/* Search for a warning in the WarnMap table and return a pointer to the
** intstack that holds its state. Return NULL if there is no such warning.
*/
{
    unsigned I;

    /* For now, do a linear search */
    for (I = 0; I < sizeof(WarnMap) / sizeof (WarnMap[0]); ++I) {
        if (strcmp (WarnMap[I].Name, Name) == 0) {
            return WarnMap[I].Stack;
        }
    }
    return 0;
}



void ListWarnings (FILE* F)
/* Print a list of warning types/names to the given file */
{
    unsigned I;
    for (I = 0; I < sizeof(WarnMap) / sizeof (WarnMap[0]); ++I) {
        fprintf (F, "%s\n", WarnMap[I].Name);
    }
}



/*****************************************************************************/
/*                          Handling of other infos                          */
/*****************************************************************************/



void LINotification_ (const char* File, int Line, const LineInfo* LI,
                      const char* Format, ...)
/* Print a notification message with the line info given explicitly */
{
    va_list ap;
    va_start (ap, Format);
    VPrintMsg (SOURCEPOS (File, Line), LI, DC_NOTE, Format, ap);
    va_end (ap);
}



void Notification_ (const char* File, int Line, const char* Format, ...)
/* Print a notification message */
{
    va_list ap;
    va_start (ap, Format);
    VPrintMsg (SOURCEPOS (File, Line), GetDiagnosticLI (), DC_NOTE, Format, ap);
    va_end (ap);
}



void PPNotification_ (const char* File, int Line, const char* Format, ...)
/* Print a notification message. For use within the preprocessor */
{
    va_list ap;
    va_start (ap, Format);
    VPrintMsg (SOURCEPOS (File, Line), GetDiagnosticLI (), DC_NOTE, Format, ap);
    va_end (ap);
}



/*****************************************************************************/
/*                               Error summary                               */
/*****************************************************************************/



unsigned GetTotalErrors (void)
/* Get total count of errors of all categories */
{
    return PPErrorCount + ErrorCount;
}



unsigned GetTotalWarnings (void)
/* Get total count of warnings of all categories */
{
    return PPWarningCount + WarningCount;
}



void ErrorReport (void)
/* Report errors (called at end of compile) */
{
    unsigned TotalErrors = GetTotalErrors ();
    unsigned TotalWarnings = GetTotalWarnings ();
    unsigned int V = (TotalErrors != 0 ? 0 : 1);
    Print (stdout, V, "%u errors and %u warnings generated.\n", TotalErrors, TotalWarnings);
}



