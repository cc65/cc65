/*****************************************************************************/
/*                                                                           */
/*				   target.c				     */
/*                                                                           */
/*			     Target specification			     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2003 Ullrich von Bassewitz                                       */
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
    "module",
    "atari",
    "vic20",
    "c16",
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
    "lunix",
    "atmos",
    "nes",
};



/* Table with default CPUs per target */
const cpu_t DefaultCPU[TGT_COUNT] = {
    CPU_6502,           /* none */
    CPU_6502,           /* module */
    CPU_6502,           /* atari */
    CPU_6502,           /* vic20 */
    CPU_6502,           /* c16 */
    CPU_6502,           /* c64 */
    CPU_6502,           /* c128 */
    CPU_6502,           /* ace */
    CPU_6502,           /* plus4 */
    CPU_6502,           /* cbm510 */
    CPU_6502,           /* cbm610 */
    CPU_6502,           /* pet */
    CPU_6502,           /* bbc */
    CPU_6502,           /* apple2 */
    CPU_6502,           /* geos */
    CPU_6502,           /* lunix */
    CPU_6502,           /* atmos */
    CPU_6502,           /* nes */
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





