/*****************************************************************************/
/*                                                                           */
/*                                location.c                                 */
/*                                                                           */
/*                        Memory location description                        */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2003      Ullrich von Bassewitz                                       */
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



/* common.h */
#include "coll.h"
#include "xmalloc.h"

/* sim65 */
#include "cfgdata.h"
#include "error.h"
#include "scanner.h"
#include "location.h"



/*****************************************************************************/
/*                                     Data                                  */
/*****************************************************************************/



/* List of all memory locations */
Collection Locations = STATIC_COLLECTION_INITIALIZER;



/*****************************************************************************/
/*     	      	    		     Code		  		     */
/*****************************************************************************/



Location* NewLocation (unsigned long Start, unsigned long End)
/* Create a new location, initialize and return it */
{
    /* Allocate memory */
    Location* L = xmalloc (sizeof (Location));

    /* Initialize the fields */
    L->Start      = Start;
    L->End        = End;
    L->Attributes = EmptyCollection;
    L->Line       = CfgErrorLine;
    L->Col        = CfgErrorCol;

    /* Return the new struct */
    return L;
}



static int CmpLocations (void* Data attribute ((unused)),
		         const void* lhs, const void* rhs)
/* Compare function for CollSort */
{
    /* Cast the object pointers */
    const Location* Left  = (const Location*) rhs;
    const Location* Right = (const Location*) lhs;

    /* Do the compare */
    if (Left->Start < Right->Start) {
        return 1;
    } else if (Left->Start > Right->Start) {
        return -1;
    } else {
        return 0;
    }
}



int LocationGetAttr (const Location* L, const char* AttrName)
/* Find the attribute with the given name and return it. Call Error() if the
 * attribute was not found.
 */
{
    int I = CfgDataFind (&L->Attributes, AttrName);
    if (I < 0) {
        Error ("%s(%u): Attribute `%s' missing", CfgGetName(), L->Line, AttrName);
    }
    return I;
}



int LocationIsMirror (const Location* L)
/* Return true if the given location is a mirror of another one. */
{
    /* Find the "mirror" attribute */
    return (CfgDataFind (&L->Attributes, "mirror") >= 0);
}



void LocationSort (Collection* Locations)
/* Sort all locations by address */
{
    /* Sort all memory locations */
    CollSort (Locations, CmpLocations, 0);
}



void LocationCheck (const Collection* Locations)
/* Check all locations for problems */
{
    unsigned I;

    /* Check for overlaps and other problems */
    const Location* Last = 0;
    for (I = 0; I < CollCount (Locations); ++I) {

        /* Get this location */
        const Location* L = CollConstAt (Locations, I);

        /* Check for an overlap with the following location */
        if (Last && Last->End >= L->Start) {
            Error ("%s(%u): Address range overlap (overlapping entry is in line %u)",
                   CfgGetName(), L->Line, Last->Line);
        }

        /* If the location is a mirror, it must not have other attributes,
         * and the mirror attribute must be an integer.
         */
        if (LocationIsMirror (L)) {
            const CfgData* D;
            if (CollCount (&L->Attributes) > 1) {
                Error ("%s(%u): Location at address $%06X is a mirror "
                       "but has attributes", CfgGetName(), L->Line, L->Start);
            }
            D = CollConstAt (&L->Attributes, 0);
            CfgDataCheckType (D, CfgDataNumber);
        }

        /* Remember this entry */
        Last = L;
    }
}




