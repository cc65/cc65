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



static char* WarnMsg [WARN_COUNT-1] = {
    "Unreachable code",
    "Condition is never true",
    "Condition is always true",
    "Converting pointer to integer without a cast",
    "Converting integer to pointer without a cast",
    "Function call without a prototype",
    "Unknown #pragma",
    "No case labels",
    "Function must be extern",
    "Parameter `%s' is never used",
    "`%s' is defined but never used",
    "Constant is long",
    "`/*' found inside a comment",
    "Useless declaration",
};



/* Error messages sorted by ErrTypes */
static char* ErrMsg [ERR_COUNT-1] = {
    "Invalid character (%u)",
    "Unexpected newline",
    "End-of-file reached in comment starting at line %u",
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
    "Macro argument count mismatch",
    "Duplicate macro parameter: %s",
    "Variable identifier expected",
    "Integer expression expected",
    "Constant expression expected",
    "No active loop",
    "`\"' or `<' expected",
    "Missing terminator or name too long",
    "Include file `%s' not found",
    "Cannot open include file `%s': %s",
    "Invalid #error directive",
    "#error: %s",
    "Unexpected `#endif'",
    "Unexpected `#else'",
    "`#endif' expected",
    "Compiler directive expected",
    "Redefinition of `%s'",
    "Conflicting types for `%s'",
    "String literal expected",
    "`while' expected",
    "Function must return a value",
    "Function cannot return a value",
    "Unexpected `continue'",
    "Undefined symbol: `%s'",
    "Undefined label: `%s'",
    "Include nesting too deep",
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
    "Illegal macro call",
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



static char* FatMsg [FAT_COUNT-1] = {
    "Too many errors",
    "Cannot open output file: %s",
    "Cannot write to output file (disk full?)",
    "Cannot open input file: %s",
    "Out of memory",
    "Stack overflow",
    "Stack empty",
    "Out of string space",
    "Too many case labels",
};



/* Count of errors/warnings */
unsigned ErrorCount	= 0;
unsigned WarningCount	= 0;



/*****************************************************************************/
/*	    	       		     Code		     		     */
/*****************************************************************************/



void Warning (unsigned WarnNum, ...)
/* Print warning message. */
{
    va_list ap;

    if (!NoWarn) {
      	fprintf (stderr, "%s(%u): Warning #%u: ",
		 GetCurrentFile(), curpos, WarnNum);

     	va_start (ap, WarnNum);
     	vfprintf (stderr, WarnMsg [WarnNum-1], ap);
     	va_end (ap);
     	fprintf (stderr, "\n");

     	if (Verbose) {
     	    fprintf (stderr, "Line: %s\n", line);
     	}
    }
    ++ WarningCount;
}



void PPWarning (unsigned WarnNum, ...)
/* Print warning message. For use within the preprocessor. */
{
    va_list ap;

    if (!NoWarn) {
      	fprintf (stderr, "%s(%u): Warning #%u: ",
		 GetCurrentFile(), GetCurrentLine(), WarnNum);

     	va_start (ap, WarnNum);
     	vfprintf (stderr, WarnMsg [WarnNum-1], ap);
     	va_end (ap);
     	fprintf (stderr, "\n");
    }
    ++WarningCount;
}



void Error (unsigned ErrNum, ...)
/* Print an error message */
{
    va_list ap;

    fprintf (stderr, "%s(%u): Error #%u: ",
	     GetCurrentFile(), curpos, ErrNum);

    va_start (ap, ErrNum);
    vfprintf (stderr, ErrMsg [ErrNum-1], ap);
    va_end (ap);
    fprintf (stderr, "\n");

    if (Verbose) {
       	fprintf (stderr, "Line: %s\n", line);
    }
    ++ErrorCount;
    if (ErrorCount > 10) {
       	Fatal (FAT_TOO_MANY_ERRORS);
    }
}



void PPError (unsigned ErrNum, ...)
/* Print an error message. For use within the preprocessor.  */
{
    va_list ap;

    fprintf (stderr, "%s(%u): Error #%u: ",
	     GetCurrentFile(), GetCurrentLine(), ErrNum);

    va_start (ap, ErrNum);
    vfprintf (stderr, ErrMsg [ErrNum-1], ap);
    va_end (ap);
    fprintf (stderr, "\n");

    ++ErrorCount;
    if (ErrorCount > 10) {
       	Fatal (FAT_TOO_MANY_ERRORS);
    }
}



void Fatal (unsigned FatNum, ...)
/* Print a message about a fatal error and die */
{
    va_list ap;

    fprintf (stderr, "%s(%u): Fatal #%u: ",
	     GetCurrentFile(), curpos, FatNum);

    va_start (ap, FatNum);
    vfprintf (stderr, FatMsg [FatNum-1], ap);
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




