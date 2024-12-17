/*****************************************************************************/
/*                                                                           */
/*                                 memory.h                                  */
/*                                                                           */
/*                  Memory subsystem for the 6502 simulator                  */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2002-2012, Ullrich von Bassewitz                                      */
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

#include "memory.h"
#include "peripherals.h"


/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* The memory */
uint8_t Mem[0x10000];



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void MemWriteByte (uint16_t Addr, uint8_t Val)
/* Write a byte to a memory location */
{
    if ((PERIPHERALS_APERTURE_BASE_ADDRESS <= Addr) && (Addr <= PERIPHERALS_APERTURE_LAST_ADDRESS))
    {
        /* Defer the the memory-mapped peripherals handler for this write. */
        PeripheralWriteByte (Addr - PERIPHERALS_APERTURE_BASE_ADDRESS, Val);
    } else {
        /* Write to the Mem array. */
        Mem[Addr] = Val;
    }
}



void MemWriteWord (uint16_t Addr, uint16_t Val)
/* Write a word to a memory location */
{
    MemWriteByte (Addr, Val & 0xFF);
    MemWriteByte (Addr + 1, Val >> 8);
}



uint8_t MemReadByte (uint16_t Addr)
/* Read a byte from a memory location */
{
    if ((PERIPHERALS_APERTURE_BASE_ADDRESS <= Addr) && (Addr <= PERIPHERALS_APERTURE_LAST_ADDRESS))
    {
        /* Defer the the memory-mapped peripherals handler for this read. */
        return PeripheralReadByte (Addr - PERIPHERALS_APERTURE_BASE_ADDRESS);
    } else {
        /* Read from the Mem array. */
        return Mem[Addr];
    }
}



uint16_t MemReadWord (uint16_t Addr)
/* Read a word from a memory location */
{
    uint8_t W = MemReadByte (Addr++);
    return (W | (MemReadByte (Addr) << 8));
}



uint16_t MemReadZPWord (uint8_t Addr)
/* Read a word from the zero page. This function differs from MemReadWord in that
** the read will always be in the zero page, even in case of an address
** overflow.
*/
{
    uint8_t W = MemReadByte (Addr++);
    return (W | (MemReadByte (Addr) << 8));
}



void MemInit (void)
/* Initialize the memory subsystem */
{
    /* Fill memory with illegal opcode */
    memset (Mem, 0xFF, sizeof (Mem));
}
