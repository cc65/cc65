/*****************************************************************************/
/*                                                                           */
/*				     o65.c				     */
/*                                                                           */
/*		    Module to handle the o65 binary format		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1999     Ullrich von Bassewitz                                        */
/*              Wacholderweg 14                                              */
/*              D-70597 Stuttgart                                            */
/* EMail:       uz@musoftware.de                                             */
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
#include <time.h>

#include "../common/version.h"

#include "global.h"
#include "error.h"
#include "mem.h"
#include "fileio.h"
#include "exports.h"
#include "expr.h"
#include "o65.h"



/*****************************************************************************/
/*     	       	    	      	     Data				     */
/*****************************************************************************/



/* Header mode bits */
#define MF_SIZE_32BIT	0x2000		/* All size words are 32bit */
#define MF_CPU_816     	0x8000		/* Executable is for 65816 */

/* The four o65 segment types. Note: These values are identical to the values
 * needed for the segmentID in the o65 spec.
 */
#define O65SEG_UNDEF	0x00
#define O65SEG_ABS	0x01
#define O65SEG_TEXT	0x02
#define O65SEG_DATA	0x03
#define O65SEG_BSS	0x04
#define O65SEG_ZP	0x05

/* Relocation type codes for the o65 format */
#define O65RELOC_WORD	0x80
#define O65RELOC_HIGH	0x40
#define O65RELOC_LOW	0x20
#define O65RELOC_SEGADR	0xc0
#define O65RELOC_SEG	0xa0

/* O65 executable file header */
typedef struct O65Header_ O65Header;
struct O65Header_ {
    unsigned	    Version;	    	/* Version number for o65 format */
    unsigned        Mode;      	    	/* Mode word */
    unsigned long   TextBase;		/* Base address of text segment */
    unsigned long   TextSize;		/* Size of text segment */
    unsigned long   DataBase;		/* Base of data segment */
    unsigned long   DataSize;		/* Size of data segment */
    unsigned long   BssBase;		/* Base of bss segment */
    unsigned long   BssSize;		/* Size of bss segment */
    unsigned long   ZPBase;		/* Base of zeropage segment */
    unsigned long   ZPSize;		/* Size of zeropage segment */
    unsigned long   StackSize;		/* Requested stack size */
};

/* An o65 option */
typedef struct O65Option_ O65Option;
struct O65Option_ {
    O65Option*	    Next;		/* Next in option list */
    unsigned char   Type;		/* Type of option */
    unsigned char   Len;		/* Data length */
    unsigned char   Data [1];		/* Data, dynamically allocated */
};

/* A o65 relocation table */
#define RELOC_BLOCKSIZE	4096
typedef struct O65RelocTab_ O65RelocTab;
struct O65RelocTab_ {
    unsigned 	    Size;   		/* Size of the table */
    unsigned	    Fill;   		/* Amount used */
    unsigned char*  Buf; 		/* Buffer, dynamically allocated */
};

/* Structure describing the format */
struct O65Desc_ {
    O65Header	    Header; 		/* File header */
    O65Option*	    Options;		/* List of file options */
    ExtSymTab*	    Exports;		/* Table with exported symbols */
    ExtSymTab*	    Imports;		/* Table with imported symbols */
    unsigned	    Undef;		/* Count of undefined symbols */
    FILE*   	    F;			/* The file we're writing to */
    char*   	    Filename;		/* Name of the output file */
    O65RelocTab*    TextReloc;		/* Relocation table for text segment */
    O65RelocTab*    DataReloc;		/* Relocation table for data segment */

    unsigned	    TextCount;		/* Number of segments assigned to .text */
    SegDesc**	    TextSeg;		/* Array of text segments */
    unsigned	    DataCount;		/* Number of segments assigned to .data */
    SegDesc**	    DataSeg;	    	/* Array of data segments */
    unsigned	    BssCount;	    	/* Number of segments assigned to .bss */
    SegDesc**	    BssSeg;    	    	/* Array of bss segments */
    unsigned 	    ZPCount;		/* Number of segments assigned to .zp */
    SegDesc**	    ZPSeg;		/* Array of zp segments */

    /* Temporary data for writing segments */
    unsigned long   SegSize;
    O65RelocTab*    CurReloc;
    long	    LastOffs;
};

/* Structure for parsing expression trees */
typedef struct ExprDesc_ ExprDesc;
struct ExprDesc_ {
    O65Desc*	    D;			/* File format descriptor */
    long	    Val;		/* The offset value */
    int	       	    TooComplex;	       	/* Expression too complex */
    Section*	    SegRef;		/* Section referenced if any */
    ExtSym*	    ExtRef;		/* External reference if any */
};



/*****************************************************************************/
/*	     	    	       Helper functions				     */
/*****************************************************************************/



static void WriteSize (const O65Desc* D, unsigned long Val)
/* Write a "size" word to the file */
{
    if (D->Header.Mode & MF_SIZE_32BIT) {
	Write32 (D->F, Val);
    } else {
	Write16 (D->F, (unsigned) Val);
    }
}



static unsigned O65SegType (const SegDesc* S)
/* Map our own segment types into something o65 compatible */
{
    /* Check the segment type. Readonly segments are assign to the o65
     * text segment, writeable segments that contain data are assigned
     * to data, bss and zp segments are handled respectively.
     * Beware: Zeropage segments have the SF_BSS flag set, so be sure
     * to check SF_ZP first.
     */
    if (S->Flags & SF_RO) {
	return O65SEG_TEXT;
    } else if (S->Flags & SF_ZP) {
	return O65SEG_ZP;
    } else if (S->Flags & SF_BSS) {
	return O65SEG_BSS;
    } else {
	return O65SEG_DATA;
    }
}



/*****************************************************************************/
/*		  	      Expression handling			     */
/*****************************************************************************/



static void O65ParseExpr (ExprNode* Expr, ExprDesc* D, int Sign)
/* Extract and evaluate all constant factors in an subtree that has only
 * additions and subtractions. If anything other than additions and
 * subtractions are found, D->TooComplex is set to true.
 */
{
    Export* E;

    switch (Expr->Op) {

	case EXPR_LITERAL:
	    if (Sign < 0) {
	       	D->Val -= Expr->V.Val;
	    } else {
	       	D->Val += Expr->V.Val;
	    }
	    break;

	case EXPR_SYMBOL:
	    /* Get the referenced Export */
	    E = GetExprExport (Expr);
    	    /* If this export has a mark set, we've already encountered it.
    	     * This means that the export is used to define it's own value,
    	     * which in turn means, that we have a circular reference.
    	     */
    	    if (ExportHasMark (E)) {
    		CircularRefError (E);
    	    } else if (E->Expr == 0) {
    		/* Dummy export, must be an o65 imported symbol */
    		ExtSym* S = O65GetImport (D->D, E->Name);
    		CHECK (S != 0);
    		if (D->ExtRef) {
    		    /* We cannot have more than one external reference in o65 */
    		    D->TooComplex = 1;
    		} else {
    		    /* Remember the external reference */
    		    D->ExtRef = S;
    		}
    	    } else {
    		MarkExport (E);
    	    	O65ParseExpr (E->Expr, D, Sign);
    		UnmarkExport (E);
    	    }
    	    break;

    	case EXPR_SEGMENT:
    	    if (D->SegRef) {
    	        /* We cannot handle more than one segment reference in o65 */
    		D->TooComplex = 1;
    	    } else {
    		/* Remember the segment reference */
    		D->SegRef = GetExprSection (Expr);
    	    }
    	    break;

    	case EXPR_PLUS:
    	    O65ParseExpr (Expr->Left, D, Sign);
      	    O65ParseExpr (Expr->Right, D, Sign);
    	    break;

    	case EXPR_MINUS:
    	    O65ParseExpr (Expr->Left, D, Sign);
    	    O65ParseExpr (Expr->Right, D, -Sign);
    	    break;

     	default:
    	    /* Expression contains illegal operators */
    	    D->TooComplex = 1;
    	    break;

    }
}



/*****************************************************************************/
/*  			       Relocation tables			     */
/*****************************************************************************/



static O65RelocTab* NewO65RelocTab (void)
/* Create a new relocation table */
{
    /* Allocate a new structure */
    O65RelocTab* R = Xmalloc (sizeof (O65RelocTab));

    /* Initialize the data */
    R->Size = RELOC_BLOCKSIZE;
    R->Fill = 0;
    R->Buf  = Xmalloc (RELOC_BLOCKSIZE);

    /* Return the created struct */
    return R;
}



static void FreeO65RelocTab (O65RelocTab* R)
/* Free a relocation table */
{
    Xfree (R->Buf);
    Xfree (R);
}



static void O65RelocPutByte (O65RelocTab* R, unsigned B)
/* Put the byte into the relocation table */
{
    /* Do we have enough space in the buffer? */
    if (R->Fill == R->Size) {
    	/* We need to grow the buffer */
       	unsigned char* NewBuf = Xmalloc (R->Size + RELOC_BLOCKSIZE);
    	memcpy (NewBuf, R->Buf, R->Size);
    	Xfree (R->Buf);
    	R->Buf = NewBuf;
    }

    /* Put the byte into the buffer */
    R->Buf [R->Fill++] = (unsigned char) B;
}



static void O65RelocPutWord (O65RelocTab* R, unsigned W)
/* Put a word into the relocation table */
{
    O65RelocPutByte (R, W);
    O65RelocPutByte (R, W >> 8);
}



static void O65WriteReloc (O65RelocTab* R, FILE* F)
/* Write the relocation table to the given file */
{
    WriteData (F, R->Buf, R->Fill);
}



/*****************************************************************************/
/*  	      			Option handling				     */
/*****************************************************************************/



static O65Option* NewO65Option (unsigned Type, const void* Data, unsigned DataLen)
/* Allocate and initialize a new option struct */
{
    O65Option* O;

    /* Check the length */
    CHECK (DataLen <= 253);

    /* Allocate memory */
    O = Xmalloc (sizeof (O65Option) - 1 + DataLen);

    /* Initialize the structure */
    O->Next    	= 0;
    O->Type    	= Type;
    O->Len     	= DataLen;
    memcpy (O->Data, Data, DataLen);

    /* Return the created struct */
    return O;
}



static void FreeO65Option (O65Option* O)
/* Free	an O65Option struct */
{
    Xfree (O);
}



/*****************************************************************************/
/*		       Subroutines to write o65 sections		     */
/*****************************************************************************/



static void O65WriteHeader (O65Desc* D)
/* Write the header of the executable to the given file */
{
    static unsigned char Trailer [5] = {
       	0x01, 0x00, 0x6F, 0x36, 0x35
    };

    O65Option* O;


    /* Write the fixed header */
    WriteData (D->F, Trailer, sizeof (Trailer));
    Write8    (D->F, D->Header.Version);
    Write16   (D->F, D->Header.Mode);
    WriteSize (D, D->Header.TextBase);
    WriteSize (D, D->Header.TextSize);
    WriteSize (D, D->Header.DataBase);
    WriteSize (D, D->Header.DataSize);
    WriteSize (D, D->Header.BssBase);
    WriteSize (D, D->Header.BssSize);
    WriteSize (D, D->Header.ZPBase);
    WriteSize (D, D->Header.ZPSize);
    WriteSize (D, D->Header.StackSize);

    /* Write the options */
    O = D->Options;
    while (O) {
       	Write8 (D->F, O->Len + 2);		/* Account for len and type bytes */
	Write8 (D->F, O->Type);
	if (O->Len) {
	    WriteData (D->F, O->Data, O->Len);
	}
	O = O->Next;
    }

    /* Write the end-of-options byte */
    Write8 (D->F, 0);
}



static unsigned O65WriteExpr (ExprNode* E, int Signed, unsigned Size,
       		  	      unsigned long Offs, void* Data)
/* Called from SegWrite for an expression. Evaluate the expression, check the
 * range and write the expression value to the file, update the relocation
 * table.
 */
{
    long Diff;
    long BinVal;
    ExprNode* Expr;
    ExprDesc ED;
    unsigned char RelocType;

    /* Cast the Data pointer to its real type, an O65Desc */
    O65Desc* D = (O65Desc*) Data;

    /* Check for a constant expression */
    if (IsConstExpr (E)) {
       	/* Write out the constant expression */
       	return SegWriteConstExpr (((O65Desc*)Data)->F, E, Signed, Size);
    }

    /* We have a relocatable expression that needs a relocation table entry.
     * Calculate the number of bytes between this entry and the last one, and
     * setup all necessary intermediate bytes in the relocation table.
     */
    Offs += D->SegSize;		/* Calulate full offset */
    Diff = ((long) Offs) - D->LastOffs;
    while (Diff > 0xFE) {
    	O65RelocPutByte (D->CurReloc, 0xFF);
    	Diff -= 0xFE;
    }
    O65RelocPutByte (D->CurReloc, (unsigned char) Diff);

    /* Remember this offset for the next time */
    D->LastOffs = Offs;

    /* Determine the expression to relocate */
    Expr = E;
    if (E->Op == EXPR_LOBYTE || E->Op == EXPR_HIBYTE) {
      	/* Use the real expression */
       	Expr = E->Left;
    }

    /* Initialize the descriptor for expression parsing */
    ED.D    	  = D;
    ED.Val  	  = 0;
    ED.TooComplex = 0;
    ED.SegRef     = 0;
    ED.ExtRef     = 0;

    /* Recursively collect information about this expression */
    O65ParseExpr (Expr, &ED, 1);

    /* We cannot handle both, an imported symbol and a segment ref */
    if (ED.SegRef != 0 && ED.ExtRef != 0) {
     	ED.TooComplex = 1;
    }

    /* Bail out if we cannot handle the expression */
    if (ED.TooComplex) {
	return SEG_EXPR_TOO_COMPLEX;
    }

    /* Safety: Check that we are really referencing a symbol or a segment */
    CHECK (ED.SegRef != 0 || ED.ExtRef != 0);

    /* Write out the offset that goes into the segment. */
    BinVal = ED.Val;
    if (E->Op == EXPR_LOBYTE) {
	BinVal &= 0x00FF;
    } else if (E->Op == EXPR_HIBYTE) {
       	BinVal = (BinVal >> 8) & 0x00FF;
    }
    WriteVal (D->F, BinVal, Size);

    /* Determine the actual type of relocation entry needed from the
     * information gathered about the expression.
     */
    if (E->Op == EXPR_LOBYTE) {
	RelocType = O65RELOC_LOW;
    } else if (E->Op == EXPR_HIBYTE) {
	RelocType = O65RELOC_HIGH;
    } else {
	switch (Size) {

	    case 1:
		RelocType = O65RELOC_LOW;
		break;

	    case 2:
		RelocType = O65RELOC_WORD;
		break;

	    case 3:
		RelocType = O65RELOC_SEGADR;
		break;

	    case 4:
		/* 4 byte expression not supported by o65 */
		return SEG_EXPR_TOO_COMPLEX;

	    default:
	 	Internal ("O65WriteExpr: Invalid expression size: %u", Size);
    		RelocType = 0;		/* Avoid gcc warnings */
	}
    }

    /* Determine which segment we're referencing */
    if (ED.ExtRef) {
	/* Imported symbol */
	RelocType |= O65SEG_UNDEF;
       	O65RelocPutByte (D->CurReloc, RelocType);
       	/* Put the number of the imported symbol into the table */
       	O65RelocPutWord (D->CurReloc, ExtSymNum (ED.ExtRef));
    } else {
	/* Segment reference */



    }

    /* Success */
    return SEG_EXPR_OK;
}



static void O65WriteSeg (O65Desc* D, SegDesc** Seg, unsigned Count, int DoWrite)
/* Write one segment to the o65 output file */
{
    SegDesc* S;
    unsigned I;

    /* Initialize variables */
    D->SegSize  = 0;
    D->LastOffs = -1;

    /* Write out all segments */
    for (I = 0; I < Count; ++I) {

	/* Get the segment from the list node */
       	S = Seg [I];

	/* Keep the user happy */
	if (Verbose) {
	    printf ("    Writing `%s'\n", S->Name);
	}

	/* Write this segment */
	if (DoWrite) {
       	    SegWrite (D->F, S->Seg, O65WriteExpr, D);
      	}

	/* Mark the segment as dumped */
	S->Seg->Dumped = 1;

	/* Calculate the total size */
	D->SegSize += S->Seg->Size;
    }

    /* Terminate the relocation table for the this segment */
    if (D->CurReloc) {
        O65RelocPutByte (D->CurReloc, 0);
    }

    /* Check the size of the segment for overflow */
    if ((D->Header.Mode & MF_SIZE_32BIT) == 0 && D->SegSize > 0xFFFF) {
     	Error ("Segment overflow in file `%s'", D->Filename);
    }

}



static void O65WriteTextSeg (O65Desc* D, Memory* M)
/* Write the code segment to the o65 output file */
{
    /* Initialize variables */
    D->CurReloc	= D->TextReloc;

    /* Dump all text segments */
    O65WriteSeg (D, D->TextSeg, D->TextCount, 1);

    /* Set the size of the segment */
    D->Header.TextSize = D->SegSize;
}



static void O65WriteDataSeg (O65Desc* D, Memory* M)
/* Write the data segment to the o65 output file */
{
    /* Initialize variables */
    D->CurReloc	= D->DataReloc;

    /* Dump all data segments */
    O65WriteSeg (D, D->DataSeg, D->DataCount, 1);

    /* Set the size of the segment */
    D->Header.DataSize = D->SegSize;
}



static void O65WriteBssSeg (O65Desc* D, Memory* M)
/* "Write" the bss segments to the o65 output file. This will only update
 * the relevant header fields.
 */
{
    /* Initialize variables */
    D->CurReloc	= 0;

    /* Dump all data segments */
    O65WriteSeg (D, D->BssSeg, D->BssCount, 0);

    /* Set the size of the segment */
    D->Header.BssSize = D->SegSize;
}



static void O65WriteZPSeg (O65Desc* D, Memory* M)
/* "Write" the zeropage segments to the o65 output file. This will only update
 * the relevant header fields.
 */
{
    /* Initialize variables */
    D->CurReloc	= 0;

    /* Dump all data segments */
    O65WriteSeg (D, D->ZPSeg, D->ZPCount, 0);

    /* Set the size of the segment */
    D->Header.ZPSize = D->SegSize;
}



static void O65WriteImports (O65Desc* D)
/* Write the list of imported symbols to the O65 file */
{
    const ExtSym* E;

    /* Write the number of external symbols */
    WriteSize (D, ExtSymCount (D->Imports));

    /* Write out the symbol names, zero terminated */
    E = ExtSymList (D->Imports);
    while (E) {
	/* Get the name */
	const char* Name = ExtSymName (E);
	/* And write it to the output file */
	WriteData (D->F, Name, strlen (Name) + 1);
	/* Next symbol */
	E = ExtSymNext (E);
    }
}



static void O65WriteTextReloc (O65Desc* D)
/* Write the relocation for the text segment to the output file */
{
    O65WriteReloc (D->TextReloc, D->F);
}



static void O65WriteDataReloc (O65Desc* D)
/* Write the relocation for the data segment to the output file */
{
    O65WriteReloc (D->DataReloc, D->F);
}



static void O65WriteExports (O65Desc* D)
/* Write the list of exports */
{
    /* For now... */
    WriteSize (D, 0);
}



/*****************************************************************************/
/*  		   	 	  Public code				     */
/*****************************************************************************/



O65Desc* NewO65Desc (void)
/* Create, initialize and return a new O65 descriptor struct */
{
    /* Allocate a new structure */
    O65Desc* D = Xmalloc (sizeof (O65Desc));

    /* Initialize the header */
    D->Header.Version	= 0;
    D->Header.Mode      = 0;
    D->Header.TextBase	= 0;
    D->Header.TextSize  = 0;
    D->Header.DataBase  = 0;
    D->Header.DataSize  = 0;
    D->Header.BssBase   = 0;
    D->Header.BssSize   = 0;
    D->Header.ZPBase    = 0;
    D->Header.ZPSize    = 0;
    D->Header.StackSize = 0;		/* Let OS choose a good value */

    /* Initialize other data */
    D->Options	    	= 0;
    D->Exports		= NewExtSymTab ();
    D->Imports		= NewExtSymTab ();
    D->Undef	   	= 0;
    D->F	   	= 0;
    D->Filename	   	= 0;
    D->TextReloc   	= NewO65RelocTab ();
    D->DataReloc   	= NewO65RelocTab ();
    D->TextCount 	= 0;
    D->TextSeg		= 0;
    D->DataCount	= 0;
    D->DataSeg		= 0;
    D->BssCount		= 0;
    D->BssSeg		= 0;
    D->ZPCount		= 0;
    D->ZPSeg		= 0;

    /* Return the created struct */
    return D;
}



void FreeO65Desc (O65Desc* D)
/* Delete the descriptor struct with cleanup */
{
    /* Free the segment arrays */
    Xfree (D->ZPSeg);
    Xfree (D->BssSeg);
    Xfree (D->DataSeg);
    Xfree (D->TextSeg);

    /* Free the relocation tables */
    FreeO65RelocTab (D->DataReloc);
    FreeO65RelocTab (D->TextReloc);

    /* Free the option list */
    while (D->Options) {
        O65Option* O = D->Options;
	D->Options = D->Options->Next;
	FreeO65Option (O);
    }

    /* Free the external symbol tables */
    FreeExtSymTab (D->Exports);
    FreeExtSymTab (D->Imports);

    /* Free the struct itself */
    Xfree (D);
}



void O65Set816 (O65Desc* D)
/* Enable 816 mode */
{
    D->Header.Mode |= MF_CPU_816;
}



void O65SetLargeModel (O65Desc* D)
/* Enable a large memory model executable */
{
    D->Header.Mode |= MF_SIZE_32BIT;
}



void O65SetAlignment (O65Desc* D, unsigned Align)
/* Set the executable alignment */
{
    /* Remove all alignment bits from the mode word */
    D->Header.Mode &= ~0x0003;

    /* Set the alignment bits */
    switch (Align) {
	case 1:	  			  break;
    	case 2:   D->Header.Mode |= 0x01; break;
	case 4:   D->Header.Mode |= 0x02; break;
        case 256: D->Header.Mode |= 0x03; break;
        default:  Error ("Invalid alignment for O65 format: %u", Align);
    }
}



void O65SetOption (O65Desc* D, unsigned Type, const void* Data, unsigned DataLen)
/* Set an o65 header option */
{
    /* Create a new option structure */
    O65Option* O = NewO65Option (Type, Data, DataLen);

    /* Insert it into the linked list */
    O->Next = D->Options;
    D->Options = O;
}



void O65SetOS (O65Desc* D, unsigned OS)
/* Set an option describing the target operating system */
{
    static const unsigned char OSA65 [2] = { O65OS_OSA65, 0 };
    static const unsigned char Lunix [2] = { O65OS_LUNIX, 0 };

    /* Write the correct option */
    switch (OS) {

	case O65OS_OSA65:
	    O65SetOption (D, O65OPT_OS, OSA65, sizeof (OSA65));
	    break;

    	case O65OS_LUNIX:
	    O65SetOption (D, O65OPT_OS, Lunix, sizeof (Lunix));
	    break;

	default:
	    Internal ("Trying to set invalid O65 operating system: %u", OS);

    }
}



ExtSym* O65GetImport (O65Desc* D, const char* Ident)
/* Return the imported symbol or NULL if not found */
{
    /* Retrieve the symbol from the table */
    return GetExtSym (D->Imports, Ident);
}



void O65SetImport (O65Desc* D, const char* Ident)
/* Set an imported identifier */
{
    /* Insert the entry into the table */
    NewExtSym (D->Imports, Ident);
}



ExtSym* O65GetExport (O65Desc* D, const char* Ident)
/* Return the exported symbol or NULL if not found */
{
    /* Retrieve the symbol from the table */
    return GetExtSym (D->Exports, Ident);
}



void O65SetExport (O65Desc* D, const char* Ident)
/* Set an exported identifier */
{
    /* Insert the entry into the table */
    NewExtSym (D->Exports, Ident);
}



static void O65SetupSegments (O65Desc* D, Memory* M)
/* Setup segment assignments */
{
    MemListNode* N;
    SegDesc* S;
    unsigned TextIdx, DataIdx, BssIdx, ZPIdx;

    /* Initialize the counters */
    D->TextCount = 0;
    D->DataCount = 0;
    D->BssCount  = 0;
    D->ZPCount   = 0;

    /* Walk through the memory list and count the segment types */
    N = M->SegList;
    while (N) {

       	/* Get the segment from the list node */
       	S = N->Seg;

       	/* Check the segment type. */
	switch (O65SegType (S)) {
	    case O65SEG_TEXT:	D->TextCount++;	break;
	    case O65SEG_DATA:	D->DataCount++; break;
	    case O65SEG_BSS:	D->BssCount++;	break;
	    case O65SEG_ZP:	D->ZPCount++;	break;
	    default:		Internal ("Invalid return from O65SegType");
      	}

	/* Next segment node */
	N = N->Next;
    }

    /* Allocate memory according to the numbers */
    D->TextSeg = Xmalloc (D->TextCount * sizeof (SegDesc*));
    D->DataSeg = Xmalloc (D->DataCount * sizeof (SegDesc*));
    D->BssSeg  = Xmalloc (D->BssCount  * sizeof (SegDesc*));
    D->ZPSeg   = Xmalloc (D->ZPCount   * sizeof (SegDesc*));

    /* Walk again through the list and setup the segment arrays */
    TextIdx = DataIdx = BssIdx = ZPIdx = 0;
    N = M->SegList;
    while (N) {

	/* Get the segment from the list node */
	S = N->Seg;

	/* Check the segment type. */
	switch (O65SegType (S)) {
	    case O65SEG_TEXT:	D->TextSeg [TextIdx++] = S;	break;
	    case O65SEG_DATA:	D->DataSeg [DataIdx++] = S;	break;
	    case O65SEG_BSS:	D->BssSeg [BssIdx++]   = S;  	break;
	    case O65SEG_ZP:	D->ZPSeg [ZPIdx++]     = S;    	break;
	    default:		Internal ("Invalid return from O65SegType");
      	}

	/* Next segment node */
	N = N->Next;
    }
}



static int O65Unresolved (const char* Name, void* D)
/* Called if an unresolved symbol is encountered */
{
    /* Check if the symbol is an imported o65 symbol */
    if (O65GetImport (D, Name) != 0) {
	/* This is an external symbol, relax... */
	return 1;
    } else {
	/* This is actually an unresolved external. Bump the counter */
	((O65Desc*) D)->Undef++;
	return 0;
    }
}



void O65WriteTarget (O65Desc* D, File* F)
/* Write an o65 output file */
{
    Memory* M;
    char OptBuf [256];	/* Buffer for option strings */
    time_t T;

    /* Place the filename in the control structure */
    D->Filename = F->Name;

    /* The o65 format uses only one memory area per file. Check that. */
    M = F->MemList;
    if (M->Next != 0) {
     	Warning ("Cannot handle more than one memory area for o65 format");
    }

    /* Check for unresolved symbols. The function O65Unresolved is called
     * if we get an unresolved symbol.
     */
    D->Undef = 0;		/* Reset the counter */
    CheckExports (O65Unresolved, D);
    if (D->Undef > 0) {
     	/* We had unresolved symbols, cannot create output file */
       	Error ("%u unresolved external(s) found - cannot create output file", D->Undef);
    }

    /* Setup the segment arrays */
    O65SetupSegments (D, M);

    /* Open the file */
    D->F = fopen (F->Name, "wb");
    if (D->F == 0) {
     	Error ("Cannot open `%s': %s", F->Name, strerror (errno));
    }

    /* Keep the user happy */
    if (Verbose) {
    	printf ("Opened `%s'...\n", F->Name);
    }

    /* Define some more options: A timestamp and the linker version */
    T = time (0);
    strcpy (OptBuf, ctime (&T));
    O65SetOption (D, O65OPT_TIMESTAMP, OptBuf, strlen (OptBuf) + 1);
    sprintf (OptBuf, "ld65 V%u.%u.%u", VER_MAJOR, VER_MINOR, VER_PATCH);
    O65SetOption (D, O65OPT_ASM, OptBuf, strlen (OptBuf) + 1);

    /* Write the header */
    O65WriteHeader (D);

    /* Write the text segment */
    O65WriteTextSeg (D, M);

    /* Write the data segment */
    O65WriteDataSeg (D, M);

    /* "Write" the bss segments */
    O65WriteBssSeg (D, M);

    /* "Write" the zeropage segments */
    O65WriteZPSeg (D, M);

    /* Write the undefined references list */
    O65WriteImports (D);

    /* Write the text segment relocation table */
    O65WriteTextReloc (D);

    /* Write the data segment relocation table */
    O65WriteDataReloc (D);

    /* Write the list of exports */
    O65WriteExports (D);

    /* Seek back to the start and write the updated header */
    fseek (D->F, 0, SEEK_SET);
    O65WriteHeader (D);

    /* Close the file */
    if (fclose (D->F) != 0) {
	Error ("Cannot write to `%s': %s", F->Name, strerror (errno));
    }

    /* Reset the file and filename */
    D->F        = 0;
    D->Filename = 0;
}




