/*****************************************************************************/
/*                                                                           */
/*                                   cpucore.h                               */
/*                                                                           */
/*                        CPU core for the 6502 simulator                    */
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



#ifndef CPUCORE_H
#define CPUCORE_H



/*****************************************************************************/
/*  				     Data				     */
/*****************************************************************************/



/* Registers */
extern unsigned char	AC;		/* Accumulator */
extern unsigned char	XR;		/* X register */
extern unsigned char	YR;		/* Y register */
extern unsigned char	SR;		/* Status register */
extern unsigned char	SP;		/* Stackpointer */
extern unsigned	     	PC;		/* Program counter */

/* Status register bits */
#define CF	0x01		/* Carry flag */
#define ZF	0x02		/* Zero flag */
#define IF	0x04		/* Interrupt flag */
#define DF	0x08		/* Decimal flag */
#define BF	0x10		/* Break flag */
#define OF	0x40		/* Overflow flag */
#define SF	0x80		/* Sign flag */



/*****************************************************************************/
/*  				     Code				     */
/*****************************************************************************/



void CPUInit (void);
/* Initialize the CPU */

void Reset (void);
/* Reset the CPU */

void IRQ (void);
/* Generate an IRQ */

void NMI (void);
/* Generate an NMI */

void Break (const char* Format, ...);
/* Stop running and display the given message */

void CPURun (void);
/* Run the CPU */



/* End of cpucore.h */

#endif



