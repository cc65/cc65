/*****************************************************************************/
/*                                                                           */
/*  				    error.c				     */
/*                                                                           */
/*  		  Error handling for the ca65 macroassembler		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2003 Ullrich von Bassewitz                                       */
/*               Römerstrasse 52                                             */
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

/* ca65 */
#include "filetab.h"
#include "nexttok.h"
#include "error.h"



/*****************************************************************************/
/*	  			     Data		     		     */
/*****************************************************************************/



/* Warning level */
unsigned WarnLevel	    = 1;

/* Statistics */
unsigned ErrorCount	= 0;
unsigned WarningCount	= 0;



/*****************************************************************************/
/*				   Warnings 				     */
/*****************************************************************************/



void WarningMsg (const FilePos* Pos, unsigned WarnNum, va_list ap)
/* Print warning message. */
{
    static const struct {
	unsigned char 	Level;
	const char*	Msg;
    } Warnings [WARN_COUNT-1] = {
       	{   2,  "Symbol `%s' is defined but never used" 	},
        {   2,  "Symbol `%s' is imported but never used"	},
	{   1,  "Cannot track processor status byte"		},
       	{   1,  "Suspicious address expression"                 },
       	{   0,	"User warning: %s"				},
    };

    if (Warnings [WarnNum-1].Level <= WarnLevel) {
	fprintf (stderr, "%s(%lu): Warning #%u: ",
		 GetFileName (Pos->Name), Pos->Line, WarnNum);
	vfprintf (stderr, Warnings [WarnNum-1].Msg, ap);
	fprintf (stderr, "\n");
	++WarningCount;
    }
}



void Warning (unsigned WarnNum, ...)
/* Print warning message. */
{
    va_list ap;
    va_start (ap, WarnNum);
    WarningMsg (&CurPos, WarnNum, ap);
    va_end (ap);
}



void PWarning (const FilePos* Pos, unsigned WarnNum, ...)
/* Print warning message giving an explicit file and position. */
{
    va_list ap;
    va_start (ap, WarnNum);
    WarningMsg (Pos, WarnNum, ap);
    va_end (ap);
}



/*****************************************************************************/
/*		     		    Errors				     */
/*****************************************************************************/



void ErrorMsg (const FilePos* Pos, unsigned ErrNum, va_list ap)
/* Print an error message */
{
    static const char* Msgs [ERR_COUNT-1] = {
     	"Command/operation not implemented",
     	"Cannot open include file `%s': %s",
	"Cannot read from include file `%s': %s",
     	"Include nesting too deep",
        "Invalid input character: %02X",
	"Hex digit expected",
	"Digit expected",
	"`0' or `1' expected",
	"Numerical overflow",
        "Control statement expected",
	"Too many characters",
	"`:' expected",
       	"`(' expected",
	"`)' expected",
	"`]' expected",
	"`,' expected",
        "Boolean switch value expected (on/off/+/-)",
	"`Y' expected",
	"`X' expected",
	"Integer constant expected",
	"String constant expected",
	"Character constant expected",
	"Constant expression expected",
	"Identifier expected",
	"`.ENDMACRO' expected",
	"Option key expected",
	"`=' expected",
	"Command is only valid in 65816 mode",
	"User error: %s",
	"String constant too long",
	"Newline in string constant",
	"Illegal character constant",
	"Illegal addressing mode",
 	"Illegal character to start local symbols",
	"Illegal use of local symbol",
	"Illegal segment name: `%s'",
     	"Illegal segment attribute",
     	"Illegal macro package name",
     	"Illegal emulation feature",
        "Illegal scope specifier",
        "Illegal assert action",
	"Syntax error",
 	"Symbol `%s' is already defined",
	"Undefined symbol `%s'",
	"Symbol `%s' is already marked as import",
        "Symbol `%s' is already marked as export",
	"Exported symbol `%s' is undefined",
	"Exported values must be constant",
	"Unexpected end of file",
	"Unexpected end of line",
	"Unexpected `%s'",
	"Division by zero",
	"Modulo operation with zero",
        "Range error",
	"Too many macro parameters",
	"Macro parameter expected",
	"Circular reference in symbol definition",
       	"Symbol `%s' redeclaration mismatch",
        "Alignment value must be a power of 2",
     	"Duplicate `.ELSE'",
       	"Conditional assembly branch was never closed",
	"Lexical level was not terminated correctly",
        "No open lexical level",
	"Segment attribute mismatch",
        "Segment stack overflow",
        "Segment stack is empty",
        "Segment stack is not empty at end of assembly",
    	"CPU not supported",
	"Counter underflow",
	"Undefined label",
       	"Open `%s'",
	"File name `%s' not found in file table",
    };

    fprintf (stderr, "%s(%lu): Error #%u: ",
	     GetFileName (Pos->Name), Pos->Line, ErrNum);
    vfprintf (stderr, Msgs [ErrNum-1], ap);
    fprintf (stderr, "\n");
    ++ErrorCount;
}



void Error (unsigned ErrNum, ...)
/* Print an error message */
{
    va_list ap;
    va_start (ap, ErrNum);
    ErrorMsg (&CurPos, ErrNum, ap);
    va_end (ap);
}



void PError (const FilePos* Pos, unsigned ErrNum, ...)
/* Print an error message giving an explicit file and position. */
{
    va_list ap;
    va_start (ap, ErrNum);
    ErrorMsg (Pos, ErrNum, ap);
    va_end (ap);
}



void ErrorSkip (unsigned ErrNum, ...)
/* Print an error message and skip the rest of the line */
{
    va_list ap;
    va_start (ap, ErrNum);
    ErrorMsg (&CurPos, ErrNum, ap);
    va_end (ap);

    SkipUntilSep ();
}



/*****************************************************************************/
/* 	       	      	    	     Code 		     		     */
/*****************************************************************************/



void Fatal (unsigned FatNum, ...)
/* Print a message about a fatal error and die */
{
    static const char* Msgs [FAT_COUNT-1] = {
	"Maximum number of input files reached",
	"Out of memory",
	"Too many segments",
	"String too long",
	"Cannot open input file `%s': %s",
	"Cannot stat input file `%s': %s",
	"Cannot open output file `%s': %s",
	"Cannot write to output file `%s': %s",
	"Cannot open listing file: %s",
	"Cannot write to listing file: %s",
	"Cannot read from listing file: %s",
	"Too many nested constructs",
	".IF nesting too deep",
	"Too many symbols",
    };
    va_list ap;

    va_start (ap, FatNum);
    fprintf (stderr, "Fatal #%u: ", FatNum);
    vfprintf (stderr, Msgs [FatNum-1], ap);
    fprintf (stderr, "\n");
    va_end (ap);

    /* And die... */
    exit (EXIT_FAILURE);
}



void Internal (const char* Format, ...)
/* Print a message about an internal compiler error and die. */
{
    va_list ap;
    va_start (ap, Format);
    fprintf (stderr, "Internal assembler error\n");
    vfprintf (stderr, Format, ap);
    va_end (ap);
    fprintf (stderr, "\n");

    exit (EXIT_FAILURE);
}



