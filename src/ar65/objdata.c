/*****************************************************************************/
/*                                                                           */
/*				   objdata.c				     */
/*                                                                           */
/*		Handling object file data for the ar65 archiver		     */
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

#include "mem.h"
#include "error.h"
#include "objdata.h"



/*****************************************************************************/
/*     	      	    		     Data				     */
/*****************************************************************************/



/* Object data list management */
unsigned    	ObjCount = 0;	/* Count of object files in the list */
ObjData*    	ObjRoot  = 0;	/* List of object files */
ObjData*    	ObjLast  = 0;	/* Last entry in list */
ObjData**      	ObjPool  = 0;	/* Object files as array */



/*****************************************************************************/
/*     	      	    		     Code				     */
/*****************************************************************************/



ObjData* NewObjData (void)
/* Allocate a new structure on the heap, insert it into the list, return it */
{
    /* Allocate memory */
    ObjData* O = Xmalloc (sizeof (ObjData));

    /* Initialize the data */
    O->Next       = 0;
    O->Name  	  = 0;
    O->Index	  = ~0;
    O->Flags   	  = 0;
    O->MTime 	  = 0;
    O->Start	  = 0;
    O->Size 	  = 0;
    O->ImportSize = 0;
    O->Imports    = 0;
    O->ExportSize = 0;
    O->Exports    = 0;

    /* Link it into the list */
    if (ObjLast) {
	ObjLast->Next = O;
	ObjLast       = O;
    } else {
	/* First entry */
	ObjRoot = ObjLast = O;
    }

    /* One object file more now */
    ++ObjCount;

    /* Return the new entry */
    return O;
}



void FreeObjData (ObjData* O)
/* Free a complete struct */
{
    Xfree (O->Name);
    Xfree (O->Imports);
    Xfree (O->Exports);
    Xfree (O);
}



ObjData* FindObjData (const char* Module)
/* Search for the module with the given name and return it. Return NULL if the
 * module is not in the list.
 */
{
    /* Hmm. Maybe we should hash the module names? */
    ObjData* O = ObjRoot;
    while (O) {
     	if (strcmp (O->Name, Module) == 0) {
     	    return O;
     	}
	O = O->Next;
    }
    return 0;
}



void DelObjData (const char* Module)
/* Delete the object module from the list */
{
    ObjData* O = ObjRoot;
    ObjData* Last = 0;
    while (O) {
	if (strcmp (O->Name, Module) == 0) {
	    /* Found the module, remove it from the list */
	    if (Last == 0) {
	       	/* This was the first entry in the list */
	       	ObjRoot = O->Next;
	    } else {
	       	Last->Next = O->Next;
	    }
	    if (ObjLast == O) {
	       	/* O was the last object in the list */
	       	ObjLast = Last;
	    }
	    --ObjCount;

	    /* Free the entry */
	    FreeObjData (O);

	    /* Done */
	    return;
	}
	Last = O;
	O = O->Next;
    }

    /* Not found! */
    Warning ("Module `%s' not found in library", Module);
}



void MakeObjPool (void)
/* Allocate memory, index the entries and make the ObjPool valid */
{
    ObjData* O;
    unsigned Index;

    /* Allocate memory for the pool */
    ObjPool = Xmalloc (ObjCount * sizeof (ObjData*));

    /* Setup the pointers and index the objects */
    Index = 0;
    O = ObjRoot;
    while (O) {

	/* Safety */
	CHECK (Index < ObjCount);

	/* Set the Index */
	O->Index = Index;

	/* Set the pool pointer */
	ObjPool [Index] = O;

     	/* Next object */
     	++Index;
     	O = O->Next;
    }
}



const char* GetObjName (unsigned Index)
/* Get the name of a module by index */
{
    PRECONDITION (ObjPool != 0 && Index < ObjCount && ObjPool [Index] != 0);
    return ObjPool [Index]->Name;
}





