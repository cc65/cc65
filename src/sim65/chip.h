/*****************************************************************************/
/*                                                                           */
/*                                    chip.h                                 */
/*                                                                           */
/*                        Interface for the chip plugins                     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2002-2003 Ullrich von Bassewitz                                       */
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
struct AddressSpace;
struct CfgData;
typedef struct ChipInstance ChipInstance;
typedef struct Chip Chip;
typedef struct ChipLibrary ChipLibrary;

/* One instance of a chip */
struct ChipInstance {
    Chip*                   C;          /* Pointer to corresponding chip */
    struct AddressSpace*    AS;         /* Pointer to address space */
    unsigned                Addr;       /* Start address of range */
    unsigned                Size;       /* Size of range */
    void*                   Data;       /* Chip instance data */
};

/* Chip structure */
struct Chip {
    struct ChipLibrary*     Lib;        /* Pointer to library data structure */
    const ChipData*         Data;       /* Chip data as given by the library */
    Collection              Instances;  /* Pointer to chip instances */
};

/* ChipLibrary structure */
struct ChipLibrary {
    char*                   LibName;    /* Name of the library as given */
    char*                   PathName;   /* Name of library including path */
    void*                   Handle;     /* Pointer to libary handle */
    Collection              Chips;      /* Chips in this library */
};



/*****************************************************************************/
/*     	      	    		     Code		  		     */
/*****************************************************************************/



ChipInstance* NewChipInstance (const char* ChipName, unsigned Addr,
                               unsigned Size, Collection* Attributes);
/* Allocate a new chip instance for the chip. */

ChipInstance* MirrorChipInstance (const ChipInstance* Orig, unsigned Addr);
/* Generate a chip instance mirror and return it. */

void SortChips (void);
/* Sort all chips by name. Called after loading */

void LoadChipLibrary (const char* LibName);
/* Load a chip library. This includes loading the shared libary, allocating
 * and initializing the data structure, and loading all chip data from the
 * library.
 */



/* End of chip.h */

#endif



