/*****************************************************************************/
/*                                                                           */
/*  				    error.c				     */
/*                                                                           */
/*  		  Error handling for the ca65 macroassembler		     */
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
#include "strbuf.h"

/* ca65 */
#include "error.h"
#include "filetab.h"
#include "nexttok.h"



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

        StrBuf S = STATIC_STRBUF_INITIALIZER;
        SB_VPrintf (&S, Format, ap);
        SB_Terminate (&S);

       	fprintf (stderr, "%s(%lu): Warning: %s\n",
                 SB_GetConstBuf (GetFileName (Pos->Name)),
                 Pos->Line,
                 SB_GetConstBuf (&S));
	++WarningCount;

        SB_Done (&S);
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
    StrBuf S = STATIC_STRBUF_INITIALIZER;
    SB_VPrintf (&S, Format, ap);
    SB_Terminate (&S);

    fprintf (stderr, "%s(%lu): Error: %s\n",
             SB_GetConstBuf (GetFileName (Pos->Name)),
             Pos->Line,
             SB_GetConstBuf (&S));
    ++ErrorCount;

    SB_Done (&S);
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



