/*****************************************************************************/
/*                                                                           */
/*				     bin.c  				     */
/*                                                                           */
/*		    Module to handle the raw binary format		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1999-2001 Ullrich von Bassewitz                                       */
/*               Wacholderweg 14                                             */
/*               D-70597 Stuttgart                                           */
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
#include <string.h>
#include <errno.h>

/* common */
#include "print.h"
#include "xmalloc.h"

/* ld65 */
#include "global.h"
#include "error.h"
#include "fileio.h"
#include "lineinfo.h"
#include "segments.h"
#include "exports.h"
#include "config.h"
#include "expr.h"
#include "bin.h"



/*****************************************************************************/
/*     	       	    	       	     Data				     */
/*****************************************************************************/



struct BinDesc {
    unsigned	Undef;	  	/* Count of undefined externals */
    FILE*    	F;		/* Output file */
    const char* Filename;	/* Name of output file */
};



/*****************************************************************************/
/*     	      	    	   	     Code	 		       	     */
/*****************************************************************************/



BinDesc* NewBinDesc (void)
/* Create a new binary format descriptor */
{
    /* Allocate memory for a new BinDesc struct */
    BinDesc* D = xmalloc (sizeof (BinDesc));

    /* Initialize the fields */
    D->Undef	= 0;
    D->F	= 0;
    D->Filename = 0;

    /* Return the created struct */
    return D;
}



void FreeBinDesc (BinDesc* D)
/* Free a binary format descriptor */
{
    xfree (D);
}



static unsigned BinWriteExpr (ExprNode* E, int Signed, unsigned Size,
		  	      unsigned long Offs attribute ((unused)),
			      void* Data)
/* Called from SegWrite for an expression. Evaluate the expression, check the
 * range and write the expression value to the file.
 */
{
    /* There's a predefined function to handle constant expressions */
    return SegWriteConstExpr (((BinDesc*)Data)->F, E, Signed, Size);
}



static void PrintBoolVal (const char* Name, int B)
/* Print a boolean value for debugging */
{
    printf ("      %s = %s\n", Name, B? "true" : "false");
}



static void BinWriteMem (BinDesc* D, Memory* M)
/* Write the segments of one memory area to a file */
{
    /* Get the start address of this memory area */
    unsigned long Addr = M->Start;

    /* Get a pointer to the first segment node */
    MemListNode* N = M->SegList;
    while (N) {

	int DoWrite;

	/* Get the segment from the list node */
    	SegDesc* S = N->Seg;

	/* Keep the user happy */
       	Print (stdout, 1, "    Writing `%s'\n", S->Name);

	/* Writes do only occur in the load area and not for BSS segments */
       	DoWrite = (S->Flags & SF_BSS) == 0 	&& 	/* No BSS segment */
		   S->Load == M 		&&	/* LOAD segment */
		   S->Seg->Dumped == 0;			/* Not already written */

	/* Output the DoWrite flag for debugging */
	if (Verbosity > 1) {
       	    PrintBoolVal ("bss", S->Flags & SF_BSS);
	    PrintBoolVal ("LoadArea", S->Load == M);
       	    PrintBoolVal ("Dumped", S->Seg->Dumped);
	    PrintBoolVal ("DoWrite", DoWrite);
	}

	/* Check if we would need an alignment */
	if (S->Seg->Align > S->Align) {
	    /* Segment itself requires larger alignment than configured
	     * in the linker.
	     */
	    Warning ("Segment `%s' in module `%s' requires larger alignment",
	     	     S->Name, GetObjFileName (S->Seg->AlignObj));
	}

	/* Handle ALIGN and OFFSET/START */
	if (S->Flags & SF_ALIGN) {
	    /* Align the address */
	    unsigned long Val, NewAddr;
	    Val = (0x01UL << S->Align) - 1;
	    NewAddr = (Addr + Val) & ~Val;
	    if (DoWrite) {
		WriteMult (D->F, M->FillVal, NewAddr-Addr);
	    }
	    Addr = NewAddr;
	    /* Remember the fill value for the segment */
       	    S->Seg->FillVal = M->FillVal;
	} else if (S->Flags & (SF_OFFSET | SF_START)) {
	    unsigned long NewAddr = S->Addr;
	    if (S->Flags & SF_OFFSET) {
		/* It's an offset, not a fixed address, make an address */
		NewAddr += M->Start;
	    }
	    if (DoWrite) {
		WriteMult (D->F, M->FillVal, NewAddr-Addr);
	    }
	    Addr = NewAddr;
	}

	/* Now write the segment to disk if it is not a BSS type segment and
	 * if the memory area is the load area.
	 */
       	if (DoWrite) {
	    RelocLineInfo (S->Seg);
	    SegWrite (D->F, S->Seg, BinWriteExpr, D);
	} else if (M->Flags & MF_FILL) {
	    WriteMult (D->F, M->FillVal, S->Seg->Size);
	}

	/* If this was the load memory area, mark the segment as dumped */
	if (S->Load == M) {
  	    S->Seg->Dumped = 1;
       	}

	/* Calculate the new address */
	Addr += S->Seg->Size;

	/* Next segment node */
	N = N->Next;
    }

    /* If a fill was requested, fill the remaining space */
    if (M->Flags & MF_FILL) {
	while (M->FillLevel < M->Size) {
	    Write8 (D->F, M->FillVal);
	    ++M->FillLevel;
	}
    }
}



static int BinUnresolved (const char* Name attribute ((unused)), void* D)
/* Called if an unresolved symbol is encountered */
{
    /* Unresolved symbols are an error in binary format. Bump the counter
     * and return zero telling the caller that the symbol is indeed
     * unresolved.
     */
    ((BinDesc*) D)->Undef++;
    return 0;
}



void BinWriteTarget (BinDesc* D, struct File* F)
/* Write a binary output file */
{
    Memory* M;

    /* Place the filename in the control structure */
    D->Filename = F->Name;

    /* Check for unresolved symbols. The function BinUnresolved is called
     * if we get an unresolved symbol.
     */
    D->Undef = 0;   		/* Reset the counter */
    CheckExports (BinUnresolved, D);
    if (D->Undef > 0) {
	/* We had unresolved symbols, cannot create output file */
       	Error ("%u unresolved external(s) found - cannot create output file", D->Undef);
    }

    /* Open the file */
    D->F = fopen (F->Name, "wb");
    if (D->F == 0) {
	Error ("Cannot open `%s': %s", F->Name, strerror (errno));
    }

    /* Keep the user happy */
    Print (stdout, 1, "Opened `%s'...\n", F->Name);

    /* Dump all memory areas */
    M = F->MemList;
    while (M) {
	Print (stdout, 1, "  Dumping `%s'\n", M->Name);
	BinWriteMem (D, M);
	M = M->FNext;
    }

    /* Close the file */
    if (fclose (D->F) != 0) {
	Error ("Cannot write to `%s': %s", F->Name, strerror (errno));
    }

    /* Reset the file and filename */
    D->F        = 0;
    D->Filename = 0;
}



