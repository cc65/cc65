/*****************************************************************************/
/*                                                                           */
/*                                 objdata.c                                 */
/*                                                                           */
/*              Handling object file data for the ar65 archiver              */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2012, Ullrich von Bassewitz                                      */
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
#include "check.h"
#include "xmalloc.h"

/* ar65 */
#include "error.h"
#include "library.h"
#include "objdata.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Collection with object files */
Collection       ObjPool        = STATIC_COLLECTION_INITIALIZER;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



ObjData* NewObjData (void)
/* Allocate a new structure on the heap, insert it into the list, return it */
{
    /* Allocate memory */
    ObjData* O = xmalloc (sizeof (ObjData));

    /* Initialize the data */
    O->Name        = 0;

    O->Flags       = 0;
    O->MTime       = 0;
    O->Start       = 0;
    O->Size        = 0;

    O->Strings     = EmptyCollection;
    O->Exports     = EmptyCollection;

    /* Add it to the list */
    CollAppend (&ObjPool, O);

    /* Return the new entry */
    return O;
}



void FreeObjData (ObjData* O)
/* Free a complete struct */
{
    unsigned I;

    xfree (O->Name);
    for (I = 0; I < CollCount (&O->Strings); ++I) {
        xfree (CollAt (&O->Strings, I));
    }
    DoneCollection (&O->Strings);
    DoneCollection (&O->Exports);
    xfree (O);
}



void ClearObjData (ObjData* O)
/* Remove any data stored in O */
{
    unsigned I;
    xfree (O->Name);
    O->Name = 0;
    for (I = 0; I < CollCount (&O->Strings); ++I) {
        xfree (CollAt (&O->Strings, I));
    }
    CollDeleteAll (&O->Strings);
    CollDeleteAll (&O->Exports);
}



ObjData* FindObjData (const char* Module)
/* Search for the module with the given name and return it. Return NULL if the
** module is not in the list.
*/
{
    unsigned I;

    /* Hmm. Maybe we should hash the module names? */
    for (I = 0; I < CollCount (&ObjPool); ++I) {

        /* Get this object file */
        ObjData* O = CollAtUnchecked (&ObjPool, I);

        /* Did we find it? */
        if (strcmp (O->Name, Module) == 0) {
            return O;
        }
    }
    return 0;
}



void DelObjData (const char* Module)
/* Delete the object module from the list */
{
    unsigned I;
    for (I = 0; I < CollCount (&ObjPool); ++I) {

        /* Get this object file */
        ObjData* O = CollAtUnchecked (&ObjPool, I);

        /* Did we find it? */
        if (strcmp (O->Name, Module) == 0) {

            /* Free the entry */
            CollDelete (&ObjPool, I);
            FreeObjData (O);

            /* Done */
            return;
        }
    }

    /* Not found! */
    Warning ("Module `%s' not found in library `%s'", Module, LibName);
}
