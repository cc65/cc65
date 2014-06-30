/*****************************************************************************/
/*                                                                           */
/*                                  instr.h                                  */
/*                                                                           */
/*             Instruction encoding for the ca65 macroassembler              */
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



#ifndef INSTR_H
#define INSTR_H



/* common */
#include "cpu.h"
#include "strbuf.h"



/*****************************************************************************/
/*                       Data for 6502 and successors                        */
/*****************************************************************************/



/* Constants for the addressing mode. If an opcode is available in zero page
** and absolut adressing mode, both bits are set. When checking for valid
** modes, the zeropage bit is checked first. Similar, the implicit bit is set
** on accu adressing modes, so the 'A' for accu adressing is not needed (but
** may be specified).
** When assembling for the 6502 or 65C02, all addressing modes that are not
** available on these CPUs are removed before doing any checks.
*/
#define AM65_IMPLICIT           0x00000003UL
#define AM65_ACCU               0x00000002UL
#define AM65_DIR                0x00000004UL
#define AM65_ABS                0x00000008UL
#define AM65_ABS_LONG           0x00000010UL
#define AM65_DIR_X              0x00000020UL
#define AM65_ABS_X              0x00000040UL
#define AM65_ABS_LONG_X         0x00000080UL
#define AM65_DIR_Y              0x00000100UL
#define AM65_ABS_Y              0x00000200UL
#define AM65_DIR_IND            0x00000400UL
#define AM65_ABS_IND            0x00000800UL
#define AM65_DIR_IND_LONG       0x00001000UL
#define AM65_DIR_IND_Y          0x00002000UL
#define AM65_DIR_IND_LONG_Y     0x00004000UL
#define AM65_DIR_X_IND          0x00008000UL
#define AM65_ABS_X_IND          0x00010000UL
#define AM65_REL                0x00020000UL
#define AM65_REL_LONG           0x00040000UL
#define AM65_STACK_REL          0x00080000UL
#define AM65_STACK_REL_IND_Y    0x00100000UL
#define AM65_IMM_ACCU           0x00200000UL
#define AM65_IMM_INDEX          0x00400000UL
#define AM65_IMM_IMPLICIT       0x00800000UL
#define AM65_BLOCKMOVE          0x01000000UL
#define AM65_BLOCKXFER          0x02000000UL
#define AM65_ABS_IND_LONG       0x04000000UL

/* Bitmask for all ZP operations that have correspondent ABS ops */
#define AM65_SET_ZP     (AM65_DIR | AM65_DIR_X | AM65_DIR_Y | AM65_DIR_IND | AM65_DIR_X_IND)

/* Bitmask for all ABS operations that have correspondent FAR ops */
#define AM65_SET_ABS    (AM65_ABS | AM65_ABS_X)

/* Bitmask for all ZP operations */
#define AM65_ALL_ZP     (AM65_DIR | AM65_DIR_X | AM65_DIR_Y | AM65_DIR_IND | AM65_DIR_X_IND)

/* Bitmask for all ABS operations */
#define AM65_ALL_ABS    (AM65_ABS | AM65_ABS_X | AM65_ABS_Y | AM65_ABS_IND | AM65_ABS_X_IND)

/* Bitmask for all FAR operations */
#define AM65_ALL_FAR    (AM65_ABS_LONG | AM65_ABS_LONG_X)
                        
/* Bitmask for all immediate operations */
#define AM65_ALL_IMM    (AM65_IMM_ACCU | AM65_IMM_INDEX | AM65_IMM_IMPLICIT)

/* Bit numbers and count */
#define AM65I_IMM_ACCU          21
#define AM65I_IMM_INDEX         22
#define AM65I_IMM_IMPLICIT      23
#define AM65I_COUNT             27



/* Description for one instruction */
typedef struct InsDesc InsDesc;
struct InsDesc {
    char                Mnemonic[5];
    unsigned long       AddrMode;               /* Valid adressing modes */
    unsigned char       BaseCode;               /* Base opcode */
    unsigned char       ExtCode;                /* Number of ext code table */
    void                (*Emit) (const InsDesc*);/* Handler function */
};

/* An instruction table */
typedef struct InsTable InsTable;
struct InsTable {
    unsigned            Count;                  /* Number of intstructions */
    InsDesc             Ins[1];                 /* Varying length */
};

/* The instruction table for the currently active CPU */
extern const InsTable* InsTab;

/* Table that encodes the additional bytes for each instruction */
extern unsigned char ExtBytes[AM65I_COUNT];



/*****************************************************************************/
/*                      Data for the SWEET16 pseudo CPU                      */
/*****************************************************************************/



/* SWEET16 addressing modes */
#define AMSW16_IMP      0x0001          /* Implicit */
#define AMSW16_BRA      0x0002          /* A branch */
#define AMSW16_IMM      0x0004          /* Immediate */
#define AMSW16_IND      0x0008          /* Indirect */
#define AMSW16_REG      0x0010          /* Register */

#define AMSW16I_COUNT   5               /* Number of addressing modes */



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void SetCPU (cpu_t NewCPU);
/* Set a new CPU */

cpu_t GetCPU (void);
/* Return the current CPU */

int FindInstruction (const StrBuf* Ident);
/* Check if Ident is a valid mnemonic. If so, return the index in the
** instruction table. If not, return -1.
*/

void HandleInstruction (unsigned Index);
/* Handle the mnemonic with the given index */



/* End of instr.h */

#endif
