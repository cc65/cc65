/*****************************************************************************/
/*                                                                           */
/*                                    chip.c                                 */
/*                                                                           */
/*                        Interface for the chip plugins                     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2002      Ullrich von Bassewitz                                       */
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
#include <dlfcn.h>

/* common */
#include "coll.h"
#include "print.h"
#include "xmalloc.h"

/* sim65 */
#include "chippath.h"
#include "error.h"
#include "chip.h"



/*****************************************************************************/
/*                                     Data                                  */
/*****************************************************************************/



/* Sorted list of all chip data structures */
static Collection Chips = STATIC_COLLECTION_INITIALIZER;



/*****************************************************************************/
/*                               Helper functions                            */
/*****************************************************************************/



static int CmpChips (void* Data attribute ((unused)),
		     const void* lhs, const void* rhs)
/* Compare function for CollSort */
{
    return strcmp (((const Chip*) lhs)->Name, ((const Chip*) rhs)->Name);
}



void* GetSym (const Chip* C, const char* SymName)
/* Locate a symbol in a module and return it. Abort on errors (may be modified
 * later to return NULL).
 */
{
    void* Val;
    const char* Msg;

    /* Fetch the error message and discard it - this will clear pending
     * errors
     */
    dlerror ();

    /* Fetch the symbol value */
    Val = dlsym (C->Handle, SymName);

    /* Check the error message */
    Msg = dlerror ();
    if (Msg) {
       	/* We had an error */
        Error ("Error loading `%s' from `%s': %s", SymName, C->LibName, Msg);
        return 0;
    }

    /* Return the symbol value read */
    return Val;
}



/*****************************************************************************/
/*     	      	    	   	     Code				     */
/*****************************************************************************/



static Chip* NewChip (void* Handle, const char* LibName)
/* Allocate a new chip structure, initialize and return it */
{
    /* Allocate memory */
    Chip* C = xmalloc (sizeof (Chip));

    /* Initialize the fields */
    C->Name         = 0;
    C->LibName      = xstrdup (LibName);
    C->Handle       = Handle;
    C->InitChip     = 0;
    C->GetVersion   = 0;
    C->WriteCtrl    = 0;
    C->Write        = 0;
    C->ReadCtrl     = 0;
    C->Read         = 0;

    /* Return the structure */
    return C;
}



void FreeChip (Chip* C)
/* Free the given chip structure */
{
    /* Free the strings */
    xfree (C->Name);
    xfree (C->LibName);

    /* Free the structure itself */
    xfree (C);
}



void LoadChip (const char* LibName)
/* Load a chip. This includes loading the shared libary, allocating and
 * initializing the data structure.
 */
{
    Chip* C;
    void* H;
    const char* Msg;
    unsigned Ver;
    const char* Name;

    /* Locate the library */
    char* PathName = FindChipLib (LibName);
    if (PathName == 0) {
        /* Library not found */
        Error ("Cannot find chip plugin library `%s'", LibName);
        return;
    }

    /* Open the library */
    H = dlopen (PathName, RTLD_GLOBAL | RTLD_LAZY);

    /* Check for errors */
    Msg = dlerror ();
    if (Msg) {
        Error ("Error opening `%s': %s", PathName, Msg);
    }

    /* Free the path to the library since we don't need it any longer */
    xfree (PathName);

    /* Allocate the chip structure */
    C = NewChip (H, LibName);

    /* Read function pointers */
/*    C->InitChip     = GetSym (C, "InitChip"); */
    C->GetName      = GetSym (C, "GetName");
    C->GetVersion   = GetSym (C, "GetVersion");
/*    C->WriteCtrl    = GetSym (C, "WriteCtrl"); */
/*    C->Write        = GetSym (C, "Write"); */
/*    C->ReadCtrl     = GetSym (C, "ReadCtrl"); */
/*    C->Read         = GetSym (C, "Read"); */

    /* Insert the structure into the list of all chips */
    CollAppend (&Chips, C);

    /* Call the functions */
    Name = C->GetName ();
    Ver  = C->GetVersion ();
    printf ("%s version %u\n", Name, Ver);
}



void InitChips (void)
/* Initialize the chips. Must be called *after* all chips are loaded */
{
    /* Sort the chips by name */
    CollSort (&Chips, CmpChips, 0);
}



const Chip* FindChip (const char* Name)
/* Find a chip by name. Returns the Chip data structure or NULL if the chip
 * could not be found.
 */
{
    return 0;
}



