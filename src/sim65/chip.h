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



/* Forwards */
struct CfgData;
struct ChipLibrary;
typedef struct Chip Chip;
typedef struct ChipInstance ChipInstance;

/* One instance of a chip */
struct ChipInstance {
    Chip*               C;              /* Pointer to corresponding chip */
    unsigned            Addr;           /* Start address of range */
    unsigned            Size;           /* Size of range */
    void*               Data;           /* Chip instance data */
};

/* Chip structure */
struct Chip {
    struct ChipLibrary* Library;        /* Pointer to library data structure */
    const ChipData*     Data;           /* Chip data as given by the library */
    Collection          Instances;      /* Pointer to chip instances */
};



/*****************************************************************************/
/*     	      	    		     Code		  		     */
/*****************************************************************************/



Chip* NewChip (struct ChipLibrary* Library, const ChipData* Data);
/* Allocate a new chip structure, initialize and return it */

ChipInstance* NewChipInstance (const char* ChipName, unsigned Addr,
                               unsigned Size, const Collection* Attributes);
/* Allocate a new chip instance for the chip. */

ChipInstance* MirrorChipInstance (const ChipInstance* Orig, unsigned Addr);
/* Generate a chip instance mirror and return it. */

void SortChips (void);
/* Sort all chips by name. Called after loading */



/* End of chip.h */

#endif




