/*****************************************************************************/
/*                                                                           */
/*	       			    instr.h				     */
/*                                                                           */
/*	       Instruction encoding for the ca65 macroassembler		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2003 Ullrich von Bassewitz                                       */
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



#ifndef INSTR_H
#define INSTR_H



/* common */
#include "cpu.h"



/*****************************************************************************/
/*     	       	    		     Data   				     */
/*****************************************************************************/



/* Constants for the addressing mode. If an opcode is available in zero page
 * and absolut adressing mode, both bits are set. When checking for valid
 * modes, the zeropage bit is checked first. Similar, the implicit bit is set
 * on accu adressing modes, so the 'A' for accu adressing is not needed (but
 * may be specified).
 * When assembling for the 6502 or 65C02, all addressing modes that are not
 * available on these CPUs are removed before doing any checks.
 */
#define AM_IMPLICIT    	       	0x00000003UL
#define AM_ACCU	       	       	0x00000002UL
#define AM_DIR      	       	0x00000004UL
#define AM_ABS     	       	0x00000008UL
#define AM_ABS_LONG  		0x00000010UL
#define AM_DIR_X 	  	0x00000020UL
#define AM_ABS_X            	0x00000040UL
#define AM_ABS_LONG_X		0x00000080UL
#define AM_DIR_Y               	0x00000100UL
#define AM_ABS_Y           	0x00000200UL
#define AM_DIR_IND      	0x00000400UL
#define AM_ABS_IND     		0x00000800UL
#define AM_DIR_IND_LONG     	0x00001000UL
#define AM_DIR_IND_Y     	0x00002000UL
#define AM_DIR_IND_LONG_Y    	0x00004000UL
#define AM_DIR_X_IND         	0x00008000UL
#define AM_ABS_X_IND    	0x00010000UL
#define AM_REL             	0x00020000UL
#define AM_REL_LONG            	0x00040000UL
#define AM_STACK_REL        	0x00080000UL
#define AM_STACK_REL_IND_Y	0x00100000UL
#define AM_IMM_ACCU		0x00200000UL
#define AM_IMM_INDEX		0x00400000UL
#define AM_IMM_IMPLICIT		0x00800000UL
#define AM_IMM           	(AM_IMM_ACCU | AM_IMM_INDEX | AM_IMM_IMPLICIT)
#define AM_BLOCKMOVE           	0x01000000UL

/* Bitmask for all ZP operations that have correspondent ABS ops */
#define AM_ZP	(AM_DIR | AM_DIR_X | AM_DIR_Y | AM_DIR_IND | AM_DIR_X_IND)

/* Bit numbers and count */
#define AMI_IMM_ACCU		21
#define AMI_IMM_INDEX		22
#define AMI_COUNT		25



/* Description for one instruction */
typedef struct InsDesc_ InsDesc;
struct InsDesc_ {
    char  	  	Mnemonic [4];
    unsigned long      	AddrMode;		/* Valid adressing modes */
    unsigned char	BaseCode;  	 	/* Base opcode */
    unsigned char	ExtCode;   	 	/* Number of ext code table */
    void       	       	(*Emit) (const InsDesc*);/* Handler function */
};

/* An instruction table */
typedef struct InsTable_ InsTable;
struct InsTable_ {
    unsigned		Count;		      	/* Number of intstructions */
    InsDesc    	       	Ins[1];		      	/* Varying length */
};

/* The instruction table for the currently active CPU */
extern const InsTable* InsTab;

/* Table to build the effective opcode from a base opcode and an addressing
 * mode.
 */
extern unsigned char EATab [9][AMI_COUNT];

/* Table that encodes the additional bytes for each instruction */
extern unsigned char ExtBytes [AMI_COUNT];



/*****************************************************************************/
/*     	     	    		     Code   				     */
/*****************************************************************************/



void SetCPU (cpu_t NewCPU);
/* Set a new CPU */

cpu_t GetCPU (void);
/* Return the current CPU */

int FindInstruction (const char* Ident);
/* Check if Ident is a valid mnemonic. If so, return the index in the
 * instruction table. If not, return -1.
 */

void HandleInstruction (unsigned Index);
/* Handle the mnemonic with the given index */



/* End of instr.h */

#endif




