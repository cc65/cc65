/*****************************************************************************/
/*                                                                           */
/*                                addrspace.h                                */
/*                                                                           */
/*                 CPU address space for the 6502 simulator                  */
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



#ifndef ADDRSPACE_H
#define ADDRSPACE_H



/*****************************************************************************/
/*     		    		     Data				     */
/*****************************************************************************/



/* Forwards */
struct CPUData;
struct ChipInstance;

/* Forwards */
typedef struct AddressSpace AddressSpace;
struct AddressSpace {

    struct CPU*                 CPU;      /* Backpointer to CPU */
    unsigned                    Size;     /* Address space size */
    struct ChipInstance*        Data[1];  /* Pointer to chips, dynamically! */

};



/*****************************************************************************/
/*     	     	    	   	     Code				     */
/*****************************************************************************/



AddressSpace* NewAddressSpace (unsigned Size);
/* Allocate a new address space and return it */

void ASWrite (AddressSpace* AS, unsigned Addr, unsigned char Val);
/* Write a byte to a given location */

unsigned char ASRead (AddressSpace* AS, unsigned Addr);
/* Read a byte from a location */

void ASWriteCtrl (AddressSpace* AS, unsigned Addr, unsigned char Val);
/* Write a byte to a given location */

unsigned char ASReadCtrl (AddressSpace* AS, unsigned Addr);
/* Read a byte from a location */

void ASAssignChip (AddressSpace* AS, struct ChipInstance* CI,
                   unsigned Addr, unsigned Range);
/* Assign a chip instance to memory locations */

struct ChipInstance* ASGetChip (const AddressSpace* AS, unsigned Addr);
/* Get the chip that is located at the given address (may return NULL). */



/* End of addrspace.h */

#endif




