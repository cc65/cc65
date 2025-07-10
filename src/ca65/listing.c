/*****************************************************************************/
/*                                                                           */
/*                                 listing.c                                 */
/*                                                                           */
/*                Listing support for the ca65 crossassembler                */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2011, Ullrich von Bassewitz                                      */
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
#include <string.h>
#include <errno.h>

/* common */
#include "check.h"
#include "fname.h"
#include "fragdefs.h"
#include "strbuf.h"
#include "version.h"
#include "xmalloc.h"

/* ca65 */
#include "error.h"
#include "filetab.h"
#include "global.h"
#include "listing.h"
#include "segment.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Single linked list of lines */
ListLine*       LineList = 0;           /* List of listing lines */
ListLine*       LineCur  = 0;           /* Current listing line */
ListLine*       LineLast = 0;           /* Last (current) listing line */

/* Page and other formatting */
int             PageLength = -1;        /* Length of a listing page */
static unsigned PageNumber = 1;         /* Current listing page number */
static int      PageLines  = 0;         /* Current line on page */
static unsigned ListBytes  = 12;        /* Number of bytes to list for one line */

/* Switch the listing on/off */
static int      ListingEnabled = 1;     /* Enabled if > 0 */



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void NewListingLine (const StrBuf* Line, unsigned char File, unsigned char Depth)
/* Create a new ListLine struct and insert it */
{
    /* Store only if listing is enabled */
    if (SB_GetLen (&ListingName) > 0) {

        ListLine* L;

        /* Get the length of the line */
        unsigned Len = SB_GetLen (Line);

        /* Ignore trailing newlines */
        while (Len > 0 && SB_AtUnchecked (Line, Len-1) == '\n') {
            --Len;
        }

        /* Allocate memory */
        L = xmalloc (sizeof (ListLine) + Len);

        /* Initialize the fields. */
        L->Next         = 0;
        L->FragList     = 0;
        L->FragLast     = 0;
        L->PC           = GetPC ();
        L->Reloc        = GetRelocMode ();
        L->File         = File;
        L->Depth        = Depth;
        L->Output       = (ListingEnabled > 0);
        L->ListBytes    = (unsigned char) ListBytes;
        memcpy (L->Line, SB_GetConstBuf (Line), Len);
        L->Line[Len] = '\0';

        /* Insert the line into the list of lines */
        if (LineList == 0) {
            LineList = L;
        } else {
            LineLast->Next = L;
        }
        LineLast = L;
    }
}



void EnableListing (void)
/* Enable output of lines to the listing */
{
    if (SB_GetLen (&ListingName) > 0) {
        /* If we're about to enable the listing, do this for the current line
        ** also, so we will see the source line that did this.
        */
        if (ListingEnabled++ == 0) {
            LineCur->Output = 1;
        }
    }
}



void DisableListing (void)
/* Disable output of lines to the listing */
{
    if (SB_GetLen (&ListingName) > 0) {
        if (ListingEnabled == 0) {
            /* Cannot switch the listing off once more */
            Error ("Counter underflow");
        } else {
            --ListingEnabled;
        }
    }
}



void SetListBytes (int Bytes)
/* Set the maximum number of bytes listed for one line */
{
    if (Bytes < 0) {
        Bytes = 0;      /* Encode "unlimited" as zero */
    }
    ListBytes = Bytes;
}



void InitListingLine (void)
/* Initialize the current listing line */
{
    if (SB_GetLen (&ListingName) > 0) {
        /* Make the last loaded line the current line */
        /* ###### This code is a hack! We really need to do it right --
        ** as soon as we know how. :-(
        */
        if (LineCur && LineCur->Next && LineCur->Next != LineLast) {
            ListLine* L = LineCur;
            do {
                L = L->Next;
                /* Set the values for this line */
                CHECK (L != 0);
                L->PC            = GetPC ();
                L->Reloc         = GetRelocMode ();
                L->Output        = (ListingEnabled > 0);
                L->ListBytes = (unsigned char) ListBytes;
            } while (L->Next != LineLast);
        }
        LineCur = LineLast;

        /* Set the values for this line */
        CHECK (LineCur != 0);
        LineCur->PC         = GetPC ();
        LineCur->Reloc      = GetRelocMode ();
        LineCur->Output     = (ListingEnabled > 0);
        LineCur->ListBytes  = (unsigned char) ListBytes;
    }
}



static char* AddHex (char* S, unsigned Val)
/* Add a hex byte in ASCII to the given string and return the new pointer */
{
    static const char HexTab [16] = {
        '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'
    };

    *S++ = HexTab [(Val >> 4) & 0x0F];
    *S++ = HexTab [Val & 0x0F];

    return S;
}



static void PrintPageHeader (FILE* F, const ListLine* L)
/* Print the header for a new page. It is assumed that the given line is the
** last line of the previous page.
*/
{
    /* Gte a pointer to the current input file */
    const StrBuf* CurFile = GetFileName (L->File);

    /* Print the header on the new page */
    fprintf (F,
             "ca65 V%s\n"
             "Main file   : %s\n"
             "Current file: %.*s\n"
             "\n",
             GetVersionAsString (),
             InFile,
             (int) SB_GetLen (CurFile), SB_GetConstBuf (CurFile));

    /* Count pages, reset lines */
    ++PageNumber;
    PageLines = 4;
}



static void PrintLine (FILE* F, const char* Header, const char* Line, const ListLine* L)
/* Print one line to the listing file, adding a newline and counting lines */
{
    /* Print the given line */
    fprintf (F, "%s%s\n", Header, Line);

    /* Increment the current line */
    ++PageLines;

    /* Switch to a new page if needed. Do not switch, if the current line is
    ** the last one, to avoid pages that consist of just the header.
    */
    if (PageLength > 0 && PageLines >= PageLength && L->Next != 0) {
        /* Do a formfeed */
        putc ('\f', F);
        /* Print the header on the new page */
        PrintPageHeader (F, L);
    }
}



static char* AddMult (char* S, char C, unsigned Count)
/* Add multiple instances of character C to S, return updated S. */
{
    memset (S, C, Count);
    return S + Count;
}



static char* MakeLineHeader (char* H, const ListLine* L)
/* Prepare the line header */
{
    char Mode;
    char Depth;

    /* Setup the PC mode */
    Mode = (L->Reloc)? 'r' : ' ';

    /* Set up the include depth */
    Depth = (L->Depth < 10)? L->Depth + '0' : '+';

    /* Format the line */
    sprintf (H, "%06lX%c %c", L->PC, Mode, Depth);
    memset (H+9, ' ', LINE_HEADER_LEN-9);

    /* Return the buffer */
    return H;
}



void CreateListing (void)
/* Create the listing */
{
    FILE* F;
    Fragment* Frag;
    ListLine* L;
    char HeaderBuf [LINE_HEADER_LEN+1];

    /* Open the real listing file */
    F = fopen (SB_GetConstBuf (&ListingName), "w");
    if (F == 0) {
        Fatal ("Cannot open listing file `%s': %s",
               SB_GetConstBuf (&ListingName),
               strerror (errno));
    }

    /* Reset variables, print the header for the first page */
    PageNumber = 0;
    PrintPageHeader (F, LineList);

    /* Terminate the header buffer. The last byte will never get overwritten */
    HeaderBuf [LINE_HEADER_LEN] = '\0';

    /* Walk through all listing lines */
    L = LineList;
    while (L) {

        char* Buf;
        char* B;
        unsigned Count;
        unsigned I;
        char* Line;

        /* If we should not output this line, go to the next */
        if (L->Output == 0) {
            L = L->Next;
            continue;
        }

        /* If we don't have a fragment list for this line, things are easy */
        if (L->FragList == 0) {
            PrintLine (F, MakeLineHeader (HeaderBuf, L), L->Line, L);
            L = L->Next;
            continue;
        }

        /* Count the number of bytes in the complete fragment list */
        Count = 0;
        Frag = L->FragList;
        while (Frag) {
            Count += Frag->Len;
            Frag = Frag->LineList;
        }

        /* Allocate memory for the given number of bytes */
        Buf = xmalloc (Count*2+1);

        /* Copy an ASCII representation of the bytes into the buffer */
        B = Buf;
        Frag = L->FragList;
        while (Frag) {

            /* Write data depending on the type */
            switch (Frag->Type) {

                case FRAG_LITERAL:
                    for (I = 0; I < Frag->Len; ++I) {
                        B = AddHex (B, Frag->V.Data[I]);
                    }
                    break;

                case FRAG_EXPR:
                case FRAG_SEXPR:
                    B = AddMult (B, 'r', Frag->Len*2);
                    break;

                case FRAG_FILL:
                    B = AddMult (B, 'x', Frag->Len*2);
                    break;

                default:
                    Internal ("Invalid fragment type: %u", Frag->Type);

            }

            /* Next fragment */
            Frag = Frag->LineList;

        }

        /* Limit the number of bytes actually printed */
        if (L->ListBytes != 0) {
            /* Not unlimited */
            if (Count > L->ListBytes) {
                Count = L->ListBytes;
            }
        }

        /* Output the data. The format of a listing line is:
        **
        **      PPPPPPm I  11 22 33 44
        **
        ** where
        **
        **      PPPPPP  is the PC
        **      m       is the mode ('r' or empty)
        **      I       is the include level
        **      11 ..   are code or data bytes
        */
        Line = L->Line;
        B    = Buf;
        while (Count) {

            unsigned    Chunk;
            char*       P;

            /* Prepare the line header */
            MakeLineHeader (HeaderBuf, L);

            /* Get the number of bytes for the next line */
            Chunk = Count;
            if (Chunk > 4) {
                Chunk = 4;
            }
            Count -= Chunk;

            /* Increment the program counter. Since we don't need the PC stored
            ** in the LineList object for anything else, just increment this
            ** variable.
            */
            L->PC += Chunk;

            /* Copy the bytes into the line */
            P = HeaderBuf + 11;
            for (I = 0; I < Chunk; ++I) {
                *P++ = *B++;
                *P++ = *B++;
                *P++ = ' ';
            }

            /* Output this line */
            PrintLine (F, HeaderBuf, Line, L);

            /* Don't output a line twice */
            Line = "";

        }

        /* Delete the temporary buffer */
        xfree (Buf);

        /* Next line */
        L = L->Next;

    }

    /* Close the listing file */
    (void) fclose (F);
}
