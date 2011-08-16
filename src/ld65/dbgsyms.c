/*****************************************************************************/
/*                                                                           */
/*				   dbgsyms.c				     */
/*                                                                           */
/*		   Debug symbol handling for the ld65 linker		     */
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



#include <string.h>

/* common */
#include "addrsize.h"
#include "check.h"
#include "symdefs.h"
#include "xmalloc.h"

/* ld65 */
#include "dbgsyms.h"
#include "error.h"
#include "exports.h"
#include "expr.h"
#include "fileio.h"
#include "global.h"
#include "lineinfo.h"
#include "objdata.h"
#include "spool.h"



/*****************************************************************************/
/*     	      	    		     Data			       	     */
/*****************************************************************************/



/* Definition of the debug symbol structure */
struct DbgSym {
    unsigned            Id;             /* Id of debug symbol */
    DbgSym*    	       	Next;  		/* Pool linear list link */
    ObjData*   		Obj;	    	/* Object file that exports the name */
    Collection          LineInfos;      /* Line infos of definition */
    ExprNode*  		Expr;		/* Expression (0 if not def'd) */
    unsigned            Size;           /* Symbol size if any */
    unsigned            OwnerId;        /* Id of parent/owner */
    unsigned            ImportId;       /* Id of import if this is one */
    unsigned            Name;  	       	/* Name */
    unsigned short      Type;		/* Type of symbol */
    unsigned short      AddrSize;       /* Address size of symbol */
};

/* We will collect all debug symbols in the following array and remove
 * duplicates before outputing them.
 */
static DbgSym*	DbgSymPool[256];



/*****************************************************************************/
/*     	      	    		     Code 			       	     */
/*****************************************************************************/



static DbgSym* NewDbgSym (unsigned Id, unsigned Type, unsigned char AddrSize,
                          ObjData* O)
/* Create a new DbgSym and return it */
{
    /* Allocate memory */
    DbgSym* D     = xmalloc (sizeof (DbgSym));

    /* Initialize the fields */
    D->Id         = Id;
    D->Next       = 0;
    D->Obj        = O;
    D->LineInfos  = EmptyCollection;
    D->Expr    	  = 0;
    D->Size       = 0;
    D->OwnerId    = ~0U;
    D->ImportId   = ~0U;
    D->Name 	  = 0;
    D->Type    	  = Type;
    D->AddrSize   = AddrSize;

    /* Return the new entry */
    return D;
}



static DbgSym* GetDbgSym (DbgSym* D, long Val)
/* Check if we find the same debug symbol in the table. If we find it, return
 * a pointer to the other occurrence, if we didn't find it, return NULL.
 */
{
    /* Create the hash. We hash over the symbol value */
    unsigned Hash = ((Val >> 24) & 0xFF) ^
	       	    ((Val >> 16) & 0xFF) ^
	       	    ((Val >>  8) & 0xFF) ^
	       	    ((Val >>  0) & 0xFF);

    /* Check for this symbol */
    DbgSym* Sym = DbgSymPool[Hash];
    while (Sym) {
	/* Is this symbol identical? */
	if (Sym->Name == D->Name && EqualExpr (Sym->Expr, D->Expr)) {
	    /* Found */
	    return Sym;
	}

	/* Next symbol */
	Sym = Sym->Next;
    }

    /* This is the first symbol of it's kind */
    return 0;
}



static void InsertDbgSym (DbgSym* D, long Val)
/* Insert the symbol into the hashed symbol pool */
{
    /* Create the hash. We hash over the symbol value */
    unsigned Hash = ((Val >> 24) & 0xFF) ^
	    	    ((Val >> 16) & 0xFF) ^
	    	    ((Val >>  8) & 0xFF) ^
	    	    ((Val >>  0) & 0xFF);

    /* Insert the symbol */
    D->Next = DbgSymPool [Hash];
    DbgSymPool [Hash] = D;
}



DbgSym* ReadDbgSym (FILE* F, ObjData* O, unsigned Id)
/* Read a debug symbol from a file, insert and return it */
{
    /* Read the type and address size */
    unsigned Type = ReadVar (F);
    unsigned char AddrSize = Read8 (F);

    /* Create a new debug symbol */
    DbgSym* D = NewDbgSym (Id, Type, AddrSize, O);

    /* Read the id of the owner scope/symbol */
    D->OwnerId = ReadVar (F);

    /* Read and assign the name */
    D->Name = MakeGlobalStringId (O, ReadVar (F));

    /* Read the value */
    if (SYM_IS_EXPR (D->Type)) {
       	D->Expr = ReadExpr (F, O);
    } else {
    	D->Expr = LiteralExpr (Read32 (F), O);
    }

    /* Read the size */
    if (SYM_HAS_SIZE (D->Type)) {
        D->Size = ReadVar (F);
    }

    /* If this is an import, the file contains its id */
    if (SYM_IS_IMPORT (D->Type)) {
        D->ImportId = ReadVar (F);
    }

    /* If its an exports, there's also the export id, but we don't remember
     * it but use it to let the export point back to us.
     */
    if (SYM_IS_EXPORT (D->Type)) {
        /* Get the export from the export id, then set the our id */
        GetObjExport (O, ReadVar (F))->DbgSymId = Id;
    }

    /* Last is the list of line infos for this symbol */
    ReadLineInfoList (F, O, &D->LineInfos);

    /* Return the new DbgSym */
    return D;
}



void ClearDbgSymTable (void)
/* Clear the debug symbol table */
{
    unsigned I;
    for (I = 0; I < sizeof (DbgSymPool) / sizeof (DbgSymPool[0]); ++I) {
        DbgSym* Sym = DbgSymPool[I];
        DbgSymPool[I] = 0;
        while (Sym) {
            DbgSym* NextSym = Sym->Next;
            Sym->Next = 0;
            Sym = NextSym;
        }
    }
}



static long GetDbgSymVal (const DbgSym* D)
/* Get the value of this symbol */
{
    CHECK (D->Expr != 0);
    return GetExprVal (D->Expr);
}



void PrintDbgSyms (FILE* F)
/* Print the debug symbols in a debug file */
{
    unsigned I, J;

    for (I = 0; I < CollCount (&ObjDataList); ++I) {

        /* Get the object file */
        ObjData* O = CollAtUnchecked (&ObjDataList, I);

        /* Walk through all debug symbols in this module */
        for (J = 0; J < CollCount (&O->DbgSyms); ++J) {

            /* Get the next debug symbol */
            const DbgSym* S = CollConstAt (&O->DbgSyms, J);

            /* Emit the base data for the entry */
            fprintf (F,
                     "sym\tid=%u,name=\"%s\",addrsize=%s,type=%s",
                     O->SymBaseId + J,
                     GetString (S->Name),
                     AddrSizeToStr (S->AddrSize),
                     SYM_IS_LABEL (S->Type)? "lab" : "equ");

            /* If this is not an import, output its value and - if we have
             * it - the segment.
             */
            if (!SYM_IS_IMPORT (S->Type)) {

                SegExprDesc D;

                /* Get the symbol value */
                long Val = GetDbgSymVal (S);

                /* Output it */
                fprintf (F, ",val=0x%lX", Val);

                /* Check for a segmented expression and add the segment id to
                 * the debug info if we have one.
                 */
                GetSegExprVal (S->Expr, &D);
                if (!D.TooComplex && D.Seg != 0) {
                    fprintf (F, ",seg=%u", D.Seg->Id);
                }

                /* Output the type */
                fprintf (F, ",type=%s", SYM_IS_LABEL (S->Type)? "lab" : "equ");

            } else {

                /* Output the type */
                fputs (",type=imp", F);

            }

            /* Emit the size only if we know it */
            if (S->Size != 0) {
                fprintf (F, ",size=%u", S->Size);
            }

            /* For cheap local symbols, add the owner symbol, for others,
             * add the owner scope.
             */
            if (SYM_IS_STD (S->Type)) {
                fprintf (F, ",scope=%u", O->ScopeBaseId + S->OwnerId);
            } else {
                fprintf (F, ",parent=%u", O->SymBaseId + S->OwnerId);
            }

            /* If this is an import, output the id of the matching export */
            if (SYM_IS_IMPORT (S->Type)) {

                /* Get the import */
                const Import* Imp = GetObjImport (O, S->ImportId);

                /* Get the export from the import */
                const Export* Exp = Imp->Exp;

                /* If this is not a linker generated symbol, output the debug
                 * symbol id for the export
                 */
                if (Exp->Obj) {
                    fprintf (F, ",exp=%u", Exp->Obj->SymBaseId + Exp->DbgSymId);
                }
            }

            /* Terminate the output line */
            fputc ('\n', F);
        }
    }
}



void PrintDbgSymLabels (ObjData* O, FILE* F)
/* Print the debug symbols in a VICE label file */
{
    unsigned I;

    /* Walk through all debug symbols in this module */
    for (I = 0; I < CollCount (&O->DbgSyms); ++I) {

	long Val;

	/* Get the next debug symbol */
 	DbgSym* D = CollAt (&O->DbgSyms, I);

        /* Emit this symbol only if it is a label (ignore equates and imports) */
        if (SYM_IS_EQUATE (D->Type) || SYM_IS_IMPORT (D->Type)) {
            continue;
        }

       	/* Get the symbol value */
       	Val = GetDbgSymVal (D);

       	/* Lookup this symbol in the table. If it is found in the table, it was
       	 * already written to the file, so don't emit it twice. If it is not in
       	 * the table, insert and output it.
       	 */
       	if (GetDbgSym (D, Val) == 0) {

       	    /* Emit the VICE label line */
       	    fprintf (F, "al %06lX .%s\n", Val, GetString (D->Name));

       	    /* Insert the symbol into the table */
       	    InsertDbgSym (D, Val);
       	}
    }
}




