/*****************************************************************************/
/*                                                                           */
/*                                  error.h                                  */
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



#ifndef ERROR_H
#define ERROR_H



#include <stdio.h>

/* common */
#include "attrib.h"
#include "intstack.h"

/* cc65 */
#include "lineinfo.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Count of errors/warnings */
extern unsigned ErrorCount;
extern unsigned WarningCount;

/* Warning and error options */
extern IntStack WarnEnable;             /* Enable warnings */
extern IntStack WarningsAreErrors;      /* Treat warnings as errors */
                                        /* Warn about: */
extern IntStack WarnConstComparison;    /* - constant comparison results */
extern IntStack WarnPointerSign;        /* - pointer conversion to pointer differing in signedness */
extern IntStack WarnPointerTypes;       /* - pointer conversion to incompatible pointer type */
extern IntStack WarnNoEffect;           /* - statements without an effect */
extern IntStack WarnRemapZero;          /* - remapping character code zero */
extern IntStack WarnReturnType;         /* - control reaches end of non-void function */
extern IntStack WarnStructParam;        /* - structs passed by val */
extern IntStack WarnUnknownPragma;      /* - unknown #pragmas */
extern IntStack WarnUnreachableCode;    /* - unreachable code */
extern IntStack WarnUnusedLabel;        /* - unused labels */
extern IntStack WarnUnusedParam;        /* - unused parameters */
extern IntStack WarnUnusedVar;          /* - unused variables */
extern IntStack WarnUnusedFunc;         /* - unused functions */
extern IntStack WarnConstOverflow;      /* - overflow conversion of numerical constants */

/* Forward */
struct StrBuf;



/*****************************************************************************/
/*                                   code                                    */
/*****************************************************************************/



void Fatal (const char* Format, ...) attribute ((noreturn, format (printf, 1, 2)));
/* Print a message about a fatal error and die */

void Internal (const char* Format, ...) attribute ((noreturn, format (printf, 1, 2)));
/* Print a message about an internal compiler error and die */

void Error (const char* Format, ...) attribute ((format (printf, 1, 2)));
/* Print an error message */

void LIError (const LineInfo* LI, const char* Format, ...) attribute ((format (printf, 2, 3)));
/* Print an error message with the line info given explicitly */

void PPError (const char* Format, ...) attribute ((format (printf, 1, 2)));
/* Print an error message. For use within the preprocessor */

void Warning (const char* Format, ...) attribute ((format (printf, 1, 2)));
/* Print a warning message */

void LIWarning (const LineInfo* LI, const char* Format, ...) attribute ((format (printf, 2, 3)));
/* Print a warning message with the line info given explicitly */

void PPWarning (const char* Format, ...) attribute ((format (printf, 1, 2)));
/* Print a warning message. For use within the preprocessor */

IntStack* FindWarning (const char* Name);
/* Search for a warning in the WarnMap table and return a pointer to the
** intstack that holds its state. Return NULL if there is no such warning.
*/

void ListWarnings (FILE* F);
/* Print a list of warning types/names to the given file */

void Note (const char* Format, ...) attribute ((format (printf, 1, 2)));
/* Print a note message */

void LINote (const LineInfo* LI, const char* Format, ...) attribute ((format (printf, 2, 3)));
/* Print a note message with the line info given explicitly */

void PPNote (const char* Format, ...) attribute ((format (printf, 1, 2)));
/* Print a note message. For use within the preprocessor */

void ErrorReport (void);
/* Report errors (called at end of compile) */

void InitDiagnosticStrBufs (void);
/* Init tracking string buffers used for diagnostics */

void DoneDiagnosticStrBufs (void);
/* Done with tracked string buffers used for diagnostics */

void ClearDiagnosticStrBufs (void);
/* Free all tracked string buffers */

struct StrBuf* NewDiagnosticStrBuf (void);
/* Get a new tracked string buffer */



/* End of error.h */

#endif
