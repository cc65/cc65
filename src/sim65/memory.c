/*****************************************************************************/
/*                                                                           */
/*		       		   memory.h				     */
/*                                                                           */
/*		    Memory subsystem for the 6502 simulator		     */
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



#include <stdio.h>
#include <string.h>
#include <errno.h>

/* common */
#include "coll.h"
#include "xmalloc.h"

/* sim65 */
#include "chip.h"
#include "cputype.h"
#include "error.h"
#include "memory.h"



/*****************************************************************************/
/*  			    	     Data				     */
/*****************************************************************************/



/* Pointer to our memory */
static const ChipInstance** MemData = 0;
unsigned MemSize                    = 0;



/*****************************************************************************/
/*   		  		     Code				     */
/*****************************************************************************/



void MemWriteByte (unsigned Addr, unsigned char Val)
/* Write a byte to a memory location */
{
    /* Get the instance of the chip at this address */
    const ChipInstance* CI = MemData[Addr];

    /* Check if the memory is mapped */
    if (CI == 0) {
        Warning ("Writing to unassigned memory at $%06X", Addr);
    } else {
        CI->C->Data->Write (CI->Data, Addr - CI->Addr, Val);
    }
}



unsigned char MemReadByte (unsigned Addr)
/* Read a byte from a memory location */
{
    /* Get the instance of the chip at this address */
    const ChipInstance* CI = MemData[Addr];

    /* Check if the memory is mapped */
    if (CI == 0) {
        Warning ("Reading from unassigned memory at $%06X", Addr);
        return 0xFF;
    } else {
        return CI->C->Data->Read (CI->Data, Addr - CI->Addr);
    }
}



unsigned MemReadWord (unsigned Addr)
/* Read a word from a memory location */
{
    unsigned W = MemReadByte (Addr++);
    return (W | (MemReadByte (Addr) << 8));
}



unsigned MemReadZPWord (unsigned char Addr)
/* Read a word from the zero page. This function differs from ReadMemW in that
 * the read will always be in the zero page, even in case of an address
 * overflow.
 */
{
    unsigned W = MemReadByte (Addr++);
    return (W | (MemReadByte (Addr) << 8));
}



void MemAssignChip (const ChipInstance* CI, unsigned Addr, unsigned Range)
/* Assign a chip instance to memory locations */
{
    /* Make sure, the addresses are in a valid range */
    PRECONDITION (Addr + Range <= MemSize);

    /* Assign the chip instance */
    while (Range--) {
        CHECK (MemData[Addr] == 0);
        MemData[Addr++] = CI;
    }
}



const struct ChipInstance* MemGetChip (unsigned Addr)
/* Get the chip that is located at the given address (may return NULL). */
{
    /* Make sure, the address is valid */
    PRECONDITION (Addr < MemSize);

    /* Return the chip instance */
    return MemData[Addr];
}



void MemInit (void)
/* Initialize the memory subsystem */
{
    unsigned I;

    /* Allocate memory depending on the CPU type */
    switch (CPU) {
        case CPU_6502:
        case CPU_65C02:
            MemSize = 0x10000;
            break;
        default:
            Internal ("Unexpected CPU type: %d", CPU);
    }
    MemData = xmalloc (MemSize * sizeof (ChipInstance*));

    /* Clear the memory */
    for (I = 0; I < MemSize; ++I) {
        MemData[I] = 0;
    }
}



