/*****************************************************************************/
/*                                                                           */
/*                                  6502.h                                   */
/*                                                                           */
/*                        6502 specific declarations                         */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2012, Ullrich von Bassewitz                                      */
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



/* We need size_t */
#ifndef _HAVE_size_t
#define _HAVE_size_t
typedef unsigned size_t;
#endif



/* Possible returns of getcpu() */
#define CPU_6502        0
#define CPU_65C02       1
#define CPU_65816       2
#define CPU_4510        3
#define CPU_65SC02      4
#define CPU_65CE02      5
#define CPU_HUC6280     6
#define CPU_2A0x        7
#define CPU_45GS02      8

unsigned char getcpu (void);
/* Detect the CPU the program is running on */



/* Macros for CPU instructions */
#define BRK()   __asm__ ("brk")
#define CLI()   __asm__ ("cli")
#define SEI()   __asm__ ("sei")



/* Struct that holds the registers for the sys function */
struct regs {
    unsigned char a;            /* A register value */
    unsigned char x;            /* X register value */
    unsigned char y;            /* Y register value */
    unsigned char flags;        /* Flags value */
    unsigned      pc;           /* Program counter */
};

/* Defines for the flags in the regs structure */
#define F6502_N         0x80    /* N flag */
#define F6502_V         0x40    /* V flag */
#define F6502_B         0x10    /* B flag */
#define F6502_D         0x08    /* D flag */
#define F6502_I         0x04    /* I flag */
#define F6502_Z         0x02    /* Z flag */
#define F6502_C         0x01    /* C flag */

/* Function to call any machine language subroutine. All registers in the
** regs structure are passed into the routine and the results are passed
** out. The B flag is ignored on input. The called routine must end with
** an RTS.
*/
void __fastcall__ _sys (struct regs* r);



/* Set and reset the break vector. The given user function is called if
** a break occurs. The values of the registers may be read from the brk_...
** variables. The value in brk_pc will point to the address that contains
** the brk instruction.
** The set_brk function will install an exit handler that will reset the
** vector if the program ends.
*/

extern unsigned char brk_a;     /* A register value */
extern unsigned char brk_x;     /* X register value */
extern unsigned char brk_y;     /* Y register value */
extern unsigned char brk_sr;    /* Status register */
extern unsigned brk_pc;         /* PC value */

typedef void (*brk_handler) (void);
/* Type of the break handler */

void __fastcall__ set_brk (brk_handler f);
/* Set the break vector to the given address */

void reset_brk (void);
/* Reset the break vector to the original value */



/* Possible returns for irq_handler() */
#define IRQ_NOT_HANDLED 0
#define IRQ_HANDLED     1

typedef unsigned char (*irq_handler) (void);
/* Type of the C level interrupt request handler */

void __fastcall__ set_irq (irq_handler f, void *stack_addr, size_t stack_size);
/* Set the C level interrupt request vector to the given address */

void reset_irq (void);
/* Reset the C level interrupt request vector */



/* End of 6502.h */
#endif
