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

/* common */
#include "coll.h"
#include "print.h"
#include "xmalloc.h"

/* sim65 */
#include "chipdata.h"
#include "chiplib.h"
#include "error.h"
#include "chip.h"



/*****************************************************************************/
/*                                     Data                                  */
/*****************************************************************************/



/* Sorted list of all chip data structures */
static Collection Chips = STATIC_COLLECTION_INITIALIZER;

/* SimData instance */
static const SimData Sim65Data = {
    1, 		    	/* MajorVersion */
    1, 		    	/* MinorVersion */
    xmalloc,            /* void* (*Malloc) (size_t Size); */
    Warning,		/* void (*Warning) (const char* Format, ...); */
    Error		/* void (*Error) (const char* Format, ...); */
};



/*****************************************************************************/
/*                               Helper functions                            */
/*****************************************************************************/



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



/*****************************************************************************/
/*     	      	       	   	     Code				     */
/*****************************************************************************/



static Chip* NewChip (ChipLibrary* Library, const ChipData* Data)
/* Allocate a new chip structure, initialize and return it */
{
    /* Allocate memory */
    Chip* C = xmalloc (sizeof (Chip));

    /* Initialize the fields */
    C->Library   = Library;
    C->Data      = Data;
    C->Instances = EmptyCollection;

    /* Return the structure */
    return C;
}



#if 0
static void FreeChip (Chip* C)
/* ## Free the given chip structure */
{
    /* Free the structure itself */
    xfree (C);
}
#endif



void LoadChips (void)
/* Load all chips from all libraries */
{
    unsigned I, J;

    /* Walk through all libraries */
    for (I = 0; I < CollCount (&ChipLibraries); ++I) {

        /* Get the library entry */
        ChipLibrary* L = CollAt (&ChipLibraries, I);

        /* Create the chips */
        for (J = 0; J < L->ChipCount; ++J) {

            /* Get a pointer to the chip data */
            const ChipData* Data = L->Data + J;

            /* Check if the chip data has the correct version */
            if (Data->MajorVersion != CHIPDATA_VER_MAJOR) {
                Warning ("Version mismatch for `%s' (%s), expected %u, got %u",
                         Data->ChipName, L->LibName,
                         CHIPDATA_VER_MAJOR, Data->MajorVersion);
                /* Ignore this chip */
                continue;
            }

            /* Generate a new chip and insert it into the collection */
            CollAppend (&Chips, NewChip (L, Data));

	    /* Output chip name and version to keep the user happy */
	    Print (stdout, 1,
		   "Found chip `%s' version %u.%u\n",
		   Data->ChipName,
		   Data->MajorVersion,
		   Data->MinorVersion);
        }
    }

    /* Last act: Sort the chips by name */
    CollSort (&Chips, CmpChips, 0);
}



const Chip* FindChip (const char* Name)
/* Find a chip by name. Returns the Chip data structure or NULL if the chip
 * could not be found.
 */
{
    unsigned I;

    /* ## We do a linear search for now */
    for (I = 0; I < CollCount (&Chips); ++I) {

	/* Get the chip at this position */
	const Chip* C = CollConstAt (&Chips, I);

	/* Compare the name */
	if (strcmp (Name, C->Data->ChipName) == 0) {
	    /* Found */
	    return C;
	}
    }

    /* Not found */
    return 0;
}



