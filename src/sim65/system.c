/*****************************************************************************/
/*                                                                           */
/*                                 system.c                                  */
/*                                                                           */
/*                    Description of the simulated system                    */
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
#include "xmalloc.h"

/* sim65 */
#include "addrspace.h"
#include "system.h"



/*****************************************************************************/
/*     	      	    		     Code		  		     */
/*****************************************************************************/



System* NewSystem (struct CPUData* CPU)
/* Create and initialize a new System struct. The function will read the size
 * of the address space from the CPU, and also create a new AddressSpace
 * object. No chips are assigned, however.
 */
{
    /* Allocate memory */
    System* Sys = xmalloc (sizeof (System));

    /* Initialize the fields */
    Sys->CPU      = CPU;
    Sys->AS       = 0;    /* ### */
    Sys->ChipInstances = AUTO_COLLECTION_INITIALIZER;

    /* Return the new system */
    return Sys;
}




