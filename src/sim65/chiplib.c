/*****************************************************************************/
/*                                                                           */
/*                                   chiplib.c                               */
/*                                                                           */
/*              Chip library handling for the sim65 6502 simulator           */
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



#include <dlfcn.h>

/* common */
#include "print.h"
#include "xmalloc.h"

/* sim65 */
#include "chippath.h"
#include "error.h"
#include "chiplib.h"



/*****************************************************************************/
/*                                     Data                                  */
/*****************************************************************************/



/* Forwards */
struct ChipData;

/* A collection containing all libraries */
Collection ChipLibraries = STATIC_COLLECTION_INITIALIZER;



/*****************************************************************************/
/*     	      	    		     Code				     */
/*****************************************************************************/



static ChipLibrary* NewChipLibrary (const char* LibName)
/* Create, initialize and return a new ChipLibrary structure */
{
    /* Allocate memory */
    ChipLibrary* L = xmalloc (sizeof (ChipLibrary));

    /* Initialize the fields */
    L->LibName   = xstrdup (LibName);
    L->PathName  = 0;
    L->Handle    = 0;
    L->Data      = 0;
    L->ChipCount = 0;
    L->Chips     = EmptyCollection;

    /* Return the allocated structure */
    return L;
}



static void FreeChipLibrary (ChipLibrary* L)
/* Free a ChipLibrary structure */
{
    /* Free the names */
    xfree (L->LibName);
    xfree (L->PathName);

    /* If the library is open, close it. Discard any errors. */
    if (L->Handle) {
        dlclose (L->Handle);
        (void) dlerror ();
    }

    /* We may have to handle the Chip pointers, but currently the function
     * is never called with a non empty Chips collection, so we don't care
     * for now.
     */
    xfree (L);
}



void LoadChipLibrary (const char* LibName)
/* Load a chip library . This includes loading the shared libary, allocating
 * and initializing the data structure.
 */
{
    const char* Msg;
    int (*GetChipData) (const struct ChipData**, unsigned*);
    int ErrorCode;

    /* Allocate a new ChipLibrary structure */
    ChipLibrary* L = NewChipLibrary (LibName);

    /* Locate the library */
    L->PathName = FindChipLib (LibName);
    if (L->PathName == 0) {
        /* Library not found */
        Error ("Cannot find chip plugin library `%s'", LibName);
        FreeChipLibrary (L);
        return;
    }

    /* Open the library */
    L->Handle = dlopen (L->PathName, RTLD_GLOBAL | RTLD_LAZY);

    /* Check for errors */
    Msg = dlerror ();
    if (Msg) {
        Error ("Cannot open `%s': %s", L->PathName, Msg);
        FreeChipLibrary (L);
        return;
    }

    /* Locate the GetChipData function */
    GetChipData = dlsym (L->Handle, "GetChipData");

    /* Check the error message */
    Msg = dlerror ();
    if (Msg) {
       	/* We had an error */
        Error ("Cannot find export `GetChipData' in `%s': %s", L->LibName, Msg);
        FreeChipLibrary (L);
        return;
    }

    /* Call the function to read the chip data */
    ErrorCode = GetChipData (&L->Data, &L->ChipCount);
    if (ErrorCode != 0) {
        Error ("Function `GetChipData' in `%s' returned error %d", L->LibName, ErrorCode);
        FreeChipLibrary (L);
        return;
    }

    /* Remember the library */
    CollAppend (&ChipLibraries, L);

    /* Print some information */
    Print (stderr, 1, "Opened plugin library `%s'", L->PathName);
}



