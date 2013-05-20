/*****************************************************************************/
/*                                                                           */
/*                                paravirt.c                                 */
/*                                                                           */
/*                Paravirtualization for the sim65 6502 simulator            */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2013-2013 Ullrich von Bassewitz                                       */
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



#include <stdlib.h>
#include <fcntl.h>
#if defined(_MSC_VER)
/* Microsoft compiler */
#  include <io.h>
#  pragma warning(disable : 4996)
#  define O_INITIAL O_BINARY
#else
/* Anyone else */
#  include <unistd.h>
#  define O_INITIAL 0
#endif

/* common */
#include "print.h"
#include "xmalloc.h"

/* sim65 */
#include "6502.h"
#include "memory.h"
#include "paravirt.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



typedef void (*PVFunc) (CPURegs* Regs);



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static unsigned char Pop (CPURegs* Regs)
{
    return MemReadByte (0x0100 + ++Regs->SP);
}



static unsigned PopParam (unsigned char Incr)
{
    unsigned SP = MemReadZPWord (0x00);
    unsigned Val = MemReadWord (SP);
    SP += Incr;
    MemWriteByte (0x00, SP);
    SP >>= 8;
    MemWriteByte (0x01, SP);
    return Val;
}



static void PVExit (CPURegs* Regs)
{
    Print (stdout, 1, "PVExit ($%02X)\n", Regs->AC);

    exit (Regs->AC);
}



static void PVOpen (CPURegs* Regs)
{
    char Path[1024];
    int OFlag = O_INITIAL;
    unsigned RetVal, I = 0;

    unsigned Mode  = PopParam (Regs->YR - 4);
    unsigned Flags = PopParam (2);
    unsigned Name  = PopParam (2);

    do {
        Path[I] = MemReadByte (Name++);
    }
    while (Path[I++]);

    Print (stdout, 2, "PVOpen (\"%s\", $%04X)\n", Path, Flags);

    switch (Flags & 0x03) {
        case 0x01:
            OFlag |= O_RDONLY;
            break;
        case 0x02:
            OFlag |= O_WRONLY;
            break;
        case 0x03:
            OFlag |= O_RDWR;
            break;
    }
    if (Flags & 0x10) {
        OFlag |= O_CREAT;
    }
    if (Flags & 0x20) {
        OFlag |= O_TRUNC;
    }
    if (Flags & 0x40) {
        OFlag |= O_APPEND;
    }
    if (Flags & 0x80) {
        OFlag |= O_EXCL;
    }

    /* Avoid gcc warning */
    (void) Mode;

    RetVal = open (Path, OFlag);

    Regs->AC = RetVal & 0xFF;
    RetVal >>= 8;
    Regs->XR = RetVal & 0xFF;
}



static void PVClose (CPURegs* Regs)
{
    unsigned RetVal;

    unsigned FD = Regs->AC + (Regs->XR << 8);

    Print (stdout, 2, "PVClose ($%04X)\n", FD);

    RetVal = close (FD);

    Regs->AC = RetVal & 0xFF;
    RetVal >>= 8;
    Regs->XR = RetVal & 0xFF;
}



static void PVRead (CPURegs* Regs)
{
    unsigned char* Data;
    unsigned RetVal, I = 0;

    unsigned Count = Regs->AC + (Regs->XR << 8);
    unsigned Buf   = PopParam (2);
    unsigned FD    = PopParam (2);

    Print (stdout, 2, "PVRead ($%04X, $%04X, $%04X)\n", FD, Buf, Count);

    Data = xmalloc (Count);

    RetVal = read (FD, Data, Count);

    if (RetVal != (unsigned) -1) {
        while (I < RetVal) {
            MemWriteByte (Buf++, Data[I++]);
        }
    }
    xfree (Data);

    Regs->AC = RetVal & 0xFF;
    RetVal >>= 8;
    Regs->XR = RetVal & 0xFF;
}



static void PVWrite (CPURegs* Regs)
{
    unsigned char* Data;
    unsigned RetVal, I = 0;

    unsigned Count = Regs->AC + (Regs->XR << 8);
    unsigned Buf   = PopParam (2);
    unsigned FD    = PopParam (2);

    Print (stdout, 2, "PVWrite ($%04X, $%04X, $%04X)\n", FD, Buf, Count);

    Data = xmalloc (Count);
    while (I < Count) {
        Data[I++] = MemReadByte (Buf++);
    }

    RetVal = write (FD, Data, Count);

    xfree (Data);

    Regs->AC = RetVal & 0xFF;
    RetVal >>= 8;
    Regs->XR = RetVal & 0xFF;
}



static const PVFunc Hooks[] = {
    PVExit,
    PVOpen,
    PVClose,
    PVRead,
    PVWrite,
};



void ParaVirtualization (CPURegs* Regs)
/* Potentially execute paravirtualization hook */
{
    /* Check for paravirtualization address range */
    if (Regs->PC <  0xFFF0 ||
        Regs->PC >= 0xFFF0 + sizeof (Hooks) / sizeof (Hooks[0])) {
        return;
    }

    /* Call paravirtualization hook */
    Hooks[Regs->PC - 0xFFF0] (Regs);

    /* Simulate RTS */
    Regs->PC = Pop(Regs) + (Pop(Regs) << 8) + 1;
}
