/*****************************************************************************/
/*                                                                           */
/*                                   cpu.c                                 */
/*                                                                           */
/*                            CPU specifications                             */
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



#include <string.h>

/* common */              
#include "cpu.h"



/*****************************************************************************/
/*     	       	    		     Data			    	     */
/*****************************************************************************/



/* CPU used */
cpu_t CPU = CPU_UNKNOWN;

/* Table with target names */
const char* CPUNames [CPU_COUNT] = {
    "6502"
    "65C02",
    "65816",
    "sunplus",
};



/*****************************************************************************/
/*     	       	     		     Code			    	     */
/*****************************************************************************/



cpu_t FindCPU (const char* Name)
/* Find a CPU by name and return the target id. CPU_UNKNOWN is returned if
 * the given name is no valid target.
 */
{
    unsigned I;

    /* Check all CPU names */
    for (I = 0; I < CPU_COUNT; ++I) {
	if (strcmp (CPUNames [I], Name) == 0) {
	    return (cpu_t)I;
	}
    }

    /* Not found */
    return CPU_UNKNOWN;
}



