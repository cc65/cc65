/*****************************************************************************/
/*                                                                           */
/*  				    error.c				     */
/*                                                                           */
/*  		  Error handling for the ca65 macroassembler		     */
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
#include "strbuf.h"

/* ca65 */
#include "error.h"
#include "filetab.h"
#include "lineinfo.h"
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
/*                             Helper functions                              */
/*****************************************************************************/



static void FormatVMsg (StrBuf* S, const FilePos* Pos, const char* Desc,
                        const char* Format, va_list ap)
/* Format an error/warning message into S. A trailing newline and a NUL
 * terminator will be added to S.
 */
{
    /* Format the actual message */
    StrBuf Msg = STATIC_STRBUF_INITIALIZER;
    SB_VPrintf (&Msg, Format, ap);
    SB_Terminate (&Msg);

    /* Format the message header */
    SB_Printf (S, "%s(%lu): %s: ",
               SB_GetConstBuf (GetFileName (Pos->Name)),
               Pos->Line,
               Desc);

    /* Append the message to the message header */
    SB_Append (S, &Msg);

    /* Delete the formatted message */
    SB_Done (&Msg);

    /* Add a new line and terminate the generated message */
    SB_AppendChar (S, '\n');
    SB_Terminate (S);
}



static void FormatMsg (StrBuf* S, const FilePos* Pos, const char* Desc,
                       const char* Format, ...)
/* Format an error/warning message into S. A trailing newline and a NUL
 * terminator will be added to S.
 */
{
    va_list ap;
    va_start (ap, Format);
    FormatVMsg (S, Pos, Desc, Format, ap);
    va_end (ap);
}



static void AddNotifications (const Collection* LineInfos)
/* Output additional notifications for an error or warning */
{
    StrBuf Msg = STATIC_STRBUF_INITIALIZER;

    /* The basic line info is always in slot zero. It has been used to
     * output the actual error or warning. The following slots may contain
     * more information. Check them and additional notifications if they're
     * present.
     */
    unsigned I;
    for (I = 1; I < CollCount (LineInfos); ++I) {
        /* Get next line info */
        const LineInfo* LI = CollConstAt (LineInfos, I);
        /* Check the type and output an appropriate note */
        unsigned Type = GetLineInfoType (LI);
        if (Type == LI_TYPE_EXT) {
            FormatMsg (&Msg, GetSourcePos (LI), "Note",
                       "Assembler code generated from this line");
            fputs (SB_GetConstBuf (&Msg), stderr);

        } else if (Type == LI_TYPE_MACRO) {

        }
    }

    SB_Done (&Msg);
}



/*****************************************************************************/
/*   	      	     	       	   Warnings 				     */
/*****************************************************************************/



void WarningMsg (const FilePos* Pos, unsigned Level, const char* Format, va_list ap)
/* Print warning message. */
{
    if (Level <= WarnLevel) {

        StrBuf Msg = STATIC_STRBUF_INITIALIZER;
        FormatVMsg (&Msg, Pos, "Warning", Format, ap);
        fputs (SB_GetConstBuf (&Msg), stderr);
        SB_Done (&Msg);

	++WarningCount;
    }
}



void Warning (unsigned Level, const char* Format, ...)
/* Print warning message. */
{
    va_list ap;
    va_start (ap, Format);
    WarningMsg (&CurTok.Pos, Level, Format, ap);
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



void LIWarning (const Collection* LineInfos, unsigned Level, const char* Format, ...)
/* Print warning message using the given line infos */
{
    if (Level <= WarnLevel) {

        va_list ap;

        /* The first entry in the collection is that of the actual source pos */
        const LineInfo* LI = CollConstAt (LineInfos, 0);

        /* Output a warning for this position */
        va_start (ap, Format);
        WarningMsg (GetSourcePos (LI), Level, Format, ap);
        va_end (ap);

        /* Add additional notifications if necessary */
        AddNotifications (LineInfos);
    }
}



/*****************************************************************************/
/*	       	     		    Errors				     */
/*****************************************************************************/



void ErrorMsg (const FilePos* Pos, const char* Format, va_list ap)
/* Print an error message */
{
    StrBuf Msg = STATIC_STRBUF_INITIALIZER;
    FormatVMsg (&Msg, Pos, "Error", Format, ap);
    fputs (SB_GetConstBuf (&Msg), stderr);
    SB_Done (&Msg);

    ++ErrorCount;
}



void Error (const char* Format, ...)
/* Print an error message */
{
    va_list ap;
    va_start (ap, Format);
    ErrorMsg (&CurTok.Pos, Format, ap);
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



void LIError (const Collection* LineInfos, const char* Format, ...)
/* Print an error message using the given line infos. */
{
    va_list ap;

    /* The first entry in the collection is that of the actual source pos */
    const LineInfo* LI = CollConstAt (LineInfos, 0);

    /* Output an error for this position */
    va_start (ap, Format);
    ErrorMsg (GetSourcePos (LI), Format, ap);
    va_end (ap);

    /* Add additional notifications if necessary */
    AddNotifications (LineInfos);
}



void ErrorSkip (const char* Format, ...)
/* Print an error message and skip the rest of the line */
{
    va_list ap;
    va_start (ap, Format);
    ErrorMsg (&CurTok.Pos, Format, ap);
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



