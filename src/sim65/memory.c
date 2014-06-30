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



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* THE memory */
static unsigned char Mem[0x10000];



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void MemWriteByte (unsigned Addr, unsigned char Val)
/* Write a byte to a memory location */
{
    Mem[Addr] = Val;
}



unsigned char MemReadByte (unsigned Addr)
/* Read a byte from a memory location */
{
    return Mem[Addr];
}



unsigned MemReadWord (unsigned Addr)
/* Read a word from a memory location */
{
    unsigned W = MemReadByte (Addr++);
    return (W | (MemReadByte (Addr) << 8));
}



unsigned MemReadZPWord (unsigned char Addr)
/* Read a word from the zero page. This function differs from ReadMemW in that
** the read will always be in the zero page, even in case of an address
** overflow.
*/
{
    unsigned W = MemReadByte (Addr++);
    return (W | (MemReadByte (Addr) << 8));
}



void MemInit (void)
/* Initialize the memory subsystem */
{
    /* Fill momory with illegal opcode */
    memset (Mem, 0xFF, sizeof (Mem));

    /* Set RESET vector to 0x0200 */
    Mem[0xFFFC] = 0x00;
    Mem[0xFFFD] = 0x02;
}
