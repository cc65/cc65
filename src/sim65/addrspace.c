/*****************************************************************************/
/*                                                                           */
/*                                addrspace.c                                */
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



/* common */
#include "check.h"
#include "xmalloc.h"

/* sim65 */
#include "chip.h"
#include "cpucore.h"
#include "error.h"
#include "addrspace.h"



/*****************************************************************************/
/*     	     	    	   	     Code				     */
/*****************************************************************************/



AddressSpace* NewAddressSpace (unsigned Size)
/* Allocate a new address space and return it */
{
    unsigned I;

    /* Allocate memory */
    AddressSpace* AS = xmalloc (sizeof (AddressSpace) +
                                (Size - 1) * sizeof (ChipInstance*));

    /* Initialize the struct */
    AS->CPU  = 0;
    AS->Size = Size;
    for (I = 0; I < Size; ++I) {
        AS->Data[I] = 0;
    }

    /* Return the new struct */
    return AS;
}



void ASWrite (AddressSpace* AS, unsigned Addr, unsigned char Val)
/* Write a byte to a given location */
{
    const ChipInstance* CI;

    /* Make sure, the addresses are in a valid range */
    PRECONDITION (Addr < AS->Size);

    /* Get the instance of the chip at this address */
    CI = AS->Data[Addr];

    /* Check if the memory is mapped */
    if (CI == 0) {
        Break ("Writing to unassigned memory at $%06X", Addr);
    } else {
        CI->C->Data->Write (CI->Data, Addr - CI->Addr, Val);
    }
}



unsigned char ASRead (AddressSpace* AS, unsigned Addr)
/* Read a byte from a location */
{
    const ChipInstance* CI;

    /* Make sure, the addresses are in a valid range */
    PRECONDITION (Addr < AS->Size);

    /* Get the instance of the chip at this address */
    CI = AS->Data[Addr];

    /* Check if the memory is mapped */
    if (CI == 0) {
        Break ("Reading from unassigned memory at $%06X", Addr);
        return 0xFF;
    } else {
        return CI->C->Data->Read (CI->Data, Addr - CI->Addr);
    }
}



void ASWriteCtrl (AddressSpace* AS, unsigned Addr, unsigned char Val)
/* Write a byte to a given location */
{
    const ChipInstance* CI;

    /* Make sure, the addresses are in a valid range */
    PRECONDITION (Addr < AS->Size);

    /* Get the instance of the chip at this address */
    CI = AS->Data[Addr];

    /* Check if the memory is mapped */
    if (CI == 0) {
        Break ("Writing to unassigned memory at $%06X", Addr);
    } else {
        CI->C->Data->WriteCtrl (CI->Data, Addr - CI->Addr, Val);
    }
}



unsigned char ASReadCtrl (AddressSpace* AS, unsigned Addr)
/* Read a byte from a location */
{
    const ChipInstance* CI;

    /* Make sure, the addresses are in a valid range */
    PRECONDITION (Addr < AS->Size);

    /* Get the instance of the chip at this address */
    CI = AS->Data[Addr];

    /* Check if the memory is mapped */
    if (CI == 0) {
        Break ("Reading from unassigned memory at $%06X", Addr);
        return 0xFF;
    } else {
        return CI->C->Data->ReadCtrl (CI->Data, Addr - CI->Addr);
    }
}


void ASAssignChip (AddressSpace* AS, ChipInstance* CI,
                   unsigned Addr, unsigned Range)
/* Assign a chip instance to memory locations */
{
    /* Make sure, the addresses are in a valid range */
    PRECONDITION (Addr + Range <= AS->Size);

    /* Assign the chip instance */
    while (Range--) {
        CHECK (AS->Data[Addr] == 0);
        AS->Data[Addr++] = CI;
    }

    /* Set the backpointer to us */
    CI->AS = AS;
}


ChipInstance* ASGetChip (const AddressSpace* AS, unsigned Addr)
/* Get the chip that is located at the given address (may return NULL). */
{
    /* Make sure, the addresses are in a valid range */
    PRECONDITION (Addr < AS->Size);

    /* Return the chip instance */
    return AS->Data[Addr];
}



