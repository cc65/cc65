/*****************************************************************************/
/*                                                                           */
/*				   target.h				     */
/*                                                                           */
/*			     Target specification			     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2004 Ullrich von Bassewitz                                       */
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



#ifndef TARGET_H
#define TARGET_H



/* common */
#include "cpu.h"



/*****************************************************************************/
/*     	       	    		     Data			    	     */
/*****************************************************************************/



/* Supported target systems */
typedef enum {
    TGT_UNKNOWN = -1,		/* Not specified or invalid target */
    TGT_NONE,
    TGT_MODULE,
    TGT_ATARI,
    TGT_VIC20,
    TGT_C16,
    TGT_C64,
    TGT_C128,
    TGT_ACE,
    TGT_PLUS4,
    TGT_CBM510,
    TGT_CBM610,
    TGT_PET,
    TGT_BBC,
    TGT_APPLE2,
    TGT_APPLE2ENH,
    TGT_GEOS,
    TGT_LUNIX,
    TGT_ATMOS,
    TGT_NES,
    TGT_SUPERVISION,
    TGT_LYNX,
    TGT_COUNT			/* Number of target systems */
} target_t;

/* Target system */
extern target_t		Target;

/* Table with target names */
extern const char* TargetNames[TGT_COUNT];

/* Table with default CPUs per target */
extern const cpu_t DefaultCPU[TGT_COUNT];



/*****************************************************************************/
/*     	       	    		     Code			    	     */
/*****************************************************************************/



target_t FindTarget (const char* Name);
/* Find a target by name and return the target id. TGT_UNKNOWN is returned if
 * the given name is no valid target.
 */



/* End of target.h */

#endif




