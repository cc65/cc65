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
#include "coll.h"
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



/* Count of errors/warnings */
unsigned ErrorCount     = 0;
unsigned WarningCount   = 0;

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
    { &WarnConstOverflow,       "const-overflow"        },
};

Collection DiagnosticStrBufs;



/*****************************************************************************/
/*                                  Helpers                                  */
/*****************************************************************************/



static const char* GetDiagnosticFileName (void)
/* Get the source file name where the diagnostic info refers to */
{
    if (CurTok.LI) {
        return GetInputName (CurTok.LI);
    } else {
        return GetCurrentFilename ();
    }
}



static unsigned GetDiagnosticLineNum (void)
/* Get the source line number where the diagnostic info refers to */
{
    if (CurTok.LI) {
        return GetInputLine (CurTok.LI);
    } else {
        return GetCurrentLineNum ();
    }
}



/*****************************************************************************/
/*                         Handling of fatal errors                          */
/*****************************************************************************/



void Fatal (const char* Format, ...)
/* Print a message about a fatal error and die */
{
    va_list ap;

    fprintf (stderr, "%s:%u: Fatal: ", GetDiagnosticFileName (), GetDiagnosticLineNum ());

    va_start (ap, Format);
    vfprintf (stderr, Format, ap);
    va_end (ap);
    fprintf (stderr, "\n");

    if (Line) {
        Print (stderr, 1, "Input: %.*s\n", (int) SB_GetLen (Line), SB_GetConstBuf (Line));
    }
    exit (EXIT_FAILURE);
}



void Internal (const char* Format, ...)
/* Print a message about an internal compiler error and die */
{
    va_list ap;

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



static void IntError (const char* Filename, unsigned LineNo, const char* Msg, va_list ap)
/* Print an error message - internal function */
{
    fprintf (stderr, "%s:%u: Error: ", Filename, LineNo);
    vfprintf (stderr, Msg, ap);
    fprintf (stderr, "\n");

    if (Line) {
        Print (stderr, 1, "Input: %.*s\n", (int) SB_GetLen (Line), SB_GetConstBuf (Line));
    }
    ++ErrorCount;
    if (ErrorCount > 20) {
        Fatal ("Too many errors");
    }
}



void Error (const char* Format, ...)
/* Print an error message */
{
    va_list ap;
    va_start (ap, Format);
    IntError (GetDiagnosticFileName (), GetDiagnosticLineNum (), Format, ap);
    va_end (ap);
}



void LIError (const LineInfo* LI, const char* Format, ...)
/* Print an error message with the line info given explicitly */
{
    va_list ap;
    va_start (ap, Format);
    IntError (GetInputName (LI), GetInputLine (LI), Format, ap);
    va_end (ap);
}



void PPError (const char* Format, ...)
/* Print an error message. For use within the preprocessor */
{
    va_list ap;
    va_start (ap, Format);
    IntError (GetCurrentFilename(), GetCurrentLineNum(), Format, ap);
    va_end (ap);
}



/*****************************************************************************/
/*                           Handling of warnings                            */
/*****************************************************************************/



static void IntWarning (const char* Filename, unsigned LineNo, const char* Msg, va_list ap)
/* Print a warning message - internal function */
{
    if (IS_Get (&WarningsAreErrors)) {

        /* Treat the warning as an error */
        IntError (Filename, LineNo, Msg, ap);

    } else if (IS_Get (&WarnEnable)) {

        fprintf (stderr, "%s:%u: Warning: ", Filename, LineNo);
        vfprintf (stderr, Msg, ap);
        fprintf (stderr, "\n");

        if (Line) {
            Print (stderr, 1, "Input: %.*s\n", (int) SB_GetLen (Line), SB_GetConstBuf (Line));
        }
        ++WarningCount;

    }
}



void Warning (const char* Format, ...)
/* Print a warning message */
{
    va_list ap;
    va_start (ap, Format);
    IntWarning (GetDiagnosticFileName (), GetDiagnosticLineNum (), Format, ap);
    va_end (ap);
}



void LIWarning (const LineInfo* LI, const char* Format, ...)
/* Print a warning message with the line info given explicitly */
{
    va_list ap;
    va_start (ap, Format);
    IntWarning (GetInputName (LI), GetInputLine (LI), Format, ap);
    va_end (ap);
}



void PPWarning (const char* Format, ...)
/* Print a warning message. For use within the preprocessor */
{
    va_list ap;
    va_start (ap, Format);
    IntWarning (GetCurrentFilename(), GetCurrentLineNum(), Format, ap);
    va_end (ap);
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



static void IntNote (const char* Filename, unsigned LineNo, const char* Msg, va_list ap)
/* Print a note message - internal function */
{
    fprintf (stderr, "%s:%u: Note: ", Filename, LineNo);
    vfprintf (stderr, Msg, ap);
    fprintf (stderr, "\n");
}



void Note (const char* Format, ...)
/* Print a note message */
{
    va_list ap;
    va_start (ap, Format);
    IntNote (GetDiagnosticFileName (), GetDiagnosticLineNum (), Format, ap);
    va_end (ap);
}



void LINote (const LineInfo* LI, const char* Format, ...)
/* Print a note message with the line info given explicitly */
{
    va_list ap;
    va_start (ap, Format);
    IntNote (GetInputName (LI), GetInputLine (LI), Format, ap);
    va_end (ap);
}



void PPNote (const char* Format, ...)
/* Print a note message. For use within the preprocessor */
{
    va_list ap;
    va_start (ap, Format);
    IntNote (GetCurrentFilename(), GetCurrentLineNum(), Format, ap);
    va_end (ap);
}



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void ErrorReport (void)
/* Report errors (called at end of compile) */
{
    unsigned int V = (ErrorCount != 0 ? 0 : 1);
    Print (stdout, V, "%u errors and %u warnings generated.\n", ErrorCount, WarningCount);
}



/*****************************************************************************/
/*                              Tracked StrBufs                              */
/*****************************************************************************/



void InitDiagnosticStrBufs (void)
/* Init tracking string buffers used for diagnostics */
{
    InitCollection (&DiagnosticStrBufs);
}



void DoneDiagnosticStrBufs (void)
/* Done with tracked string buffers used for diagnostics */
{
    ClearDiagnosticStrBufs ();
    DoneCollection (&DiagnosticStrBufs);
}



void ClearDiagnosticStrBufs (void)
/* Free all tracked string buffers */
{
    unsigned I;

    for (I = 0; I < CollCount (&DiagnosticStrBufs); ++I) {
        SB_Done (CollAtUnchecked (&DiagnosticStrBufs, I));
    }

    CollDeleteAll (&DiagnosticStrBufs);
}



struct StrBuf* NewDiagnosticStrBuf (void)
/* Get a new tracked string buffer */
{
    StrBuf *Buf = NewStrBuf ();
    CollAppend (&DiagnosticStrBufs, Buf);
    return Buf;
}
