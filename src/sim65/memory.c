/*****************************************************************************/
/*                                                                           */
/*		       		   memory.h				     */
/*                                                                           */
/*		    Memory subsystem for the 6502 simulator		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2002      Ullrich von Bassewitz                                       */
/*               Wacholderweg 14                                             */
/*               D-70597 Stuttgart                                           */
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
#include "coll.h"

/* sim65 */            
#include "error.h"
#include "memory.h"



/*****************************************************************************/
/*                                   Forwards                                */
/*****************************************************************************/



static void MemWrite (unsigned Addr, unsigned char Val);
/* Write one byte to the memory cell */

static unsigned char MemRead (unsigned Attr);
/* Read one memory cell */



/*****************************************************************************/
/*  			    	     Data				     */
/*****************************************************************************/



/* RAM attributes */
#define RA_READFUNC_MASK        0x000F  /* Up to 16 read functions */
#define RA_WRITEFUNC_MASK       0x00F0  /* Up to 16 write functions */
#define RA_INITIALIZED          0x0100  /* Memory cell is initialized */
#define RA_WPROT                0x0200  /* Memory cell is write protected */

/* Defines reader and writer functions */
#define RA_READFUNC_SHIFT       0
#define RA_WRITEFUNC_SHIFT      4
#define RA_READFUNC_MAX         16
#define RA_WRITEFUNC_MAX        16

/* Read/write function declarations */
typedef unsigned char (*ReadFunc) (unsigned Addr);
typedef void (*WriteFunc) (unsigned Addr, unsigned char Val);
static Collection ReadFuncs  = STATIC_COLLECTION_INITIALIZER;
static Collection WriteFuncs = STATIC_COLLECTION_INITIALIZER;

/* Memory attributes and the memory */
static unsigned short MemAttr[0x10000];
static unsigned char Mem[0x10000];



/*****************************************************************************/
/*                              Internal functions                           */
/*****************************************************************************/



static void MemWrite (unsigned Addr, unsigned char Val)
/* Write one byte to the memory cell */
{
    if (MemAttr[Addr] & RA_WPROT) {
        Warning ("Writing to write protected memory at $%04X", Addr);
    }
    Mem[Addr] = Val;
    MemAttr[Addr] |= RA_INITIALIZED;
}



static unsigned char MemRead (unsigned Addr)
/* Read one memory cell */
{
    if ((MemAttr[Addr] & RA_INITIALIZED) == 0) {
        /* We're reading a memory cell that was never written */
        Warning ("Reading from uninitialized memory at $%04X", Addr);
    }
    return Mem[Addr];
}



/*****************************************************************************/
/*   		  		     Code				     */
/*****************************************************************************/



void MemWriteByte (unsigned Addr, unsigned char Val)
/* Write a byte to a memory location */
{
    /* Get the writer function */
    unsigned  WI = (MemAttr[Addr] & RA_WRITEFUNC_MASK) >> RA_WRITEFUNC_SHIFT;
    WriteFunc WF = CollAt (&WriteFuncs, WI);

    /* Call the writer function */
    WF (Addr, Val);
}



unsigned char MemReadByte (unsigned Addr)
/* Read a byte from a memory location */
{
    /* Get the reader function */
    unsigned  RI = (MemAttr[Addr] & RA_READFUNC_MASK) >> RA_READFUNC_SHIFT;
    ReadFunc RF = CollAt (&WriteFuncs, RI);

    /* Call the reader function */
    return RF (Addr);
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



void MemInit (void)
/* Initialize the memory subsystem */
{
    unsigned I;

    /* Clear the memory and it's attributes. Writing zeroes to the
     * attribute array will cause any special flags to be reset and
     * the default read and write functions to be used.
     */
    for (I = 0; I < sizeof (Mem) / sizeof (Mem[0]); ++I) {
        Mem[I] = 0;
    }
    for (I = 0; I < sizeof (MemAttr) / sizeof (MemAttr[0]); ++I) {
        MemAttr[I] = 0;
    }

    /* Add the default reader and writer functions to the collection */
    CollAppend (&ReadFuncs, MemRead);
    CollAppend (&WriteFuncs, MemWrite);
}
