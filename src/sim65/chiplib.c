/*****************************************************************************/
/*                                                                           */
/*                                   chiplib.c                               */
/*                                                                           */
/*              Chip library handling for the sim65 6502 simulator           */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2002-2003 Ullrich von Bassewitz                                       */
/*               Römerstrasse 52                                             */
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



#include <dlfcn.h>

/* common */
#include "fname.h"
#include "print.h"
#include "xmalloc.h"

/* sim65 */
#include "chip.h"
#include "chippath.h"
#include "error.h"
#include "chiplib.h"



/*****************************************************************************/
/*                                     Data                                  */
/*****************************************************************************/



/* A collection containing all libraries */
Collection ChipLibraries = STATIC_COLLECTION_INITIALIZER;



/*****************************************************************************/
/*     	      	    		     Code				     */
/*****************************************************************************/



static ChipLibrary* NewChipLibrary (const char* PathName)
/* Create, initialize and return a new ChipLibrary structure */
{
    /* Allocate memory */
    ChipLibrary* L = xmalloc (sizeof (ChipLibrary));

    /* Initialize the fields */
    L->LibName   = xstrdup (FindName (PathName));
    L->PathName  = xstrdup (PathName);
    L->Handle    = 0;
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
/* Load a chip library. This includes loading the shared libary, allocating
 * and initializing the data structure, and loading all chip data from the
 * library.
 */
{
    const char* Msg;
    int (*GetChipData) (const struct ChipData**, unsigned*);
    int ErrorCode;
    const ChipData* Data;       /* Pointer to chip data */
    unsigned ChipCount;         /* Number of chips in this library */
    unsigned I;


    /* Allocate a new ChipLibrary structure */
    ChipLibrary* L = NewChipLibrary (LibName);

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
    ErrorCode = GetChipData (&Data, &ChipCount);
    if (ErrorCode != 0) {
        Error ("Function `GetChipData' in `%s' returned error %d", L->LibName, ErrorCode);
        FreeChipLibrary (L);
        return;
    }

    /* Remember the library */
    CollAppend (&ChipLibraries, L);

    /* Print some information */
    Print (stderr, 1, "Opened chip library `%s'\n", L->PathName);

    /* Create the chips */
    for (I = 0; I < ChipCount; ++I) {

        Chip* C;

        /* Get a pointer to the chip data */
        const ChipData* D = Data + I;

        /* Check if the chip data has the correct version */
        if (Data->MajorVersion != CHIPDATA_VER_MAJOR) {
            Warning ("Version mismatch for `%s' (%s), expected %u, got %u",
                     D->ChipName, L->LibName,
                     CHIPDATA_VER_MAJOR, D->MajorVersion);
            /* Ignore this chip */
            continue;
        }

        /* Generate a new chip */
        C = NewChip (L, D);

        /* Insert a reference to the chip into the library exporting it */
        CollAppend (&L->Chips, C);

        /* Output chip name and version to keep the user happy */
        Print (stdout, 1,
               "  Found `%s', version %u.%u in library `%s'\n",
               Data->ChipName,
               Data->MajorVersion,
               Data->MinorVersion,
               L->LibName);
    }
}



