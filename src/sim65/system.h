/*****************************************************************************/
/*                                                                           */
/*                                 system.h                                  */
/*                                                                           */
/*                    Description of the simulated system                    */
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



#ifndef SYSTEM_H
#define SYSTEM_H



/* common.h */
#include "coll.h"

/* sim65 */
#include "addrspace.h"
#include "cpucore.h"



/*****************************************************************************/
/*                                     Data                                  */
/*****************************************************************************/



/* Forwards */
struct CPUCore;

/* */
typedef struct System System;
struct System {
    CPUCore*            CPU;            /* The CPU in the system */
    AddressSpace*       AS;             /* The CPU address space */
    Collection          ChipInstances;  /* Instances of all the chips */
};

/* Global pointer to simulated system */
extern System*  System;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



System* NewSystem (CPUCore* CPU);
/* Create and initialize a new System struct. The function will read the size
 * of the address space from the CPU, and also create a new AddressSpace
 * object. No chips are assigned, however.
 */



/* End of system.h */

#endif



