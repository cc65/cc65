/*****************************************************************************/
/*                                                                           */
/*				   listing.c				     */
/*                                                                           */
/*		  Listing support for the ca65 crossassembler		     */
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
#include <string.h>
#include <errno.h>

#include "../common/segdefs.h"
#include "../common/version.h"

#include "error.h"
#include "fname.h"
#include "global.h"
#include "mem.h"
#include "objcode.h"
#include "listing.h"



/*****************************************************************************/
/*	 	   	  	     Data				     */
/*****************************************************************************/



/* Single linked list of lines */
ListLine*      	LineList = 0;  		/* List of listing lines */
ListLine*      	LineCur  = 0; 		/* Current listing line */
ListLine*       LineLast = 0;   	/* Last (current) listing line */

/* Page and other formatting */
int    	     	PageLength = -1;    	/* Length of a listing page */
static unsigned	PageNumber = 1;		/* Current listing page number */
static unsigned PageLines  = 0;		/* Current line on page */
static unsigned ListBytes  = 12;	/* Number of bytes to list for one line */

/* Switch the listing on/off */
static int     	ListingEnabled = 1;	/* Enabled if > 0 */



/*****************************************************************************/
/*     	       	       		     Code				     */
/*****************************************************************************/



void NewListingLine (const char* Line, unsigned char File, unsigned char Depth)
/* Create a new ListLine struct and insert it */
{
    /* Store only if listing is enabled */
    if (Listing) {

	ListLine* L;

	/* Get the length of the line */
	unsigned Len = strlen (Line);

	/* Ignore trailing newlines */
	while (Len > 0 && Line[Len-1] == '\n') {
	    --Len;
	}

	/* Allocate memory */
	L = Xmalloc (sizeof (ListLine) + Len);

	/* Initialize the fields. */
	L->Next  	= 0;
	L->FragList	= 0;
	L->FragLast 	= 0;
	L->PC  		= GetPC ();
	L->Reloc	= RelocMode;
	L->File		= File;
     	L->Depth	= Depth;
	L->Output	= (ListingEnabled > 0);
	L->ListBytes	= (unsigned char) ListBytes;
 	memcpy (L->Line, Line, Len);
	L->Line [Len] = '\0';

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
    if (Listing) {
	/* If we're about to enable the listing, do this for the current line
	 * also, so we will see the source line that did this.
	 */
	if (ListingEnabled++ == 0) {
	    LineCur->Output = 1;
	}
    }
}



void DisableListing (void)
/* Disable output of lines to the listing */
{
    if (Listing) {
	if (ListingEnabled == 0) {
	    /* Cannot switch the listing off once more */
	    Error (ERR_COUNTER_UNDERFLOW);
	} else {
	    --ListingEnabled;
	}
    }
}



void SetListBytes (int Bytes)
/* Set the maximum number of bytes listed for one line */
{
    if (Bytes < 0) {
     	Bytes = 0;	/* Encode "unlimited" as zero */
    }
    ListBytes = Bytes;
}



void InitListingLine (void)
/* Initialize the current listing line */
{
    if (Listing) {
	/* Make the last loaded line the current line */
	LineCur = LineLast;

	/* Set the values for this line */
	CHECK (LineCur != 0);
	LineCur->PC    	    = GetPC ();
	LineCur->Reloc	    = RelocMode;
	LineCur->Output	    = (ListingEnabled > 0);
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
 * last line of the previous page.
 */
{
    /* Print the header on the new page */
    fprintf (F, 
	     "ca65 V%u.%u.%u - (C) Copyright 1998-2000 Ullrich von Bassewitz\n"
    	     "Main file   : %s\n"
    	     "Current file: %s\n"
	     "\n",
     	     VER_MAJOR, VER_MINOR, VER_PATCH,
	     InFile,
	     GetFileName (L->File));

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
     * the last one, to avoid pages that consist of just the header.
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

    /* Create the name of the listing file if needed */
    if (ListFile == 0) {
    	ListFile = MakeFilename (InFile, ListExt);
    }

    /* Open the real listing file */
    F = fopen (ListFile, "w");
    if (F == 0) {
    	Fatal (FAT_CANNOT_OPEN_LISTING, strerror (errno));
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
	Buf = Xmalloc (Count*2+1);

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
	 *
       	 *     	PPPPPPm I  11 22 33 44
     	 *
     	 * where
     	 *
     	 *	PPPPPP 	is the PC
	 *	m	is the mode ('r' or empty)
     	 *	I      	is the include level
     	 *	11 ..  	are code or data bytes
     	 */
	Line = L->Line;
       	B    = Buf;
      	while (Count) {

	    unsigned   	Chunk;
      	    char*      	P;

     	    /* Prepare the line header */
       	    MakeLineHeader (HeaderBuf, L);

      	    /* Get the number of bytes for the next line */
      	    Chunk = Count;
      	    if (Chunk > 4) {
      	       	Chunk = 4;
      	    }
      	    Count -= Chunk;

	    /* Increment the program counter. Since we don't need the PC stored
	     * in the LineList object for anything else, just increment this
	     * variable.
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
	Xfree (Buf);

	/* Next line */
	L = L->Next;

    }

    /* Close the listing file */
    (void) fclose (F);
}



