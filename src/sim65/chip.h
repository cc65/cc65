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



/*****************************************************************************/
/*                                     Data                                  */
/*****************************************************************************/



/* Forward */
struct SimData;

/* Chip structure */
typedef struct Chip Chip;
struct Chip {
    char*       Name;                   /* Name - must be unique */
    char*       LibName;                /* Name of the associated library */
    void*       Handle;                 /* Library handle or pointer to it */

    /* -- Exported functions -- */
    unsigned        (*InitChip) (const struct SimData* Data);
    const char*     (*GetName) (void);
    unsigned        (*GetVersion) (void);

    void            (*WriteCtrl) (unsigned Addr, unsigned char Val);
    void            (*Write) (unsigned Addr, unsigned char Val);

    unsigned char   (*ReadCtrl) (unsigned Addr);
    unsigned char   (*Read) (unsigned Addr);
};



/*****************************************************************************/
/*     	      	    		     Code				     */
/*****************************************************************************/



void LoadChip (const char* LibName);
/* Load a chip. This includes loading the shared libary, allocating and
 * initializing the data structure.
 */

void InitChips (void);
/* Initialize the chips. Must be called *after* all chips are loaded */

const Chip* GetChip (const char* Name);
/* Find a chip by name. Returns the Chip data structure or NULL if the chip
 * could not be found.
 */



/* End of chip.h */

#endif



