/*****************************************************************************/
/*                                                                           */
/*                                location.h                                 */
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



#ifndef LOCATION_H
#define LOCATION_H



/* common.h */
#include "coll.h"



/*****************************************************************************/
/*                                     Data                                  */
/*****************************************************************************/



/* List of all memory locations */
extern Collection Locations;

/* One memory location */
typedef struct Location Location;
struct Location {
    unsigned    Start;          /* Start of memory location */
    unsigned    End;            /* End memory location */
    Collection  Attributes;     /* Attributes given */
    unsigned    Line;           /* Line in config file */
    unsigned    Col;            /* Column in config file */
};



/*****************************************************************************/
/*     	      	    		     Code		  		     */
/*****************************************************************************/



Location* NewLocation (unsigned long Start, unsigned long End);
/* Create a new location, initialize and return it */

int LocationGetAttr (const Location* L, const char* AttrName);
/* Find the attribute with the given name and return it. Call Error() if the
 * attribute was not found.
 */

int LocationIsMirror (const Location* L);
/* Return true if the given location is a mirror of another one. */

void LocationSort (Collection* Locations);
/* Sort all locations by address */

void LocationCheck (const Collection* Locations);
/* Check all locations for problems */



/* End of location.h */

#endif



