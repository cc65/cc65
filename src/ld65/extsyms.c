/*****************************************************************************/
/*                                                                           */
/*                                 extsyms.c                                 */
/*                                                                           */
/*      Handle program external symbols for relocatable output formats       */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1999-2011, Ullrich von Bassewitz                                      */
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
#include "hashfunc.h"
#include "xmalloc.h"

/* ld65 */
#include "error.h"
#include "extsyms.h"
#include "spool.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Structure holding an external symbol */
struct ExtSym {
    unsigned    Name;           /* Name index */
    ExtSym*     List;           /* Next entry in list of all symbols */
    ExtSym*     Next;           /* Next entry in hash list */
    unsigned    Flags;          /* Generic flags */
    unsigned    Num;            /* Number of external symbol */
};

/* External symbol table structure */
#define HASHTAB_MASK    0x3FU
#define HASHTAB_SIZE    (HASHTAB_MASK + 1)
struct ExtSymTab {
    ExtSym*     Root;           /* List of symbols */
    ExtSym*     Last;           /* Pointer to last symbol */
    unsigned    Count;          /* Number of symbols */
    ExtSym*     HashTab[HASHTAB_SIZE];
};



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



ExtSym* NewExtSym (ExtSymTab* Tab, unsigned Name)
/* Create a new external symbol and insert it into the table */
{
    /* Get the hash value of the string */
    unsigned Hash = (Name & HASHTAB_MASK);

    /* Check for duplicates */
    ExtSym* E = GetExtSym (Tab, Name);
    if (E != 0) {
        /* We do already have a symbol with this name */
        Error ("Duplicate external symbol `%s'", GetString (Name));
    }

    /* Allocate memory for the structure */
    E = xmalloc (sizeof (ExtSym));

    /* Initialize the structure */
    E->Name  = Name;
    E->List  = 0;
    E->Flags = 0;
    E->Num   = Tab->Count;

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
    E->Next = Tab->HashTab[Hash];
    Tab->HashTab[Hash] = E;

    /* Done, return the created entry */
    return E;
}



static void FreeExtSym (ExtSym* E)
/* Free an external symbol structure. Will not unlink the entry, so internal
** use only.
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
    Tab->Root   = 0;
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



ExtSym* GetExtSym (const ExtSymTab* Tab, unsigned Name)
/* Return the entry for the external symbol with the given name. Return NULL
** if there is no such symbol.
*/
{
    /* Hash the name */
    unsigned Hash = (Name & HASHTAB_MASK);

    /* Check the linked list */
    ExtSym* E = Tab->HashTab[Hash];
    while (E) {
        if (E->Name == Name) {
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
** ExtSymNext for the next symbol.
*/
{
    return Tab->Root;
}



unsigned ExtSymNum (const ExtSym* E)
/* Return the number of an external symbol */
{
    return E->Num;
}



unsigned ExtSymName (const ExtSym* E)
/* Return the symbol name index */
{
    return E->Name;
}



const ExtSym* ExtSymNext (const ExtSym* E)
/* Return the next symbol in the list */
{
    return E->List;
}
