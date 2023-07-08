/*****************************************************************************/
/*                                                                           */
/*                                  6502.h                                   */
/*                                                                           */
/*                           CPU core for the 6502                           */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2003-2012, Ullrich von Bassewitz                                      */
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



#ifndef _6502_H
#define _6502_H



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Supported CPUs */
typedef enum CPUType {
    CPU_6502,
    CPU_65C02
} CPUType;

/* Current CPU */
extern CPUType CPU;

/* 6502 CPU registers */
typedef struct CPURegs CPURegs;
struct CPURegs {
    unsigned    AC;             /* Accumulator */
    unsigned    XR;             /* X register */
    unsigned    YR;             /* Y register */
    unsigned    ZR;             /* Z register */
    unsigned    SR;             /* Status register */
    unsigned    SP;             /* Stackpointer */
    unsigned    PC;             /* Program counter */
};

/* Status register bits */
#define CF      0x01            /* Carry flag */
#define ZF      0x02            /* Zero flag */
#define IF      0x04            /* Interrupt flag */
#define DF      0x08            /* Decimal flag */
#define BF      0x10            /* Break flag */
#define OF      0x40            /* Overflow flag */
#define SF      0x80            /* Sign flag */



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void Reset (void);
/* Generate a CPU RESET */

void IRQRequest (void);
/* Generate an IRQ */

void NMIRequest (void);
/* Generate an NMI */

unsigned ExecuteInsn (void);
/* Execute one CPU instruction. Return the number of clock cycles for the
** executed instruction.
*/


/* End of 6502.h */

#endif
