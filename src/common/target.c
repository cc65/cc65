/*****************************************************************************/
/*                                                                           */
/*				   target.c				     */
/*                                                                           */
/*			     Target specification			     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000      Ullrich von Bassewitz                                       */
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



#include <stdlib.h>
#include <string.h>

#include "chartype.h"
#include "target.h"



/*****************************************************************************/
/*     	      	    		     Data			    	     */
/*****************************************************************************/



/* Target system */
target_t Target	= TGT_NONE;

/* Table with target names */
const char* TargetNames [TGT_COUNT] = {
    "none",
    "atari",
    "c64",
    "c128",
    "ace",
    "plus4",
    "cbm510",
    "cbm610",
    "pet",
    "bbc",
    "apple2",
    "geos",
};



/*****************************************************************************/
/*     	      	    		     Code			    	     */
/*****************************************************************************/



target_t FindTarget (const char* Name)
/* Find a target by name and return the target id. TGT_UNKNOWN is returned if
 * the given name is no valid target.
 */
{
    unsigned I;

    /* Check for a numeric target */
    if (IsDigit (*Name)) {
       	int Target = atoi (Name);
	if (Target >= 0 && Target < TGT_COUNT) {
	    return (target_t)Target;
	}
    }

    /* Check for a target string */
    for (I = 0; I < TGT_COUNT; ++I) {
	if (strcmp (TargetNames [I], Name) == 0) {
	    return (target_t)I;
	}
    }

    /* Not found */
    return TGT_UNKNOWN;
}


	     
