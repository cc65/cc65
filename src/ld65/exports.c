/*****************************************************************************/
/*                                                                           */
/*				   exports.c				     */
/*                                                                           */
/*		     Exports handling for the ld65 linker		     */
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
#include <string.h>

/* common */
#include "addrsize.h"
#include "check.h"
#include "coll.h"
#include "hashstr.h"
#include "symdefs.h"
#include "xmalloc.h"

/* ld65 */
#include "condes.h"
#include "error.h"
#include "exports.h"
#include "expr.h"
#include "fileio.h"
#include "global.h"
#include "objdata.h"
#include "spool.h"



/*****************************************************************************/
/*     	      	       		     Data				     */
/*****************************************************************************/



/* Hash table */
#define HASHTAB_MASK    0x0FFFU
#define HASHTAB_SIZE   	(HASHTAB_MASK + 1)
static Export* 	       	HashTab[HASHTAB_SIZE];

/* Import management variables */
static unsigned	       	ImpCount = 0;	   	/* Import count */
static unsigned	       	ImpOpen  = 0;		/* Count of open imports */

/* Export management variables */
static unsigned	       	ExpCount = 0;	   	/* Export count */
static Export**	       	ExpPool  = 0;	  	/* Exports array */

/* Defines for the flags in Import */
#define IMP_INLIST      0x0001U                 /* Import is in exports list */

/* Defines for the flags in Export */
#define EXP_INLIST      0x0001U                 /* Export is in exports list */
#define EXP_USERMARK   	0x0002U                 /* User setable flag */



/*****************************************************************************/
/*	       			Import handling				     */
/*****************************************************************************/



static Export* NewExport (unsigned char Type, unsigned char AddrSize,
                          unsigned Name, ObjData* Obj);
/* Create a new export and initialize it */



static Import* NewImport (unsigned char AddrSize, ObjData* Obj)
/* Create a new import and initialize it */
{
    /* Allocate memory */
    Import* I = xmalloc (sizeof (Import));

    /* Initialize the fields */
    I->Next	= 0;
    I->Obj	= Obj;
    I->Exp      = 0;
    I->Name     = INVALID_STRING_ID;
    I->Flags    = 0;
    I->AddrSize = AddrSize;

    /* Return the new structure */
    return I;
}



void InsertImport (Import* I)
/* Insert an import into the table */
{
    Export* E;

    /* As long as the import is not inserted, V.Name is valid */
    unsigned Name = I->Name;

    /* Create a hash value for the given name */
    unsigned Hash = (Name & HASHTAB_MASK);

    /* Search through the list in that slot and print matching duplicates */
    if (HashTab[Hash] == 0) {
    	/* The slot is empty, we need to insert a dummy export */
       	E = HashTab[Hash] = NewExport (0, ADDR_SIZE_DEFAULT, Name, 0);
	++ExpCount;
    } else {
    	E = HashTab [Hash];
    	while (1) {
    	    if (E->Name == Name) {
    	  	/* We have an entry, L points to it */
       	       	break;
    	    }
    	    if (E->Next == 0) {
    	  	/* End of list an entry not found, insert a dummy */
    	  	E->Next = NewExport (0, ADDR_SIZE_DEFAULT, Name, 0);
    	  	E = E->Next;   		/* Point to dummy */
		++ExpCount;    		/* One export more */
       	  	break;
    	    } else {
    	  	E = E->Next;
    	    }
    	}
    }

    /* Ok, E now points to a valid exports entry for the given import. Insert
     * the import into the imports list and update the counters.
     */
    I->Exp     = E;
    I->Next    = E->ImpList;
    E->ImpList = I;
    E->ImpCount++;
    ++ImpCount;	   	       	/* Total import count */
    if (E->Expr == 0) {
       	/* This is a dummy export */
    	++ImpOpen;
    }

    /* Mark the import so we know it's in the list */
    I->Flags |= IMP_INLIST;
}



void FreeImport (Import* I)
/* Free an import. NOTE: This won't remove the import from the exports table,
 * so it may only be called for unused imports (imports from modules that
 * aren't referenced).
 */
{
    /* Safety */
    PRECONDITION ((I->Flags & IMP_INLIST) == 0);

    /* Free the struct */
    xfree (I);
}



Import* ReadImport (FILE* F, ObjData* Obj)
/* Read an import from a file and return it */
{
    Import* I;

    /* Read the import address size */
    unsigned char AddrSize = Read8 (F);

    /* Create a new import */
    I = NewImport (AddrSize, Obj);

    /* Read the name */
    I->Name = MakeGlobalStringId (Obj, ReadVar (F));

    /* Read the file position */
    ReadFilePos (F, &I->Pos);

    /* Check the address size */
    if (I->AddrSize == ADDR_SIZE_DEFAULT || I->AddrSize > ADDR_SIZE_LONG) {
        /* Beware: This function may be called in cases where the object file
         * is not read completely into memory. In this case, the file list is
         * invalid. Be sure not to access it in this case.
         */
        if (ObjHasFiles (I->Obj)) {
            Error ("Invalid import size in for `%s', imported from %s(%lu): 0x%02X",
                   GetString (I->Name),
                   GetSourceFileName (I->Obj, I->Pos.Name),
                   I->Pos.Line,
                   I->AddrSize);
        } else {
            Error ("Invalid import size in for `%s', imported from %s: 0x%02X",
                   GetString (I->Name),
                   GetObjFileName (I->Obj),
                   I->AddrSize);
        }
    }

    /* Return the new import */
    return I;
}



/*****************************************************************************/
/*     	      	       	   	     Code  	      	  	  	     */
/*****************************************************************************/



static Export* NewExport (unsigned char Type, unsigned char AddrSize,
                          unsigned Name, ObjData* Obj)
/* Create a new export and initialize it */
{
    /* Allocate memory */
    Export* E = xmalloc (sizeof (Export));

    /* Initialize the fields */
    E->Name     = Name;
    E->Next     = 0;
    E->Flags   	= 0;
    E->Obj      = Obj;
    E->ImpCount = 0;
    E->ImpList  = 0;
    E->Expr    	= 0;
    E->Type    	= Type;
    E->AddrSize = AddrSize;
    memset (E->ConDes, 0, sizeof (E->ConDes));

    /* Return the new entry */
    return E;
}



void FreeExport (Export* E)
/* Free an export. NOTE: This won't remove the export from the exports table,
 * so it may only be called for unused exports (exports from modules that
 * aren't referenced).
 */
{
    /* Safety */
    PRECONDITION ((E->Flags & EXP_INLIST) == 0);

    /* Free the export expression */
    FreeExpr (E->Expr);

    /* Free the struct */
    xfree (E);
}



void InsertExport (Export* E)
/* Insert an exported identifier and check if it's already in the list */
{
    Export* L;
    Export* Last;
    Import* Imp;
    unsigned Hash;

    /* Mark the export as inserted */
    E->Flags |= EXP_INLIST;

    /* Insert the export into any condes tables if needed */
    if (IS_EXP_CONDES (E->Type)) {
       	ConDesAddExport (E);
    }

    /* Create a hash value for the given name */
    Hash = (E->Name & HASHTAB_MASK);

    /* Search through the list in that slot */
    if (HashTab[Hash] == 0) {
      	/* The slot is empty */
      	HashTab[Hash] = E;
	++ExpCount;
    } else {

      	Last = 0;
      	L = HashTab[Hash];
      	do {
       	    if (L->Name == E->Name) {
      	       	/* This may be an unresolved external */
      	       	if (L->Expr == 0) {

      	       	    /* This *is* an unresolved external. Use the actual export
                     * in E instead of the dummy one in L.
                     */
      	       	    E->Next     = L->Next;
      	       	    E->ImpCount = L->ImpCount;
      	       	    E->ImpList  = L->ImpList;
      	      	    if (Last) {
      	      	       	Last->Next = E;
      	      	    } else {
      	      	       	HashTab[Hash] = E;
      	      	    }
       	       	    ImpOpen -= E->ImpCount;	/* Decrease open imports now */
      	      	    xfree (L);
      	      	    /* We must run through the import list and change the
      	       	     * export pointer now.
      	      	     */
      	      	    Imp = E->ImpList;
      	      	    while (Imp) {
      	      	   	Imp->Exp = E;
      	      	   	Imp = Imp->Next;
      	      	    }
      	      	} else {
      	      	    /* Duplicate entry, ignore it */
       	      	    Warning ("Duplicate external identifier: `%s'",
                             GetString (L->Name));
      	      	}
      	      	return;
      	    }
      	    Last = L;
      	    L = L->Next;

      	} while (L);

	/* Insert export at end of queue */
      	Last->Next = E;
	++ExpCount;
    }
}



Export* ReadExport (FILE* F, ObjData* O)
/* Read an export from a file */
{
    unsigned      ConDesCount;
    Export* E;

    /* Read the type */
    unsigned char Type = Read8 (F);

    /* Read the address size */
    unsigned char AddrSize = Read8 (F);

    /* Create a new export without a name */
    E = NewExport (Type, AddrSize, INVALID_STRING_ID, O);

    /* Read the constructor/destructor decls if we have any */
    ConDesCount = GET_EXP_CONDES_COUNT (Type);
    if (ConDesCount > 0) {

	unsigned char ConDes[CD_TYPE_COUNT];
	unsigned I;

	/* Read the data into temp storage */
	ReadData (F, ConDes, ConDesCount);

	/* Re-order the data. In the file, each decl is encoded into a byte
	 * which contains the type and the priority. In memory, we will use
	 * an array of types which contain the priority. This array was
	 * cleared by the constructor (NewExport), so we must only set the
	 * fields that contain values.
	 */
	for (I = 0; I < ConDesCount; ++I) {
	    unsigned ConDesType = CD_GET_TYPE (ConDes[I]);
	    unsigned ConDesPrio = CD_GET_PRIO (ConDes[I]);
	    E->ConDes[ConDesType] = ConDesPrio;
	}
    }

    /* Read the name */
    E->Name = MakeGlobalStringId (O, ReadVar (F));

    /* Read the value */
    if (IS_EXP_EXPR (Type)) {
       	E->Expr = ReadExpr (F, O);
    } else {
     	E->Expr = LiteralExpr (Read32 (F), O);
    }

    /* Last is the file position where the definition was done */
    ReadFilePos (F, &E->Pos);

    /* Return the new export */
    return E;
}



Export* CreateConstExport (unsigned Name, long Value)
/* Create an export for a literal date */
{
    /* Create a new export */
    Export* E = NewExport (EXP_CONST | EXP_EQUATE, ADDR_SIZE_ABS, Name, 0);

    /* Assign the value */
    E->Expr = LiteralExpr (Value, 0);

    /* Insert the export */
    InsertExport (E);

    /* Return the new export */
    return E;
}



Export* CreateMemoryExport (unsigned Name, Memory* Mem, unsigned long Offs)
/* Create an relative export for a memory area offset */
{
    /* Create a new export */
    Export* E = NewExport (EXP_EXPR | EXP_LABEL, ADDR_SIZE_ABS, Name, 0);

    /* Assign the value */
    E->Expr = MemoryExpr (Mem, Offs, 0);

    /* Insert the export */
    InsertExport (E);

    /* Return the new export */
    return E;
}



Export* CreateSegmentExport (unsigned Name, Segment* Seg, unsigned long Offs)
/* Create a relative export to a segment */
{
    /* Create a new export */
    Export* E = NewExport (EXP_EXPR | EXP_LABEL, Seg->AddrSize, Name, 0);

    /* Assign the value */
    E->Expr = SegmentExpr (Seg, Offs, 0);

    /* Insert the export */
    InsertExport (E);

    /* Return the new export */
    return E;
}



Export* CreateSectionExport (unsigned Name, Section* Sec, unsigned long Offs)
/* Create a relative export to a section */
{
    /* Create a new export */
    Export* E = NewExport (EXP_EXPR | EXP_LABEL, Sec->AddrSize, Name, 0);

    /* Assign the value */
    E->Expr = SectionExpr (Sec, Offs, 0);

    /* Insert the export */
    InsertExport (E);

    /* Return the new export */
    return E;
}



Export* FindExport (unsigned Name)
/* Check for an identifier in the list. Return 0 if not found, otherwise
 * return a pointer to the export.
 */
{
    /* Get a pointer to the list with the symbols hash value */
    Export* L = HashTab[Name & HASHTAB_MASK];
    while (L) {
        /* Search through the list in that slot */
       	if (L->Name == Name) {
	    /* Entry found */
	    return L;
	}
	L = L->Next;
    }

    /* Not found */
    return 0;
}



int IsUnresolved (unsigned Name)
/* Check if this symbol is an unresolved export */
{
    /* Find the export */
    return IsUnresolvedExport (FindExport (Name));
}



int IsUnresolvedExport (const Export* E)
/* Return true if the given export is unresolved */
{
    /* Check if it's unresolved */
    return E != 0 && E->Expr == 0;
}



int IsConstExport (const Export* E)
/* Return true if the expression associated with this export is const */
{
    if (E->Expr == 0) {
     	/* External symbols cannot be const */
     	return 0;
    } else {
        return IsConstExpr (E->Expr);
    }
}



long GetExportVal (const Export* E)
/* Get the value of this export */
{
    if (E->Expr == 0) {
     	/* OOPS */
       	Internal ("`%s' is an undefined external", GetString (E->Name));
    }
    return GetExprVal (E->Expr);
}



static void CheckSymType (const Export* E)
/* Check the types for one export */
{
    /* External with matching imports */
    Import* Imp = E->ImpList;
    while (Imp) {
       	if (E->AddrSize != Imp->AddrSize) {
	    /* Export is ZP, import is abs or the other way round */
	    if (E->Obj) {
	      	/* User defined export */
       	       	Warning ("Address size mismatch for `%s', export in "
			 "%s(%lu), import in %s(%lu)",
			 GetString (E->Name),
                         GetSourceFileName (E->Obj, E->Pos.Name),
    			 E->Pos.Line,
                         GetSourceFileName (Imp->Obj, Imp->Pos.Name),
		   	 Imp->Pos.Line);
	    } else {
		/* Export created by the linker */
		Warning ("Address size mismatch for `%s', imported from %s(%lu)",
			 GetString (E->Name),
                         GetSourceFileName (Imp->Obj, Imp->Pos.Name),
			 Imp->Pos.Line);
	    }
	}
	Imp = Imp->Next;
    }
}



static void CheckSymTypes (void)
/* Check for symbol tape mismatches */
{
    unsigned I;

    /* Print all open imports */
    for (I = 0; I < ExpCount; ++I) {
	const Export* E = ExpPool [I];
	if (E->Expr != 0 && E->ImpCount > 0) {
	    /* External with matching imports */
	    CheckSymType (E);
	}
    }
}



static void PrintUnresolved (ExpCheckFunc F, void* Data)
/* Print a list of unresolved symbols. On unresolved symbols, F is
 * called (see the comments on ExpCheckFunc in the data section).
 */
{
    unsigned I;

    /* Print all open imports */
    for (I = 0; I < ExpCount; ++I) {
	Export* E = ExpPool [I];
	if (E->Expr == 0 && E->ImpCount > 0 && F (E->Name, Data) == 0) {
	    /* Unresolved external */
	    Import* Imp = E->ImpList;
	    fprintf (stderr,
		     "Unresolved external `%s' referenced in:\n",
		     GetString (E->Name));
	    while (Imp) {
		const char* Name = GetSourceFileName (Imp->Obj, Imp->Pos.Name);
		fprintf (stderr, "  %s(%lu)\n", Name, Imp->Pos.Line);
		Imp = Imp->Next;
	    }
	}
    }
}



static int CmpExpName (const void* K1, const void* K2)
/* Compare function for qsort */
{
    return strcmp (GetString ((*(Export**)K1)->Name),
                   GetString ((*(Export**)K2)->Name));
}



static void CreateExportPool (void)
/* Create an array with pointer to all exports */
{
    unsigned I, J;

    /* Allocate memory */
    if (ExpPool) {
	xfree (ExpPool);
    }
    ExpPool = xmalloc (ExpCount * sizeof (Export*));

    /* Walk through the list and insert the exports */
    for (I = 0, J = 0; I < sizeof (HashTab) / sizeof (HashTab [0]); ++I) {
	Export* E = HashTab[I];
	while (E) {
	    CHECK (J < ExpCount);
       	    ExpPool[J++] = E;
	    E = E->Next;
	}
    }

    /* Sort them by name */
    qsort (ExpPool, ExpCount, sizeof (Export*), CmpExpName);
}



void CheckExports (ExpCheckFunc F, void* Data)
/* Check if there are any unresolved symbols. On unresolved symbols, F is
 * called (see the comments on ExpCheckFunc in the data section).
 */
{
    /* Create an export pool */
    CreateExportPool ();

    /* Check for symbol type mismatches */
    CheckSymTypes ();

    /* Check for unresolved externals (check here for special bin formats) */
    if (ImpOpen != 0) {
       	/* Print all open imports */
	PrintUnresolved (F, Data);
    }
}



static char GetAddrSizeCode (unsigned char AddrSize)
/* Get a one char code for the address size */
{
    switch (AddrSize) {
        case ADDR_SIZE_ZP:      return 'Z';
        case ADDR_SIZE_ABS:     return 'A';
        case ADDR_SIZE_FAR:     return 'F';
        case ADDR_SIZE_LONG:    return 'L';
        default:
            Internal ("Invalid address size: %u", AddrSize);
	    /* NOTREACHED */
	    return '-';
    }
}



void PrintExportMap (FILE* F)
/* Print an export map to the given file */
{
    unsigned I;
    unsigned Count;

    /* Print all exports */
    Count = 0;
    for (I = 0; I < ExpCount; ++I) {
     	const Export* E = ExpPool [I];

	/* Print unreferenced symbols only if explictly requested */
	if (VerboseMap || E->ImpCount > 0 || IS_EXP_CONDES (E->Type)) {
	    fprintf (F,
	      	     "%-25s %06lX %c%c%c%c   ",
	      	     GetString (E->Name),
	      	     GetExportVal (E),
	      	     E->ImpCount? 'R' : ' ',
		     IS_EXP_LABEL (E->Type)? 'L' : 'E',
       	       	     GetAddrSizeCode (E->AddrSize),
		     IS_EXP_CONDES (E->Type)? 'I' : ' ');
	    if (++Count == 2) {
	      	Count = 0;
	      	fprintf (F, "\n");
	    }
	}
    }
    fprintf (F, "\n");
}



void PrintImportMap (FILE* F)
/* Print an import map to the given file */
{
    unsigned I;
    const Import* Imp;

    /* Loop over all exports */
    for (I = 0; I < ExpCount; ++I) {

	/* Get the export */
     	const Export* Exp = ExpPool [I];

	/* Print the symbol only if there are imports, or if a verbose map
	 * file is requested.
	 */
	if (VerboseMap || Exp->ImpCount > 0) {

	    /* Print the export */
	    fprintf (F,
	      	     "%s (%s):\n",
	      	     GetString (Exp->Name),
	      	     GetObjFileName (Exp->Obj));

	    /* Print all imports for this symbol */
	    Imp = Exp->ImpList;
	    while (Imp) {

	      	/* Print the import */
	      	fprintf (F,
	      		 "    %-25s %s(%lu)\n",
	      		 GetObjFileName (Imp->Obj),
	      		 GetSourceFileName (Imp->Obj, Imp->Pos.Name),
	      	       	 Imp->Pos.Line);

	      	/* Next import */
	      	Imp = Imp->Next;
	    }
	}
    }
    fprintf (F, "\n");
}



void PrintExportLabels (FILE* F)
/* Print the exports in a VICE label file */
{
    unsigned I;

    /* Print all exports */
    for (I = 0; I < ExpCount; ++I) {
 	const Export* E = ExpPool [I];
       	fprintf (F, "al %06lX .%s\n", GetExportVal (E), GetString (E->Name));
    }
}



void MarkExport (Export* E)
/* Mark the export */
{
    E->Flags |= EXP_USERMARK;
}



void UnmarkExport (Export* E)
/* Remove the mark from the export */
{
    E->Flags &= ~EXP_USERMARK;
}



int ExportHasMark (Export* E)
/* Return true if the export has a mark */
{
    return (E->Flags & EXP_USERMARK) != 0;
}



void CircularRefError (const Export* E)
/* Print an error about a circular reference using to define the given export */
{
    Error ("Circular reference for symbol `%s', %s(%lu)",
	   GetString (E->Name),
           GetSourceFileName (E->Obj, E->Pos.Name),
           E->Pos.Line);
}




