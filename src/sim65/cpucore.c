/*****************************************************************************/
/*                                                                           */
/*                                 cpucore.c                                 */
/*                                                                           */
/*                     CPU definition for the simulator                      */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2003-2012, Ullrich von Bassewitz                                      */
/*                Roemerstrasse 52                                           */
/*                D-70794 Filderstadt                                        */
/* EMail:         uz@cc65.org                                                */
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
#include "xmalloc.h"

/* sim65 */
#include "cpucore.h"
#include "error.h"



/*****************************************************************************/
/*     	   	    		     Data				     */
/*****************************************************************************/



/* The actual CPU instance */
CPUCore*        CPU = 0;



/*****************************************************************************/
/*     	       	    	   	     Code				     */
/*****************************************************************************/



CPUCore* NewCPUCore (const char* Name, unsigned AddrSpaceSize)
/* Create and return a new CPU including it's address space */
{
    CPUCore* C;

    /* Make sure this is a 6502 CPU for now */
    if (strcmp (Name, "6502") != 0) {
        Error ("Unknown CPU type `%s'", Name);
    }

    /* Allocate memory */
    C = xmalloc (sizeof (*C));

    /* Initialize the data */
    C->Handle = 0;              /* ### */
    C->AS     = NewAddressSpace (AddrSpaceSize);

    /* Return the new CPU core */
    return C;
}



