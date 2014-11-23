/*****************************************************************************/
/*                                                                           */
/*                                 output.c                                  */
/*                                                                           */
/*                       Disassembler output routines                        */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2014, Ullrich von Bassewitz                                      */
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
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

/* common */
#include "addrsize.h"
#include "cpu.h"
#include "version.h"

/* da65 */
#include "code.h"
#include "error.h"
#include "global.h"
#include "output.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



static FILE*    F       = 0;            /* Output stream */
static unsigned Col     = 1;            /* Current column */
static unsigned Line    = 0;            /* Current line on page */
static unsigned Page    = 1;            /* Current output page */

static const char* SegmentName = 0;     /* Name of current segment */



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static void PageHeader (void)
/* Print a page header */
{
    fprintf (F,
             "; da65 V%s\n"
             "; Created:    %s\n"
             "; Input file: %s\n"
             "; Page:       %u\n\n",
             GetVersionAsString (),
             Now,
             InFile,
             Page);
}



void OpenOutput (const char* Name)
/* Open the given file for output */
{
    /* If we have a name given, open the output file, otherwise use stdout */
    if (Name != 0) {
        F = fopen (Name, "w");
        if (F == 0) {
            Error ("Cannot open `%s': %s", Name, strerror (errno));
        }
    } else {
        F = stdout;
    }

    /* Output the header and initialize stuff */
    PageHeader ();
    Line = 5;
    Col  = 1;
}



void CloseOutput (void)
/* Close the output file */
{
    if (F != stdout && fclose (F) != 0) {
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
        if (PageLength > 0 && ++Line >= PageLength) {
            if (FormFeeds) {
                fputc ('\f', F);
            }
            ++Page;
            PageHeader ();
            Line = 5;
        }
        Col = 1;
    }
}



void DefLabel (const char* Name)
/* Define a label with the given name */
{
    Output ("%s:", Name);
    /* If the label is longer than the configured maximum, or if it runs into
    ** the opcode column, start a new line.
    */
    if (Col > LBreak+2 || Col > MCol) {
        LineFeed ();
    }
}



void DefForward (const char* Name, const char* Comment, unsigned Offs)
/* Define a label as "* + x", where x is the offset relative to the
** current PC.
*/
{
    if (Pass == PassCount) {
        /* Flush existing output if necessary */
        if (Col > 1) {
            LineFeed ();
        }

        /* Output the forward definition */
        Output ("%s", Name);
        Indent (ACol);
        if (UseHexOffs) {
            Output (":= * + $%04X", Offs);
        } else {
            Output (":= * + %u", Offs);
        }
        if (Comment) {
            Indent (CCol);
            Output ("; %s", Comment);
        }
        LineFeed ();
    }
}



void DefConst (const char* Name, const char* Comment, unsigned Addr)
/* Define an address constant */
{
    if (Pass == PassCount) {
        Output ("%s", Name);
        Indent (ACol);
        Output (":= $%04X", Addr);
        if (Comment) {
            Indent (CCol);
            Output ("; %s", Comment);
        }
        LineFeed ();
    }
}



void DataByteLine (unsigned ByteCount)
/* Output a line with bytes */
{
    unsigned I;

    Indent (MCol);
    Output (".byte");
    Indent (ACol);
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



void DataDByteLine (unsigned ByteCount)
/* Output a line with dbytes */
{
    unsigned I;

    Indent (MCol);
    Output (".dbyt");
    Indent (ACol);
    for (I = 0; I < ByteCount; I += 2) {
        if (I > 0) {
            Output (",$%04X", GetCodeDByte (PC+I));
        } else {
            Output ("$%04X", GetCodeDByte (PC+I));
        }
    }
    LineComment (PC, ByteCount);
    LineFeed ();
}



void DataWordLine (unsigned ByteCount)
/* Output a line with words */
{
    unsigned I;

    Indent (MCol);
    Output (".word");
    Indent (ACol);
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

    Indent (MCol);
    Output (".dword");
    Indent (ACol);
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
    if (Pass == PassCount && Comments >= 1) {
        Output ("; ----------------------------------------------------------------------------");
        LineFeed ();
    }
}



void StartSegment (const char* Name, unsigned AddrSize)
/* Start a segment */
{
    if (Pass == PassCount) {
        LineFeed ();
        Output (".segment");
        Indent (ACol);
        SegmentName = Name;
        Output ("\"%s\"", Name);
        if (AddrSize != ADDR_SIZE_DEFAULT) {
            Output (": %s", AddrSizeToStr (AddrSize));
        }
        LineFeed ();
        LineFeed ();
    }
}



void EndSegment (void)
/* End a segment */
{
    LineFeed ();
    Output ("; End of \"%s\" segment", SegmentName);
    LineFeed ();
    SeparatorLine ();
    Output (".code");
    LineFeed ();
    LineFeed ();
}



void UserComment (const char* Comment)
/* Output a comment line */
{
    Output ("; %s", Comment);
    LineFeed ();
}



void LineComment (unsigned PC, unsigned Count)
/* Add a line comment with the PC and data bytes */
{
    unsigned I;

    if (Pass == PassCount && Comments >= 2) {
        Indent (CCol);
        Output ("; %04X", PC);
        if (Comments >= 3) {
            for (I = 0; I < Count; ++I) {
                Output (" %02X", CodeBuf [PC+I]);
            }
            if (Comments >= 4) {
                Indent (TCol);
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



void OutputSettings (void)
/* Output CPU and other settings */
{
    LineFeed ();
    Indent (MCol);
    Output (".setcpu");
    Indent (ACol);
    Output ("\"%s\"", CPUNames[CPU]);
    LineFeed ();
    LineFeed ();
}
