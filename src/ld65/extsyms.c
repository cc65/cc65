/*****************************************************************************/
/*                                                                           */
/*				   extsyms.c				     */
/*                                                                           */
/*	Handle program external symbols for relocatable output formats	     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1999-2001 Ullrich von Bassewitz                                       */
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



#include <string.h>

/* common */
#include "hashstr.h"
#include "xmalloc.h"

/* ld65 */
#include "error.h"
#include "extsyms.h"



/*****************************************************************************/
/*     	       	    	      	     Data				     */
/*****************************************************************************/



/* Structure holding an external symbol */
struct ExtSym {
    ExtSym*  	List;		/* Next entry in list of all symbols */
    ExtSym*    	Next;  		/* Next entry in hash list */
    unsigned   	Flags;		/* Generic flags */
    unsigned 	Num;		/* Number of external symbol */
    char       	Name [1];	/* Name - dynamically allocated */
};

/* External symbol table structure */
#define HASHTAB_SIZE  	53
struct ExtSymTab {
    ExtSym*	Root;  		/* List of symbols */
    ExtSym*	Last;		/* Pointer to last symbol */
    unsigned	Count;		/* Number of symbols */
    ExtSym*	HashTab [HASHTAB_SIZE];
};



/*****************************************************************************/
/*     	      	     		     Code			       	     */
/*****************************************************************************/



ExtSym* NewExtSym (ExtSymTab* Tab, const char* Name)
/* Create a new external symbol and insert it into the table */
{
    /* Get the hash value of the string */
    unsigned Hash = HashStr (Name) % HASHTAB_SIZE;

    /* Get the length of the name */
    unsigned Len = strlen (Name);

    /* Check for duplicates */
    ExtSym* E =	GetExtSym (Tab, Name);	/* Don't care about duplicate hash here... */
    if (E != 0) {
	/* We do already have a symbol with this name */
     	Error ("Duplicate external symbol `%s'", Name);
    }

    /* Allocate memory for the structure */
    E = xmalloc (sizeof (ExtSym) + Len);

    /* Initialize the structure */
    E->List  = 0;
    E->Flags = 0;
    E->Num   = Tab->Count;
    memcpy (E->Name, Name, Len+1);

    /* Insert the entry into the list of all symbols */
    if (Tab->Last == 0) {
     	/* List is empty */
     	Tab->Root = E;
    } else {
     	/* List not empty */
       	Tab->Last->List = E;
    }
    Tab->Last = E;
    ++Tab->Count;

    /* Insert the symbol into the hash table */
    E->Next = Tab->HashTab [Hash];
    Tab->HashTab [Hash] = E;

    /* Done, return the created entry */
    return E;
}



static void FreeExtSym (ExtSym* E)
/* Free an external symbol structure. Will not unlink the entry, so internal
 * use only.
 */
{
    xfree (E);
}



ExtSymTab* NewExtSymTab (void)
/* Create a new external symbol table */
{
    unsigned I;

    /* Allocate memory */
    ExtSymTab* Tab = xmalloc (sizeof (ExtSymTab));

    /* Initialize the fields */
    Tab->Root	= 0;
    Tab->Last   = 0;
    Tab->Count  = 0;
    for (I = 0; I < HASHTAB_SIZE; ++I) {
     	Tab->HashTab [I] = 0;
    }

    /* Done, return the hash table */
    return Tab;
}



void FreeExtSymTab (ExtSymTab* Tab)
/* Free an external symbol structure */
{
    /* Free all entries */
    while (Tab->Root) {
     	ExtSym* E = Tab->Root;
     	Tab->Root = E->Next;
     	FreeExtSym (E);
    }

    /* Free the struct itself */
    xfree (Tab);
}



ExtSym* GetExtSym (const ExtSymTab* Tab, const char* Name)
/* Return the entry for the external symbol with the given name. Return NULL
 * if there is no such symbol.
 */
{
    /* Hash the name */
    unsigned Hash = HashStr (Name) % HASHTAB_SIZE;

    /* Check the linked list */
    ExtSym* E = Tab->HashTab [Hash];
    while (E) {
	if (strcmp (E->Name, Name) == 0) {
	    /* Found it */
	    break;
	}
	E = E->Next;
    }

    /* Return the symbol we found */
    return E;
}



unsigned ExtSymCount (const ExtSymTab* Tab)
/* Return the number of symbols in the table */
{
    return Tab->Count;
}



const ExtSym* ExtSymList (const ExtSymTab* Tab)
/* Return the start of the symbol list sorted by symbol number. Call
 * ExtSymNext for the next symbol.
 */
{
    return Tab->Root;
}



unsigned ExtSymNum (const ExtSym* E)
/* Return the number of an external symbol */
{
    return E->Num;
}



const char* ExtSymName (const ExtSym* E)
/* Return the symbol name */
{
    return E->Name;
}



const ExtSym* ExtSymNext (const ExtSym* E)
/* Return the next symbol in the list */
{
    return E->List;
}



