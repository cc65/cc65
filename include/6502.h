/*
 * 6502.h
 *
 * Ullrich von Bassewitz, 20.09.1998
 */



#ifndef _6502_H
#define _6502_H



/* Possible returns of getcpu() */
#define CPU_6502	0
#define CPU_65C02	1
#define CPU_65816	2

unsigned char getcpu (void);
/* Detect the CPU the program is running on */



/* Macros for CPU instructions */
#define	BRK()	__asm__ ("\tbrk")
#define CLI()	__asm__ ("\tcli")
#define SEI()	__asm__ ("\tsei")
#define JAM()	__asm__ ("\t.byte\t$02")



/* Struct that holds the registers for the sys function */
struct regs {
    unsigned char a;		/* A register value */
    unsigned char x;		/* X register value */
    unsigned char y;		/* Y register value */
    unsigned char flags;	/* Flags value */
    unsigned      pc;		/* Program counter */
};

/* Defines for the flags in the regs structure */
#define F_NEG		0x80	/* N flag */
#define F_OVF		0x40	/* V flag */
#define F_BRK		0x10	/* B flag */
#define F_DEC		0x08	/* D flag */
#define F_IEN  	   	0x04	/* I flag */
#define F_ZERO	   	0x02	/* Z flag */
#define F_CARRY	   	0x01	/* C flag */

/* Function to call any machine language subroutine. All registers in the
 * regs structure are passed into the routine and the results are passed
 * out. Some of the flags are ignored on input. The called routine must
 * end with an RTS.
 */
void __fastcall__ _sys (struct regs* r);



/* Set and reset the break vector. The given user function is called if
 * a break occurs. The values of the registers may be read from the brk_...
 * variables. The value in brk_pc will point to the address that contains
 * the brk instruction.
 * The set_brk function will install an exit handler that will reset the
 * vector if the program ends.
 */

extern unsigned char brk_a;	/* A register value */
extern unsigned char brk_x;	/* X register value */
extern unsigned char brk_y;	/* Y register value */
extern unsigned char brk_sr;	/* Status register */
extern unsigned brk_pc;		/* PC value */

typedef void (*brk_handler) (void);
/* Type of the break handler */

void __fastcall__ set_brk (brk_handler f);
/* Set the break vector to the given address, return the old address */

void reset_brk (void);
/* Reset the break vector to the original value */



/* End of 6502.h */
#endif



