/*****************************************************************************/
/*                                                                           */
/*				   dbgsyms.c				     */
/*                                                                           */
/*		   Debug symbol handing for the ld65 linker		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998     Ullrich von Bassewitz                                        */
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



#include <string.h>

#include "../common/symdefs.h"
#include "../common/xmalloc.h"

#include "global.h"
#include "error.h"
#include "fileio.h"
#include "objdata.h"
#include "expr.h"
#include "dbgsyms.h"



/*****************************************************************************/
/*     	      	    		     Data			       	     */
/*****************************************************************************/



/* We will collect all debug symbols in the following array and remove
 * duplicates before outputing them.
 */
static DbgSym*	DbgSymPool [256];



/*****************************************************************************/
/*     	      	    		     Code			       	     */
/*****************************************************************************/



static DbgSym* NewDbgSym (unsigned char Type, const char* Name, ObjData* O)
/* Create a new DbgSym and return it */
{
    /* Get the length of the symbol name */
    unsigned Len = strlen (Name);

    /* Allocate memory */
    DbgSym* D = xmalloc (sizeof (DbgSym) + Len);

    /* Initialize the fields */
    D->Next     = 0;
    D->Flags	= 0;
    D->Obj      = O;
    D->Expr    	= 0;
    D->Type    	= Type;
    memcpy (D->Name, Name, Len);
    D->Name [Len] = '\0';

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
    DbgSym* Sym = DbgSymPool [Hash];
    while (Sym) {
	/* Is this symbol identical? */
	if (strcmp (Sym->Name, D->Name) == 0 && EqualExpr (Sym->Expr, D->Expr)) {
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



DbgSym* ReadDbgSym (FILE* F, ObjData* O)
/* Read a debug symbol from a file, insert and return it */
{
    unsigned char Type;
    char Name [256];
    DbgSym* D;

    /* Read the type */
    Type = Read8 (F);

    /* Read the name */
    ReadStr (F, Name);

    /* Create a new export */
    D = NewDbgSym (Type, Name, O);

    /* Read the value */
    if (Type & EXP_EXPR) {
       	D->Expr = ReadExpr (F, O);
    } else {
    	D->Expr = LiteralExpr (Read32 (F), O);
    }

    /* Last is the file position where the definition was done */
    ReadFilePos (F, &D->Pos);

    /* Return the new DbgSym */
    return D;
}



long GetDbgSymVal (DbgSym* D)
/* Get the value of this symbol */
{
    CHECK (D->Expr != 0);
    return GetExprVal (D->Expr);
}



void PrintDbgSymLabels (ObjData* O, FILE* F)
/* Print the debug symbols in a VICE label file */
{
    unsigned I;

    /* Walk through all debug symbols in this module */
    for (I = 0; I < O->DbgSymCount; ++I) {

	/* Get the next debug symbol */
 	DbgSym* D = O->DbgSyms [I];

	/* Get the symbol value */
	long Val = GetDbgSymVal (D);

	/* Lookup this symbol in the table. If it is found in the table, it was
	 * already written to the file, so don't emit it twice. If it is not in
	 * the table, insert and output it.
	 */
       	if (GetDbgSym (D, Val) == 0) {

	    /* Emit the VICE label line */
       	    fprintf (F, "al %06lX .%s\n", Val, D->Name);

	    /* Insert the symbol into the table */
	    InsertDbgSym (D, Val);
       	}
    }
}



