/*****************************************************************************/
/*                                                                           */
/*				   output.c				     */
/*                                                                           */
/*			 Disassembler output routines			     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000      Ullrich von Bassewitz                                       */
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
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

/* common */
#include "version.h"

/* da65 */
#include "code.h"
#include "error.h"
#include "global.h"
#include "output.h"



/*****************************************************************************/
/*	      	  		     Data				     */
/*****************************************************************************/



static FILE* 	F 	= 0;		/* Output stream */
static unsigned	Col    	= 1;		/* Current column */
static unsigned Line   	= 0;		/* Current line on page */
static unsigned Page	= 1;		/* Current output page */



/*****************************************************************************/
/*	      	  		     Code				     */
/*****************************************************************************/



static void PageHeader (void)
/* Print a page header */
{
    fprintf (F,
       	     "; da65 V%u.%u.%u - (C) Copyright 2000 Ullrich von Bassewitz\n"
    	     "; Input file: %s\n"
    	     "; Page:       %u\n\n",
       	     VER_MAJOR, VER_MINOR, VER_PATCH,
    	     InFile,
    	     Page);
}



void OpenOutput (const char* Name)
/* Open the given file for output */
{
    /* Open the output file */
    F = fopen (Name, "w");
    if (F == 0) {
	Error ("Cannot open `%s': %s", Name, strerror (errno));
    }
    PageHeader ();
    Line = 4;
    Col  = 1;
}



void CloseOutput (void)
/* Close the output file */
{
    if (fclose (F) != 0) {
	Error ("Error closing output file: %s", strerror (errno));
    }
}



void Output (const char* Format, ...)
/* Write to the output file */
{
    if (Pass == PassCount) {
	va_list ap;
	va_start (ap, Format);
	Col += vfprintf (F, Format, ap);
	va_end (ap);
    }
}



void Indent (unsigned N)
/* Make sure the current line column is at position N (zero based) */
{
    if (Pass == PassCount) {
	while (Col < N) {
	    fputc (' ', F);
	    ++Col;
	}
    }
}



void LineFeed (void)
/* Add a linefeed to the output file */
{
    if (Pass == PassCount) {
	fputc ('\n', F);
	if (++Line >= PageLength) {
	    if (FormFeeds) {
		fputc ('\f', F);
	    }
	    ++Page;
	    PageHeader ();
	    Line = 4;
 	}
 	Col = 1;
    }
}



void DefLabel (const char* Name)
/* Define a label with the given name */
{
    Output ("%s:", Name);
    /* Don't start a new line if the label is fully in the left column */
    if (Col > MIndent) {
     	LineFeed ();
    }
}



void DataByteLine (unsigned ByteCount)
/* Output a line with bytes */
{
    unsigned I;

    Indent (MIndent);
    Output (".byte");
    Indent (AIndent);
    for (I = 0; I < ByteCount; ++I) {
 	if (I > 0) {
 	    Output (",$%02X", CodeBuf[PC+I]);
 	} else {
 	    Output ("$%02X", CodeBuf[PC+I]);
 	}
    }
    LineComment (PC, ByteCount);
    LineFeed ();
}



void DataWordLine (unsigned ByteCount)
/* Output a line with words */
{
    unsigned I;

    Indent (MIndent);
    Output (".word");
    Indent (AIndent);
    for (I = 0; I < ByteCount; I += 2) {
	if (I > 0) {
	    Output (",$%04X", GetCodeWord (PC+I));
	} else {
	    Output ("$%04X", GetCodeWord (PC+I));
	}
    }
    LineComment (PC, ByteCount);
    LineFeed ();
}



void DataDWordLine (unsigned ByteCount)
/* Output a line with dwords */
{
    unsigned I;

    Indent (MIndent);
    Output (".dword");
    Indent (AIndent);
    for (I = 0; I < ByteCount; I += 4) {
	if (I > 0) {
	    Output (",$%08lX", GetCodeDWord (PC+I));
	} else {
	    Output ("$%08lX", GetCodeDWord (PC+I));
	}
    }
    LineComment (PC, ByteCount);
    LineFeed ();
}



void SeparatorLine (void)
/* Print a separator line */
{
    if (Pass == PassCount && Verbosity >= 1) {
	Output ("; ----------------------------------------------------------------------------");
	LineFeed ();
    }
}



void LineComment (unsigned PC, unsigned Count)
/* Add a line comment with the PC and data bytes */
{
    unsigned I;

    if (Pass == PassCount && Verbosity >= 2) {
	Indent (CIndent);
	Output ("; %04X", PC);
	if (Verbosity >= 3) {
	    for (I = 0; I < Count; ++I) {
		Output (" %02X", CodeBuf [PC+I]);
	    }
	    if (Verbosity >= 4) {
		Indent (TIndent);
		for (I = 0; I < Count; ++I) {
		    unsigned char C = CodeBuf [PC+I];
		    if (!isprint (C)) {
			C = '.';
		    }
		    Output ("%c", C);
		}
	    }
	}
    }
}



