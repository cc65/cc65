/*****************************************************************************/
/*                                                                           */
/*  				    error.c				     */
/*                                                                           */
/*  		  Error handling for the ca65 macroassembler		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2003 Ullrich von Bassewitz                                       */
/*               Römerstraße 52                                              */
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
unsigned WarnLevel	= 1;

/* Statistics */
unsigned ErrorCount	= 0;
unsigned WarningCount	= 0;



/*****************************************************************************/
/*				   Warnings 				     */
/*****************************************************************************/



void WarningMsg (const FilePos* Pos, unsigned Level, const char* Format, va_list ap)
/* Print warning message. */
{
    if (Level <= WarnLevel) {
	fprintf (stderr, "%s(%lu): Warning: ",
		 GetFileName (Pos->Name), Pos->Line);
	vfprintf (stderr, Format, ap);
	fprintf (stderr, "\n");
	++WarningCount;
    }
}



void Warning (unsigned Level, const char* Format, ...)
/* Print warning message. */
{
    va_list ap;
    va_start (ap, Format);
    WarningMsg (&CurPos, Level, Format, ap);
    va_end (ap);
}



void PWarning (const FilePos* Pos, unsigned Level, const char* Format, ...)
/* Print warning message giving an explicit file and position. */
{
    va_list ap;
    va_start (ap, Format);
    WarningMsg (Pos, Level, Format, ap);
    va_end (ap);
}



/*****************************************************************************/
/*		     		    Errors				     */
/*****************************************************************************/



void ErrorMsg (const FilePos* Pos, const char* Format, va_list ap)
/* Print an error message */
{
    fprintf (stderr, "%s(%lu): Error: ",
	     GetFileName (Pos->Name), Pos->Line);
    vfprintf (stderr, Format, ap);
    fprintf (stderr, "\n");
    ++ErrorCount;
}



void Error (const char* Format, ...)
/* Print an error message */
{
    va_list ap;
    va_start (ap, Format);
    ErrorMsg (&CurPos, Format, ap);
    va_end (ap);
}



void PError (const FilePos* Pos, const char* Format, ...)
/* Print an error message giving an explicit file and position. */
{
    va_list ap;
    va_start (ap, Format);
    ErrorMsg (Pos, Format, ap);
    va_end (ap);
}



void ErrorSkip (const char* Format, ...)
/* Print an error message and skip the rest of the line */
{
    va_list ap;
    va_start (ap, Format);
    ErrorMsg (&CurPos, Format, ap);
    va_end (ap);

    SkipUntilSep ();
}



/*****************************************************************************/
/* 	       	      	    	     Code 		     		     */
/*****************************************************************************/



void Fatal (const char* Format, ...)
/* Print a message about a fatal error and die */
{
    va_list ap;

    va_start (ap, Format);
    fprintf (stderr, "Fatal error: ");
    vfprintf (stderr, Format, ap);
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



