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



#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#if defined(_WIN32)
#  define O_INITIAL O_BINARY
#else
#  define O_INITIAL 0
#endif
#if defined(_MSC_VER)
/* Microsoft compiler */
#  include <io.h>
#else
/* Anyone else */
#  include <unistd.h>
#endif

/* common */
#include "cmdline.h"
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

static unsigned ArgStart;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static unsigned GetAX (CPURegs* Regs)
{
    return Regs->AC + (Regs->XR << 8);
}



static void SetAX (CPURegs* Regs, unsigned Val)
{
    Regs->AC = Val & 0xFF;
    Val >>= 8;
    Regs->XR = Val;
}



static void MemWriteWord (unsigned Addr, unsigned Val)
{
    MemWriteByte (Addr, Val);
    Val >>= 8;
    MemWriteByte (Addr + 1, Val);
}



static unsigned char Pop (CPURegs* Regs)
{
    return MemReadByte (0x0100 + ++Regs->SP);
}



static unsigned PopParam (unsigned char Incr)
{
    unsigned SP = MemReadZPWord (0x00);
    unsigned Val = MemReadWord (SP);
    MemWriteWord (0x0000, SP + Incr);
    return Val;
}



static void PVArgs (CPURegs* Regs)
{
    unsigned ArgC = ArgCount - ArgStart;
    unsigned ArgV = GetAX (Regs);
    unsigned SP   = MemReadZPWord (0x00);
    unsigned Args = SP - (ArgC + 1) * 2;

    Print (stderr, 2, "PVArgs ($%04X)\n", ArgV);

    MemWriteWord (ArgV, Args);

    SP = Args;
    while (ArgStart < ArgCount) {
        unsigned I = 0;
        const char* Arg = ArgVec[ArgStart++];
        SP -= strlen (Arg) + 1;
        do {
            MemWriteByte (SP + I, Arg[I]);
        }
        while (Arg[I++]);

        MemWriteWord (Args, SP);
        Args += 2;
    }
    MemWriteWord (Args, 0x0000);

    MemWriteWord (0x0000, SP);
    SetAX (Regs, ArgC);
}



static void PVExit (CPURegs* Regs)
{
    Print (stderr, 1, "PVExit ($%02X)\n", Regs->AC);

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

    Print (stderr, 2, "PVOpen (\"%s\", $%04X)\n", Path, Flags);

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

    SetAX (Regs, RetVal);
}



static void PVClose (CPURegs* Regs)
{
    unsigned RetVal;

    unsigned FD = GetAX (Regs);

    Print (stderr, 2, "PVClose ($%04X)\n", FD);

    RetVal = close (FD);

    SetAX (Regs, RetVal);
}



static void PVRead (CPURegs* Regs)
{
    unsigned char* Data;
    unsigned RetVal, I = 0;

    unsigned Count = GetAX (Regs);
    unsigned Buf   = PopParam (2);
    unsigned FD    = PopParam (2);

    Print (stderr, 2, "PVRead ($%04X, $%04X, $%04X)\n", FD, Buf, Count);

    Data = xmalloc (Count);

    RetVal = read (FD, Data, Count);

    if (RetVal != (unsigned) -1) {
        while (I < RetVal) {
            MemWriteByte (Buf++, Data[I++]);
        }
    }
    xfree (Data);

    SetAX (Regs, RetVal);
}



static void PVWrite (CPURegs* Regs)
{
    unsigned char* Data;
    unsigned RetVal, I = 0;

    unsigned Count = GetAX (Regs);
    unsigned Buf   = PopParam (2);
    unsigned FD    = PopParam (2);

    Print (stderr, 2, "PVWrite ($%04X, $%04X, $%04X)\n", FD, Buf, Count);

    Data = xmalloc (Count);
    while (I < Count) {
        Data[I++] = MemReadByte (Buf++);
    }

    RetVal = write (FD, Data, Count);

    xfree (Data);

    SetAX (Regs, RetVal);
}



static const PVFunc Hooks[] = {
    PVArgs,
    PVExit,
    PVOpen,
    PVClose,
    PVRead,
    PVWrite,
};



void ParaVirtInit (unsigned aArgStart)
/* Initialize the paravirtualization subsystem */
{
    ArgStart = aArgStart;
};



void ParaVirtHooks (CPURegs* Regs)
/* Potentially execute paravirtualization hooks */
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
