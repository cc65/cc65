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



Chip* NewChip (ChipLibrary* Library, const ChipData* Data)
/* Allocate a new chip structure, initialize and return it */
{
    /* Allocate memory */
    Chip* C = xmalloc (sizeof (Chip));

    /* Initialize the fields */
    C->Library   = Library;
    C->Data      = Data;
    C->Instances = EmptyCollection;

    /* Insert the new chip into the collection of all chips */
    CollAppend (&Chips, C);

    /* Return the structure */
    return C;
}



ChipInstance* NewChipInstance (unsigned long Addr, unsigned Size)
/* Allocate a new chip instance for the chip. */
{
    /* Allocate a new ChipInstance structure */
    ChipInstance* Instance = xmalloc (sizeof (*Instance));

    /* Initialize the fields */
    Instance->C             = 0;
    Instance->Addr          = Addr;
    Instance->Size          = Size;
    Instance->InstanceData  = 0;

    /* Return the new struct */
    return Instance;
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



void InitChipInstance (ChipInstance* CI, const char* ChipName,
                       const struct CfgData** Data, unsigned Count)
/* Initialize the given chip instance. Assign it to the chip named ChipName,
 * and call the init function of the chip passing the given config data.
 */
{
    /* Find the chip with the given name */
    Chip* C = FindChip (ChipName);
    if (C == 0) {
        Error ("No chip `%s' found for address $%6lX", ChipName, CI->Addr);
    }

    /* Call the initialization function */
    CI->InstanceData = C->Data->InitInstance (CI->Addr, CI->Size, Data, Count);

    /* Assign the chip instance to the chip */
    CI->C = C;
    CollAppend (&C->Instances, CI);
}



void SortChips (void)
/* Sort all chips by name. Called after loading */
{
    /* Last act: Sort the chips by name */
    CollSort (&Chips, CmpChips, 0);
}



