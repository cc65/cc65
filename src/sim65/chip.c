/*****************************************************************************/
/*                                                                           */
/*                                    chip.c                                 */
/*                                                                           */
/*                        Interface for the chip plugins                     */
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



#include <string.h>
#include <dlfcn.h>

/* common */
#include "coll.h"
#include "fname.h"
#include "print.h"
#include "xmalloc.h"

/* sim65 */
#include "cfgdata.h"
#include "chipdata.h"
#include "cpucore.h"
#include "error.h"
#include "chip.h"



/*****************************************************************************/
/*                                 Forwards                                  */
/*****************************************************************************/



static int GetCfgId (void* CfgInfo, const char* Name, char** Id);
/* Search CfgInfo for an attribute with the given name and type "id". If
 * found, remove it from the configuration, pass a pointer to a dynamically
 * allocated string containing the value to Id, and return true. If not
 * found, return false. The memory passed in Id must be free by a call to
 * Free();
 */

static int GetCfgStr (void* CfgInfo, const char* Name, char** S);
/* Search CfgInfo for an attribute with the given name and type "id". If
 * found, remove it from the configuration, pass a pointer to a dynamically
 * allocated string containing the value to Id, and return true. If not
 * found, return false. The memory passed in S must be free by a call to
 * Free();
 */

static int GetCfgNum (void* CfgInfo, const char* Name, long* Val);
/* Search CfgInfo for an attribute with the given name and type "number".
 * If found, remove it from the configuration, copy it into Val and return
 * true. If not found, return false.
 */



/*****************************************************************************/
/*                                     Data                                  */
/*****************************************************************************/



/* Sorted list of all chip data structures */
static Collection Chips = STATIC_COLLECTION_INITIALIZER;

/* A collection containing all libraries */
static Collection ChipLibraries = STATIC_COLLECTION_INITIALIZER;

/* SimData instance */
static const SimData Sim65Data = {
    1, 		    	/* MajorVersion */
    1, 		    	/* MinorVersion */
    xmalloc,
    xfree,
    Warning,
    Error,
    Internal,
    Break,
    GetCfgId,
    GetCfgStr,
    GetCfgNum
};



/*****************************************************************************/
/*                               Helper functions                            */
/*****************************************************************************/



static int GetCfgId (void* CfgInfo, const char* Name, char** Id)
/* Search CfgInfo for an attribute with the given name and type "id". If
 * found, remove it from the configuration, pass a pointer to a dynamically
 * allocated string containing the value to Id, and return true. If not
 * found, return false. The memory passed in Id must be free by a call to
 * Free();
 */
{
    return CfgDataGetId (CfgInfo, Name, Id);
}



static int GetCfgStr (void* CfgInfo, const char* Name, char** S)
/* Search CfgInfo for an attribute with the given name and type "id". If
 * found, remove it from the configuration, pass a pointer to a dynamically
 * allocated string containing the value to Id, and return true. If not
 * found, return false. The memory passed in S must be free by a call to
 * Free();
 */
{
    return CfgDataGetStr (CfgInfo, Name, S);
}



static int GetCfgNum (void* CfgInfo, const char* Name, long* Val)
/* Search CfgInfo for an attribute with the given name and type "number".
 * If found, remove it from the configuration, copy it into Val and return
 * true. If not found, return false.
 */
{
    return CfgDataGetNum (CfgInfo, Name, Val);
}



static int CmpChips (void* Data attribute ((unused)),
		     const void* lhs, const void* rhs)
/* Compare function for CollSort */
{
    /* Cast the object pointers */
    const Chip* Left  = (const Chip*) rhs;
    const Chip* Right = (const Chip*) lhs;

    /* Do the compare */
    return strcmp (Left->Data->ChipName, Right->Data->ChipName);
}



static Chip* FindChip (const char* Name)
/* Find a chip by name. Returns the Chip data structure or NULL if the chip
 * could not be found.
 */
{
    unsigned I;

    /* ## We do a linear search for now */
    for (I = 0; I < CollCount (&Chips); ++I) {

       	/* Get the chip at this position */
    	Chip* C = CollAt (&Chips, I);

    	/* Compare the name */
    	if (strcmp (Name, C->Data->ChipName) == 0) {
    	    /* Found */
    	    return C;
    	}
    }

    /* Not found */
    return 0;
}



/*****************************************************************************/
/*     	      	       	   	     Code	   			     */
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



static Chip* NewChip (ChipLibrary* Library, const ChipData* Data)
/* Allocate a new chip structure, initialize and return it */
{
    /* Allocate memory */
    Chip* C = xmalloc (sizeof (Chip));

    /* Initialize the fields */
    C->Lib       = Library;
    C->Data      = Data;
    C->Instances = EmptyCollection;

    /* Insert the new chip into the collection of all chips */
    CollAppend (&Chips, C);

    /* Return the structure */
    return C;
}



ChipInstance* NewChipInstance (const char* ChipName, unsigned Addr,
                               unsigned Size, Collection* Attributes)
{
    ChipInstance* CI;

    /* Find the chip with the given name */
    Chip* C = FindChip (ChipName);
    if (C == 0) {
        Error ("No chip `%s' found for address $%06X", ChipName, Addr);
    }

    /* Allocate a new ChipInstance structure */
    CI = xmalloc (sizeof (*CI));

    /* Initialize the fields */
    CI->C    = C;
    CI->AS   = 0;
    CI->Addr = Addr;
    CI->Size = Size;
    CI->Data = C->Data->CreateInstance (Addr, Size, Attributes);

    /* Assign the chip instance to the chip */
    CollAppend (&C->Instances, CI);

    /* Return the new instance struct */
    return CI;
}



ChipInstance* MirrorChipInstance (const ChipInstance* Orig, unsigned Addr)
/* Generate a chip instance mirror and return it. */
{
    /* Allocate a new ChipInstance structure */
    ChipInstance* CI = xmalloc (sizeof (*CI));

    /* Initialize the fields */
    CI->C    = Orig->C;
    CI->Addr = Addr;
    CI->Size = Orig->Size;
    CI->Data = Orig->Data;

    /* Assign the chip instance to the chip */
    CollAppend (&CI->C->Instances, CI);

    /* Return the new instance struct */
    return CI;
}



void SortChips (void)
/* Sort all chips by name. Called after loading */
{
    /* Last act: Sort the chips by name */
    CollSort (&Chips, CmpChips, 0);
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
        if (D->MajorVersion != CHIPDATA_VER_MAJOR) {
            Warning ("Version mismatch for `%s' (%s), expected %u, got %u",
                     D->ChipName, L->LibName,
                     CHIPDATA_VER_MAJOR, D->MajorVersion);
            /* Ignore this chip */
            continue;
        }

	/* Initialize the chip passing the simulator data */
	D->InitChip (&Sim65Data);

        /* Generate a new chip */
        C = NewChip (L, D);

        /* Insert a reference to the chip into the library exporting it */
        CollAppend (&L->Chips, C);

        /* Output chip name and version to keep the user happy */
        Print (stdout, 1,
               "  Found %s `%s', version %u.%u in library `%s'\n",
	       (D->Type == CHIPDATA_TYPE_CHIP)? "chip" : "cpu",
               D->ChipName,
               D->MajorVersion,
               D->MinorVersion,
               L->LibName);
    }
}



