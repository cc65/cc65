/*****************************************************************************/
/*                                                                           */
/*                                    chip.h                                 */
/*                                                                           */
/*                        Interface for the chip plugins                     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2002      Ullrich von Bassewitz                                       */
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



#ifndef CHIP_H
#define CHIP_H



/* common.h */
#include "coll.h"

/* sim65 */
#include "chipdata.h"
#include "simdata.h"



/*****************************************************************************/
/*                                     Data                                  */
/*****************************************************************************/



#if 0
typedef struct ChipInstance ChipInstance;
struct ChipInstance {
    Chip*       C;                      /* Pointer to corresponding chip */
    unsigned    Addr;                   /* Start address of range */
    unsigned    Size;                   /* Size of range */
};
#endif



/* Chip structure */
typedef struct Chip Chip;
struct Chip {
    struct ChipLibrary* Library;        /* Pointer to library data structure */
    const ChipData*     Data;           /* Chip data as given by the library */
    Collection          Instances;      /* Pointer to chip instances */
};



/*****************************************************************************/
/*     	      	    		     Code				     */
/*****************************************************************************/



void LoadChips (void);
/* Load all chips from all libraries */

const Chip* FindChip (const char* Name);
/* Find a chip by name. Returns the Chip data structure or NULL if the chip
 * could not be found.
 */



/* End of chip.h */

#endif



