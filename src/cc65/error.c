/*****************************************************************************/
/*                                                                           */
/*				    error.c				     */
/*                                                                           */
/*		    Error handling for the cc65 C compiler		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2000 Ullrich von Bassewitz                                       */
/*               Wacholderweg 14                                             */
/*               D-70597 Stuttgart                                           */
/* EMail:        uz@musoftware.de                                            */
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

#include "global.h"
#include "input.h"
#include "scanner.h"
#include "stmt.h"
#include "error.h"



/*****************************************************************************/
/*	  			     Data  		     		     */
/*****************************************************************************/



/* Error messages sorted by ErrTypes */
static char* ErrMsg [ERR_COUNT-1] = {
    "Syntax error",
    "`\"' expected",
    "`:' expected",
    "`;' expected",
    "`,' expected",
    "`(' expected",
    "`)' expected",
    "`[' expected",
    "`]' expected",
    "`{' expected",
    "`}' expected",
    "Identifier expected",
    "Type expected",
    "Incompatible types",
    "Incompatible pointer types",
    "Too many arguments in function call",
    "Too few arguments in function call",
    "Duplicate macro parameter: %s",
    "Variable identifier expected",
    "Integer expression expected",
    "Constant expression expected",
    "No active loop",
    "Redefinition of `%s'",
    "Conflicting types for `%s'",
    "String literal expected",
    "`while' expected",
    "Function must return a value",
    "Function cannot return a value",
    "Unexpected `continue'",
    "Undefined symbol: `%s'",
    "Undefined label: `%s'",
    "Too many local variables",
    "Too many initializers",
    "Cannot initialize incomplete type",
    "Cannot subscript",
    "Operation not allowed with this type of argument",
    "Struct expected",
    "Struct/union has no field named `%s'",
    "Struct pointer expected",
    "lvalue expected",
    "Expression expected",
    "Preprocessor expression expected",
    "Illegal type",
    "Illegal function call",
    "Illegal indirection",
    "Illegal address",
    "Illegal hex digit",
    "Illegal character constant",
    "Illegal modifier",
    "Illegal type qualifier",
    "Illegal storage class",
    "Illegal attribute",
    "Illegal segment name: `%s'",
    "Division by zero",
    "Modulo operation with zero",
    "Range error",
    "Symbol is already different kind",
    "Too many lexical levels",
    "Parameter name omitted",
    "Old style function decl used as prototype",
    "Declaration for parameter `%s' but no such parameter",
    "Cannot take address of a register variable",
    "Illegal size of data type",
    "__fastcall__ is not allowed for C functions",
    "Variable has unknown size",
    "Unknown identifier: `%s'",
    "Duplicate qualifier: `%s'",
    "Assignment to const",
    "Pointer types differ in type qualifiers",
};



/* Count of errors/warnings */
unsigned ErrorCount	= 0;
unsigned WarningCount	= 0;



/*****************************************************************************/
/*	    	       		     Code		     		     */
/*****************************************************************************/



static void IntWarning (const char* Filename, unsigned Line, const char* Msg, va_list ap)
/* Print warning message - internal function. */
{
    if (!NoWarn) {
       	fprintf (stderr, "%s(%u): Warning: ", Filename, Line);
     	vfprintf (stderr, Msg, ap);
     	fprintf (stderr, "\n");

     	if (Verbose) {
     	    fprintf (stderr, "Line: %s\n", line);
     	}
	++WarningCount;
    }
}



void Warning (const char* Format, ...)
/* Print warning message. */
{
    va_list ap;
    va_start (ap, Format);
    IntWarning (GetCurrentFile(), curpos, Format, ap);
    va_end (ap);
}



void PPWarning (const char* Format, ...)
/* Print warning message. For use within the preprocessor. */
{
    va_list ap;
    va_start (ap, Format);
    IntWarning (GetCurrentFile(), GetCurrentLine(), Format, ap);
    va_end (ap);
}



static void IntError (const char* Filename, unsigned Line, const char* Msg, va_list ap)
/* Print an error message - internal function*/
{
    fprintf (stderr, "%s(%u): Error: ", Filename, Line);
    vfprintf (stderr, Msg, ap);
    fprintf (stderr, "\n");

    if (Verbose) {
       	fprintf (stderr, "Line: %s\n", line);
    }
    ++ErrorCount;
    if (ErrorCount > 10) {
       	Fatal ("Too many errors");
    }
}



void Error (unsigned ErrNum, ...)
/* Print an error message */
{
    va_list ap;
    va_start (ap, ErrNum);
    IntError (GetCurrentFile(), curpos, ErrMsg [ErrNum-1], ap);
    va_end (ap);
}



void MError (const char* Format, ...)
/* Print an error message */
{
    va_list ap;
    va_start (ap, Format);
    IntError (GetCurrentFile(), curpos, Format, ap);
    va_end (ap);
}



void PPError (const char* Format, ...)
/* Print an error message. For use within the preprocessor.  */
{
    va_list ap;
    va_start (ap, Format);
    IntError (GetCurrentFile(), GetCurrentLine(), Format, ap);
    va_end (ap);
}



void Fatal (const char* Format, ...)
/* Print a message about a fatal error and die */
{
    va_list ap;

    fprintf (stderr, "%s(%u): Fatal: ", GetCurrentFile(), curpos);

    va_start (ap, Format);
    vfprintf (stderr, Format, ap);
    va_end (ap);
    fprintf (stderr, "\n");

    if (Verbose) {
       	fprintf (stderr, "Line: %s\n", line);
    }
    exit (EXIT_FAILURE);
}



void Internal (char* Format, ...)
/* Print a message about an internal compiler error and die. */
{
    va_list ap;

    fprintf (stderr, "%s(%u): Internal compiler error:\n",
	     GetCurrentFile(), curpos);

    va_start (ap, Format);
    vfprintf (stderr, Format, ap);
    va_end (ap);
    fprintf (stderr, "\nLine: %s\n", line);

    /* Use abort to create a core dump */
    abort ();
}



void ErrorReport (void)
/* Report errors (called at end of compile) */
{
    if (ErrorCount == 0 && Verbose) {
     	printf ("No errors.\n");
    }
}




