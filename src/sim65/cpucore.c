/*****************************************************************************/
/*                                                                           */
/*                                   cpucore.c                               */
/*                                                                           */
/*                        CPU core for the 6502 simulator                    */
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



#include <stdlib.h>

/* common */
#include "abend.h"
#include "print.h"

/* sim65 */
#include "cputype.h"
#include "global.h"
#include "memory.h"
#include "cpucore.h"



/*****************************************************************************/
/*  				     Data				     */
/*****************************************************************************/



/* Registers */
unsigned char	AC;		/* Accumulator */
unsigned char	XR;		/* X register */
unsigned char	YR;		/* Y register */
unsigned char	SR;		/* Status register */
unsigned char	SP;		/* Stackpointer */
unsigned	PC;		/* Program counter */

/* Count the total number of cylcles */
unsigned	Cycles;		/* Cycles per insn */
unsigned long	TotalCycles;	/* Total cycles */

/* Allow the stack page to be changed */
static unsigned StackPage = 0x100;

/* Status register bits */
#define CF	0x01		/* Carry flag */
#define ZF	0x02		/* Zero flag */
#define IF	0x04		/* Interrupt flag */
#define DF	0x08		/* Decimal flag */
#define BF	0x10		/* Break flag */
#define OF	0x40		/* Overflow flag */
#define SF	0x80		/* Sign flag */

/* */
int CPUHalted;



/*****************************************************************************/
/*			  Helper functions and macros			     */
/*****************************************************************************/



/* Return the flags as a boolean value */
#define GET_CF()        ((SR & CF) != 0)
#define GET_ZF()        ((SR & ZF) != 0)
#define GET_IF()        ((SR & IF) != 0)
#define GET_DF()        ((SR & DF) != 0)
#define GET_BF()        ((SR & BF) != 0)
#define GET_OF()        ((SR & OF) != 0)
#define GET_SF()        ((SR & SF) != 0)

/* Set the flags. The parameter is a boolean flag that says if the flag should be
 * set or reset.
 */
#define SET_CF(f)       do { if (f) { SR |= CF; } else { SR &= ~CF; } } while (0)
#define SET_ZF(f)       do { if (f) { SR |= ZF; } else { SR &= ~ZF; } } while (0)
#define SET_IF(f)       do { if (f) { SR |= IF; } else { SR &= ~IF; } } while (0)
#define SET_DF(f)       do { if (f) { SR |= DF; } else { SR &= ~DF; } } while (0)
#define SET_BF(f)       do { if (f) { SR |= BF; } else { SR &= ~BF; } } while (0)
#define SET_OF(f)       do { if (f) { SR |= OF; } else { SR &= ~OF; } } while (0)
#define SET_SF(f)       do { if (f) { SR |= SF; } else { SR &= ~SF; } } while (0)

/* Special test and set macros. The meaning of the parameter depends on the
 * actual flag that should be set or reset.
 */
#define TEST_ZF(v)      SET_ZF (((v) & 0xFF) == 0)
#define TEST_SF(v)      SET_SF (((v) & 0x80) != 0)
#define TEST_CF(v)      SET_CF (((v) & 0xFF00) != 0)

/* Program counter halves */
#define PCL		(PC & 0xFF)
#define PCH		((PC >> 8) & 0xFF)

/* Stack operations */
#define PUSH(Val)       WriteMem (StackPage + SP--, Val)
#define POP()           ReadMem (StackPage + ++SP)

/* Test for page cross */
#define PAGE_CROSS(addr,offs)   ((((addr) & 0xFF) + offs) >= 0x100)

/* #imm */
#define AC_OP_IMM(op)                           \
    Cycles = 2;                                 \
    AC = AC op ReadMem (PC+1);                  \
    TEST_ZF (AC);                               \
    TEST_SF (AC);                               \
    PC += 2

/* zp */
#define AC_OP_ZP(op)                            \
    Cycles = 3;                                 \
    AC = AC op ReadMem (ReadMem (PC+1));        \
    TEST_ZF (AC);                               \
    TEST_SF (AC);                               \
    PC += 2

/* zp,x */
#define AC_OP_ZPX(op)                           \
    unsigned char ZPAddr;                       \
    Cycles = 4;                                 \
    ZPAddr = ReadMem (PC+1) + XR;               \
    AC = AC op ReadMem (ZPAddr);                \
    TEST_ZF (AC);                               \
    TEST_SF (AC);                               \
    PC += 2

/* zp,y */
#define AC_OP_ZPY(op)                           \
    unsigned char ZPAddr;                       \
    Cycles = 4;                                 \
    ZPAddr = ReadMem (PC+1) + YR;               \
    AC = AC op ReadMem (ZPAddr);                \
    TEST_ZF (AC);                               \
    TEST_SF (AC);                               \
    PC += 2

/* abs */
#define AC_OP_ABS(op)                           \
    unsigned Addr;                              \
    Cycles = 4;                                 \
    Addr = ReadMemW (PC+1);                     \
    AC = AC op ReadMem (Addr);                  \
    TEST_ZF (AC);                               \
    TEST_SF (AC);                               \
    PC += 3

/* abs,x */
#define AC_OP_ABSX(op)                          \
    unsigned Addr;                              \
    Cycles = 4;                                 \
    Addr = ReadMemW (PC+1);                     \
    if (PAGE_CROSS (Addr, XR)) {                \
        ++Cycles;                               \
    }                                           \
    AC = AC | ReadMem (Addr + XR);              \
    TEST_ZF (AC);                               \
    TEST_SF (AC);                               \
    PC += 3

/* abs,y */
#define AC_OP_ABSY(op)                          \
    unsigned Addr;                              \
    Cycles = 4;                                 \
    Addr = ReadMemW (PC+1);                     \
    if (PAGE_CROSS (Addr, YR)) {                \
        ++Cycles;                               \
    }                                           \
    AC = AC | ReadMem (Addr + YR);              \
    TEST_ZF (AC);                               \
    TEST_SF (AC);                               \
    PC += 3

/* (zp,x) */
#define AC_OP_ZPXIND(op)                        \
    unsigned char ZPAddr;                       \
    unsigned Addr;                              \
    Cycles = 6;                                 \
    ZPAddr = ReadMem (PC+1) + XR;               \
    Addr = ReadZeroPageW (ZPAddr);              \
    AC = AC op ReadMem (Addr);                  \
    TEST_ZF (AC);                               \
    TEST_SF (AC);                               \
    PC += 2

/* (zp),y */
#define AC_OP_ZPINDY(op)                        \
    unsigned char ZPAddr;                       \
    unsigned Addr;                              \
    Cycles = 5;                                 \
    ZPAddr = ReadMem (PC+1);                    \
    Addr = ReadZeroPageW (ZPAddr) + YR;         \
    AC = AC op ReadMem (Addr);                  \
    TEST_ZF (AC);                               \
    TEST_SF (AC);                               \
    PC += 2

/* branches */
#define BRANCH(cond)                            \
    Cycles = 2;                                 \
    if (cond) {                                 \
        signed char Offs;                       \
        unsigned char OldPCH;                   \
        ++Cycles;                               \
        Offs = (signed char) ReadMem (PC+1);    \
        OldPCH = PCH;                           \
        PC += (int) Offs;                       \
        if (PCH != OldPCH) {                    \
            ++Cycles;                           \
        }                                       \
    } else {                                    \
        PC += 2;                                \
    }

/* compares */
#define CMP(v1,v2)                              \
    {                                           \
        unsigned Result = v1 - v2;              \
        TEST_ZF (Result & 0xFF);                \
        TEST_SF (Result);                       \
        SET_CF (Result <= 0xFF);                \
    }



/*****************************************************************************/
/*                               Helper functions                            */
/*****************************************************************************/



static void Illegal (void)
{
    fprintf (stderr, "Illegal: $%02X\n", ReadMem (PC));
    exit (EXIT_FAILURE);
}



static void NotImplemented (void)
{
    fprintf (stderr, "Not implemented: $%02X\n", ReadMem (PC));
    exit (EXIT_FAILURE);
}



/*****************************************************************************/
/*  	  			     Code				     */
/*****************************************************************************/



static void OPC_6502_00 (void)
/* Opcode $00: BRK */
{
    Cycles = 7;
    PC += 2;
    SET_BF (1);
    PUSH (PCH);
    PUSH (PCL);
    PUSH (SR);
    SET_IF (1);
    PC = ReadMemW (0xFFFE);
}



static void OPC_6502_01 (void)
/* Opcode $01: ORA (ind,x) */
{
    AC_OP_ZPXIND (|);
}



static void OPC_6502_02 (void)
/* Opcode $02 */
{
    Illegal ();
}



static void OPC_6502_03 (void)
/* Opcode $03 */
{
    Illegal ();
}



static void OPC_6502_04 (void)
/* Opcode $04 */
{
    Illegal ();
}



static void OPC_6502_05 (void)
/* Opcode $05: ORA zp */
{
    AC_OP_ZP (|);
}



static void OPC_6502_06 (void)
/* Opcode $06: ASL zp */
{
    unsigned char ZPAddr;
    unsigned Val;
    Cycles = 5;
    ZPAddr = ReadMem (PC+1);
    Val    = ReadMem (ZPAddr) << 1;
    WriteMem (ZPAddr, (unsigned char) Val);
    TEST_ZF (Val & 0xFF);
    TEST_SF (Val);
    SET_CF (Val & 0x100);
    PC += 2;
}



static void OPC_6502_07 (void)
/* Opcode $07 */
{
    Illegal ();
}



static void OPC_6502_08 (void)
/* Opcode $08: PHP */
{
    Cycles = 3;
    PUSH (SR & ~BF);
    PC += 1;
}



static void OPC_6502_09 (void)
/* Opcode $09: ORA #imm */
{
    AC_OP_IMM (|);
}



static void OPC_6502_0A (void)
/* Opcode $0A: ASL a */
{
    unsigned Val;
    Cycles = 2;
    Val    = AC << 1;
    AC     = (unsigned char) Val;
    TEST_ZF (Val & 0xFF);
    TEST_SF (Val);
    SET_CF (Val & 0x100);
    PC += 1;
}



static void OPC_6502_0B (void)
/* Opcode $0B */
{
    Illegal ();
}



static void OPC_6502_0C (void)
/* Opcode $0C */
{
    Illegal ();
}



static void OPC_6502_0D (void)
/* Opcode $0D: ORA abs */
{
    AC_OP_ABS (|);
}



static void OPC_6502_0E (void)
/* Opcode $0E: ALS abs */
{
    unsigned Addr;
    unsigned Val;
    Cycles = 6;
    Addr = ReadMemW (PC+1);
    Val  = ReadMem (Addr) << 1;
    WriteMem (Addr, (unsigned char) Val);
    TEST_ZF (Val & 0xFF);
    TEST_SF (Val);
    SET_CF (Val & 0x100);
    PC += 3;
}



static void OPC_6502_0F (void)
/* Opcode $0F */
{
    Illegal ();
}



static void OPC_6502_10 (void)
/* Opcode $10: BPL */
{
    BRANCH (!GET_SF ());
}



static void OPC_6502_11 (void)
/* Opcode $11: ORA (zp),y */
{
    AC_OP_ZPINDY (|);
}



static void OPC_6502_12 (void)
/* Opcode $12 */
{
    Illegal ();
}



static void OPC_6502_13 (void)
/* Opcode $13 */
{
    Illegal ();
}



static void OPC_6502_14 (void)
/* Opcode $14 */
{
    Illegal ();
}



static void OPC_6502_15 (void)
/* Opcode $15: ORA zp,x */
{
   AC_OP_ZPX (|);
}



static void OPC_6502_16 (void)
/* Opcode $16: ASL zp,x */
{
    unsigned char ZPAddr;
    unsigned Val;
    Cycles = 6;
    ZPAddr = ReadMem (PC+1) + XR;
    Val    = ReadMem (ZPAddr) << 1;
    WriteMem (ZPAddr, (unsigned char) Val);
    TEST_ZF (Val & 0xFF);
    TEST_SF (Val);
    SET_CF (Val & 0x100);
    PC += 2;
}



static void OPC_6502_17 (void)
/* Opcode $17 */
{
    Illegal ();
}



static void OPC_6502_18 (void)
/* Opcode $18: CLC */
{
    Cycles = 2;
    SET_CF (0);
    PC += 1;
}



static void OPC_6502_19 (void)
/* Opcode $19: ORA abs,y */
{
    AC_OP_ABSY (|);
}



static void OPC_6502_1A (void)
/* Opcode $1A */
{
    Illegal ();
}



static void OPC_6502_1B (void)
/* Opcode $1B */
{
    Illegal ();
}



static void OPC_6502_1C (void)
/* Opcode $1C */
{
    Illegal ();
}



static void OPC_6502_1D (void)
/* Opcode $1D: ORA abs,x */
{
    AC_OP_ABSX (|);
}



static void OPC_6502_1E (void)
/* Opcode $1E: ASL abs,x */
{
    unsigned Addr;
    unsigned Val;
    Cycles = 7;
    Addr = ReadMemW (PC+1) + XR;
    Val  = ReadMem (Addr) << 1;
    WriteMem (Addr, (unsigned char) Val);
    TEST_ZF (Val & 0xFF);
    TEST_SF (Val);
    SET_CF (Val & 0x100);
    PC += 3;
}



static void OPC_6502_1F (void)
/* Opcode $1F */
{
    Illegal ();
}



static void OPC_6502_20 (void)
/* Opcode $20: JSR */
{
    unsigned Addr;
    Cycles = 6;
    Addr   = ReadMemW (PC+1);
    PC += 2;
    PUSH (PCH);
    PUSH (PCL);
    PC = Addr;
}



static void OPC_6502_21 (void)
/* Opcode $21: AND (zp,x) */
{
    AC_OP_ZPXIND (&);
}



static void OPC_6502_22 (void)
/* Opcode $22 */
{
    Illegal ();
}



static void OPC_6502_23 (void)
/* Opcode $23 */
{
    Illegal ();
}



static void OPC_6502_24 (void)
/* Opcode $24: BIT zp */
{
    unsigned char ZPAddr;
    unsigned char Val;
    Cycles = 3;
    ZPAddr = ReadMem (PC+1);
    Val    = ReadMem (ZPAddr);
    SET_SF (Val & 0x80);
    SET_OF (Val & 0x40);
    SET_ZF ((Val & AC) == 0);
    PC += 2;
}



static void OPC_6502_25 (void)
/* Opcode $25: AND zp */
{
    AC_OP_ZP (&);
}



static void OPC_6502_26 (void)
/* Opcode $26: ROL zp */
{
    unsigned char ZPAddr;
    unsigned Val;
    Cycles = 5;
    ZPAddr = ReadMem (PC+1);
    Val    = ReadMem (ZPAddr) << 1;
    if (GET_CF ()) {
        Val |= 0x01;
    }
    AC = (unsigned char) Val;
    TEST_ZF (AC);
    TEST_SF (AC);
    TEST_CF (Val);
    PC += 2;
}



static void OPC_6502_27 (void)
/* Opcode $27 */
{
    Illegal ();
}



static void OPC_6502_28 (void)
/* Opcode $28: PLP */
{
    Cycles = 4;
    SR = (POP () & ~BF);
    PC += 1;
}



static void OPC_6502_29 (void)
/* Opcode $29: AND #imm */
{
    AC_OP_IMM (&);
}



static void OPC_6502_2A (void)
/* Opcode $2A: ROL a */
{
    unsigned Val;
    Cycles = 2;
    Val    = AC << 1;
    if (GET_CF ()) {
        Val |= 0x01;
    }
    AC = (unsigned char) Val;
    TEST_ZF (AC);
    TEST_SF (AC);
    TEST_CF (Val);
    PC += 1;
}



static void OPC_6502_2B (void)
/* Opcode $2B */
{
    Illegal ();
}



static void OPC_6502_2C (void)
/* Opcode $2C: BIT abs */
{
    unsigned Addr;
    unsigned char Val;
    Cycles = 4;
    Addr = ReadMem (PC+1);
    Val  = ReadMem (Addr);
    SET_SF (Val & 0x80);
    SET_OF (Val & 0x40);
    SET_ZF ((Val & AC) == 0);
    PC += 3;
}



static void OPC_6502_2D (void)
/* Opcode $2D: AND abs */
{
    AC_OP_ABS (&);
}



static void OPC_6502_2E (void)
/* Opcode $2E: ROL abs */
{
    unsigned Addr;
    unsigned Val;
    Cycles = 6;
    Addr = ReadMemW (PC+1);
    Val  = ReadMem (Addr) << 1;
    if (GET_CF ()) {
        Val |= 0x01;
    }
    AC = (unsigned char) Val;
    TEST_ZF (AC);
    TEST_SF (AC);
    TEST_CF (Val);
    PC += 3;
}



static void OPC_6502_2F (void)
/* Opcode $2F */
{
    Illegal ();
}



static void OPC_6502_30 (void)
/* Opcode $30: BMI */
{
    BRANCH (GET_SF ());
}



static void OPC_6502_31 (void)
/* Opcode $31: AND (zp),y */
{
    AC_OP_ZPINDY (&);
}



static void OPC_6502_32 (void)
/* Opcode $32 */
{
    Illegal ();
}



static void OPC_6502_33 (void)
/* Opcode $33 */
{
    Illegal ();
}



static void OPC_6502_34 (void)
/* Opcode $34 */
{
    Illegal ();
}



static void OPC_6502_35 (void)
/* Opcode $35: AND zp,x */
{
    AC_OP_ZPX (&);
}



static void OPC_6502_36 (void)
/* Opcode $36: ROL zp,x */
{
    unsigned char ZPAddr;
    unsigned Val;
    Cycles = 6;
    ZPAddr = ReadMem (PC+1) + XR;
    Val    = ReadMem (ZPAddr) << 1;
    if (GET_CF ()) {
        Val |= 0x01;
    }
    AC = (unsigned char) Val;
    TEST_ZF (AC);
    TEST_SF (AC);
    TEST_CF (Val);
    PC += 2;
}



static void OPC_6502_37 (void)
/* Opcode $37 */
{
    Illegal ();
}



static void OPC_6502_38 (void)
/* Opcode $38: SEC */
{
    Cycles = 2;
    SET_CF (1);
    PC += 1;
}



static void OPC_6502_39 (void)
/* Opcode $39: AND abs,y */
{
    AC_OP_ABSY (&);
}



static void OPC_6502_3A (void)
/* Opcode $3A */
{
    Illegal ();
}



static void OPC_6502_3B (void)
/* Opcode $3B */
{
    Illegal ();
}



static void OPC_6502_3C (void)
/* Opcode $3C */
{
    Illegal ();
}



static void OPC_6502_3D (void)
/* Opcode $3D: AND abs,x */
{
    AC_OP_ABSX (&);
}



static void OPC_6502_3E (void)
/* Opcode $3E: ROL abs,x */
{
    unsigned Addr;
    unsigned Val;
    Cycles = 7;
    Addr = ReadMemW (PC+1) + XR;
    Val  = ReadMem (Addr) << 1;
    if (GET_CF ()) {
        Val |= 0x01;
    }
    AC = (unsigned char) Val;
    TEST_ZF (AC);
    TEST_SF (AC);
    TEST_CF (Val);
    PC += 2;
}



static void OPC_6502_3F (void)
/* Opcode $3F */
{
    Illegal ();
}



static void OPC_6502_40 (void)
/* Opcode $40: RTI */
{
    Cycles = 6;
    SR = POP ();
    PC = POP ();                /* PCL */
    PC |= (POP () << 8);        /* PCH */
}



static void OPC_6502_41 (void)
/* Opcode $41: EOR (zp,x) */
{
    AC_OP_ZPXIND (^);
}



static void OPC_6502_42 (void)
/* Opcode $42 */
{
    Illegal ();
}



static void OPC_6502_43 (void)
/* Opcode $43 */
{
    Illegal ();
}



static void OPC_6502_44 (void)
/* Opcode $44 */
{
    Illegal ();
}



static void OPC_6502_45 (void)
/* Opcode $45: EOR zp */
{
    AC_OP_ZP (^);
}



static void OPC_6502_46 (void)
/* Opcode $46: LSR zp */
{
    unsigned char ZPAddr;
    unsigned char Val;
    Cycles = 5;
    ZPAddr = ReadMem (PC+1);
    Val    = ReadMem (ZPAddr);
    SET_CF (Val & 0x01);
    Val >>= 1;
    WriteMem (ZPAddr, Val);
    TEST_ZF (Val);
    TEST_SF (Val);
    PC += 2;
}



static void OPC_6502_47 (void)
/* Opcode $47 */
{
    Illegal ();
}



static void OPC_6502_48 (void)
/* Opcode $48: PHA */
{
    Cycles = 3;
    PUSH (AC);
    PC += 1;
}



static void OPC_6502_49 (void)
/* Opcode $49: EOR #imm */
{
    AC_OP_IMM (^);
}



static void OPC_6502_4A (void)
/* Opcode $4A: LSR a */
{
    Cycles = 2;
    SET_CF (AC & 0x01);
    AC >>= 1;
    TEST_ZF (AC);
    TEST_SF (AC);
    PC += 1;
}



static void OPC_6502_4B (void)
/* Opcode $4B */
{
    Illegal ();
}



static void OPC_6502_4C (void)
/* Opcode $4C: JMP abs */
{
    Cycles = 3;
    PC = ReadMemW (PC+1);
}



static void OPC_6502_4D (void)
/* Opcode $4D: EOR abs */
{
    AC_OP_ABS (^);
}



static void OPC_6502_4E (void)
/* Opcode $4E: LSR abs */
{
    unsigned Addr;
    unsigned char Val;
    Cycles = 6;
    Addr = ReadMemW (PC+1);
    Val  = ReadMem (Addr);
    SET_CF (Val & 0x01);
    Val >>= 1;
    WriteMem (Addr, Val);
    TEST_ZF (Val);
    TEST_SF (Val);
    PC += 3;
}



static void OPC_6502_4F (void)
/* Opcode $4F */
{
    Illegal ();
}



static void OPC_6502_50 (void)
/* Opcode $50: BVC */
{
    BRANCH (!GET_OF ());
}



static void OPC_6502_51 (void)
/* Opcode $51: EOR (zp),y */
{
    AC_OP_ZPINDY (^);
}



static void OPC_6502_52 (void)
/* Opcode $52 */
{
    Illegal ();
}



static void OPC_6502_53 (void)
/* Opcode $53 */
{
    Illegal ();
}



static void OPC_6502_54 (void)
/* Opcode $54 */
{
    Illegal ();
}



static void OPC_6502_55 (void)
/* Opcode $55: EOR zp,x */
{
    AC_OP_ZPX (^);
}



static void OPC_6502_56 (void)
/* Opcode $56: LSR zp,x */
{
    unsigned char ZPAddr;
    unsigned char Val;
    Cycles = 6;
    ZPAddr = ReadMem (PC+1) + XR;
    Val    = ReadMem (ZPAddr);
    SET_CF (Val & 0x01);
    Val >>= 1;
    WriteMem (ZPAddr, Val);
    TEST_ZF (Val);
    TEST_SF (Val);
    PC += 2;
}



static void OPC_6502_57 (void)
/* Opcode $57 */
{
    Illegal ();
}



static void OPC_6502_58 (void)
/* Opcode $58: CLI */
{
    Cycles = 2;
    SET_IF (0);
    PC += 1;
}



static void OPC_6502_59 (void)
/* Opcode $59: EOR abs,y */
{
    AC_OP_ABSY (^);
}



static void OPC_6502_5A (void)
/* Opcode $5A */
{
    Illegal ();
}



static void OPC_6502_5B (void)
/* Opcode $5B */
{
    Illegal ();
}



static void OPC_6502_5C (void)
/* Opcode $5C */
{
    Illegal ();
}



static void OPC_6502_5D (void)
/* Opcode $5D: EOR abs,x */
{
    AC_OP_ABSX (^);
}



static void OPC_6502_5E (void)
/* Opcode $5E: LSR abs,x */
{
    unsigned Addr;
    unsigned char Val;
    Cycles = 7;
    Addr = ReadMemW (PC+1) + XR;
    Val  = ReadMem (Addr);
    SET_CF (Val & 0x01);
    Val >>= 1;
    WriteMem (Addr, Val);
    TEST_ZF (Val);
    TEST_SF (Val);
    PC += 3;
}



static void OPC_6502_5F (void)
/* Opcode $5F */
{
    Illegal ();
}



static void OPC_6502_60 (void)
/* Opcode $60: RTS */
{
    Cycles = 6;
    PC = POP ();                /* PCL */
    PC |= (POP () << 8);        /* PCH */
    PC += 1;
}



static void OPC_6502_61 (void)
/* Opcode $61: ADC (zp,x) */
{
    NotImplemented ();
}



static void OPC_6502_62 (void)
/* Opcode $62 */
{
    Illegal ();
}



static void OPC_6502_63 (void)
/* Opcode $63 */
{
    Illegal ();
}



static void OPC_6502_64 (void)
/* Opcode $64 */
{
    Illegal ();
}



static void OPC_6502_65 (void)
/* Opcode $65: ADC zp */
{
    NotImplemented ();
}



static void OPC_6502_66 (void)
/* Opcode $66: ROR zp */
{
    NotImplemented ();
}



static void OPC_6502_67 (void)
/* Opcode $67 */
{
    Illegal ();
}



static void OPC_6502_68 (void)
/* Opcode $68: PLA */
{
    Cycles = 4;
    AC = POP ();
    TEST_ZF (AC);
    TEST_SF (AC);
    PC += 1;
}



static void OPC_6502_69 (void)
/* Opcode $69: ADC #imm */
{
    NotImplemented ();
}



static void OPC_6502_6A (void)
/* Opcode $6A: ROR a */
{
    NotImplemented ();
}



static void OPC_6502_6B (void)
/* Opcode $6B */
{
    Illegal ();
}



static void OPC_6502_6C (void)
/* Opcode $6C: JMP (ind) */
{
    unsigned Addr;
    Cycles = 5;
    Addr = ReadMemW (PC+1);
    if (CPU == CPU_6502) {
        /* Emulate the 6502 bug */
        PC = ReadMem (Addr);
        Addr = (Addr & 0xFF00) | ((Addr + 1) & 0xFF);
        PC |= (ReadMem (Addr) << 8);
    } else {
        /* 65C02 and above have this bug fixed */
        PC = ReadMemW (Addr);
    }
    PC += 3;
}



static void OPC_6502_6D (void)
/* Opcode $6D: ADC abs */
{
    NotImplemented ();
}



static void OPC_6502_6E (void)
/* Opcode $6E: ROR abs */
{
    NotImplemented ();
}



static void OPC_6502_6F (void)
/* Opcode $6F */
{
    Illegal ();
}



static void OPC_6502_70 (void)
/* Opcode $70: BVS */
{
    BRANCH (GET_OF ());
}



static void OPC_6502_71 (void)
/* Opcode $71: ADC (zp),y */
{
    NotImplemented ();
}



static void OPC_6502_72 (void)
/* Opcode $72 */
{
    Illegal ();
}



static void OPC_6502_73 (void)
/* Opcode $73 */
{
    Illegal ();
}



static void OPC_6502_74 (void)
/* Opcode $74 */
{
    Illegal ();
}



static void OPC_6502_75 (void)
/* Opcode $75: ADC zp,x */
{
    NotImplemented ();
}



static void OPC_6502_76 (void)
/* Opcode $76: ROR zp,x */
{
    NotImplemented ();
}



static void OPC_6502_77 (void)
/* Opcode $77 */
{
    Illegal ();
}



static void OPC_6502_78 (void)
/* Opcode $78: SEI */
{
    Cycles = 2;
    SET_IF (1);
    PC += 1;
}



static void OPC_6502_79 (void)
/* Opcode $79: ADC abs,y */
{
    NotImplemented ();
}



static void OPC_6502_7A (void)
/* Opcode $7A */
{
    Illegal ();
}



static void OPC_6502_7B (void)
/* Opcode $7B */
{
    Illegal ();
}



static void OPC_6502_7C (void)
/* Opcode $7C */
{
    Illegal ();
}



static void OPC_6502_7D (void)
/* Opcode $7D: ADC abs,x */
{
    NotImplemented ();
}



static void OPC_6502_7E (void)
/* Opcode $7E: ROR abs,x */
{
    NotImplemented ();
}



static void OPC_6502_7F (void)
/* Opcode $7F */
{
    Illegal ();
}



static void OPC_6502_80 (void)
/* Opcode $80 */
{
    Illegal ();
}



static void OPC_6502_81 (void)
/* Opcode $81: STA (zp,x) */
{
    NotImplemented ();
}



static void OPC_6502_82 (void)
/* Opcode $82 */
{
    Illegal ();
}



static void OPC_6502_83 (void)
/* Opcode $83 */
{
    Illegal ();
}



static void OPC_6502_84 (void)
/* Opcode $84: STY zp */
{
    unsigned char ZPAddr;
    Cycles = 3;
    ZPAddr = ReadMem (PC+1);
    WriteMem (ZPAddr, YR);
    PC += 2;
}



static void OPC_6502_85 (void)
/* Opcode $85: STA zp */
{
    unsigned char ZPAddr;
    Cycles = 3;
    ZPAddr = ReadMem (PC+1);
    WriteMem (ZPAddr, AC);
    PC += 2;
}



static void OPC_6502_86 (void)
/* Opcode $86: STX zp */
{
    unsigned char ZPAddr;
    Cycles = 3;
    ZPAddr = ReadMem (PC+1);
    WriteMem (ZPAddr, XR);
    PC += 2;
}



static void OPC_6502_87 (void)
/* Opcode $87 */
{
    Illegal ();
}



static void OPC_6502_88 (void)
/* Opcode $88: DEY */
{
    Cycles = 2;
    --YR;
    TEST_ZF (YR);
    TEST_SF (YR);
    PC += 1;
}



static void OPC_6502_89 (void)
/* Opcode $89 */
{
    Illegal ();
}



static void OPC_6502_8A (void)
/* Opcode $8A: TXA */
{
    Cycles = 2;
    AC = XR;
    TEST_ZF (AC);
    TEST_SF (AC);
    PC += 1;
}



static void OPC_6502_8B (void)
/* Opcode $8B */
{
    Illegal ();
}



static void OPC_6502_8C (void)
/* Opcode $8C: STY abs */
{
    unsigned Addr;
    Cycles = 4;
    Addr = ReadMemW (PC+1);
    WriteMem (Addr, YR);
    PC += 3;
}



static void OPC_6502_8D (void)
/* Opcode $8D: STA abs */
{
    unsigned Addr;
    Cycles = 4;
    Addr = ReadMemW (PC+1);
    WriteMem (Addr, AC);
    PC += 3;
}



static void OPC_6502_8E (void)
/* Opcode $8E: STX abs */
{
    unsigned Addr;
    Cycles = 4;
    Addr = ReadMemW (PC+1);
    WriteMem (Addr, XR);
    PC += 3;
}



static void OPC_6502_8F (void)
/* Opcode $8F */
{
    Illegal ();
}



static void OPC_6502_90 (void)
/* Opcode $90: BCC */
{
    BRANCH (!GET_CF ());
}



static void OPC_6502_91 (void)
/* Opcode $91: sta (zp),y */
{
    unsigned char ZPAddr;
    unsigned Addr;
    Cycles = 6;
    ZPAddr = ReadMem (PC+1);
    Addr   = ReadZeroPageW (ZPAddr) + YR;
    WriteMem (Addr, AC);
    PC += 2;
}



static void OPC_6502_92 (void)
/* Opcode $92 */
{
    Illegal ();
}



static void OPC_6502_93 (void)
/* Opcode $93 */
{
    Illegal ();
}



static void OPC_6502_94 (void)
/* Opcode $94: STY zp,x */
{
    unsigned char ZPAddr;
    Cycles = 4;
    ZPAddr = ReadMem (PC+1) + XR;
    WriteMem (ZPAddr, YR);
    PC += 2;
}



static void OPC_6502_95 (void)
/* Opcode $95: STA zp,x */
{
    unsigned char ZPAddr;
    Cycles = 4;
    ZPAddr = ReadMem (PC+1) + XR;
    WriteMem (ZPAddr, AC);
    PC += 2;
}



static void OPC_6502_96 (void)
/* Opcode $96: stx zp,y */
{
    unsigned char ZPAddr;
    Cycles = 4;
    ZPAddr = ReadMem (PC+1) + YR;
    WriteMem (ZPAddr, XR);
    PC += 2;
}



static void OPC_6502_97 (void)
/* Opcode $97 */
{
    Illegal ();
}



static void OPC_6502_98 (void)
/* Opcode $98: TYA */
{
    Cycles = 2;
    AC = YR;
    TEST_ZF (AC);
    TEST_SF (AC);
    PC += 1;
}



static void OPC_6502_99 (void)
/* Opcode $99: STA abs,y */
{
    unsigned Addr;
    Cycles = 5;
    Addr   = ReadMemW (PC+1) + YR;
    WriteMem (Addr, AC);
    PC += 3;
}



static void OPC_6502_9A (void)
/* Opcode $9A: TXS */
{
    Cycles = 2;
    SP = XR;
    PC += 1;
}



static void OPC_6502_9B (void)
/* Opcode $9B */
{
    Illegal ();
}



static void OPC_6502_9C (void)
/* Opcode $9C */
{
    Illegal ();
}



static void OPC_6502_9D (void)
/* Opcode $9D: STA abs,x */
{
    unsigned Addr;
    Cycles = 5;
    Addr   = ReadMemW (PC+1) + XR;
    WriteMem (Addr, AC);
    PC += 3;
}



static void OPC_6502_9E (void)
/* Opcode $9E */
{
    Illegal ();
}



static void OPC_6502_9F (void)
/* Opcode $9F */
{
    Illegal ();
}



static void OPC_6502_A0 (void)
/* Opcode $A0: LDY #imm */
{
    Cycles = 2;
    YR = ReadMem (PC+1);
    TEST_ZF (YR);
    TEST_SF (YR);
    PC += 2;
}



static void OPC_6502_A1 (void)
/* Opcode $A1: LDA (zp,x) */
{
    unsigned char ZPAddr;
    unsigned Addr;
    Cycles = 6;
    ZPAddr = ReadMem (PC+1) + XR;
    Addr = ReadZeroPageW (ZPAddr);
    AC = ReadMem (Addr);
    TEST_ZF (AC);
    TEST_SF (AC);
    PC += 2;
}



static void OPC_6502_A2 (void)
/* Opcode $A2: LDX #imm */
{
    Cycles = 2;
    XR = ReadMem (PC+1);
    TEST_ZF (XR);
    TEST_SF (XR);
    PC += 2;
}



static void OPC_6502_A3 (void)
/* Opcode $A3 */
{
    Illegal ();
}



static void OPC_6502_A4 (void)
/* Opcode $A4: LDY zp */
{
    unsigned char ZPAddr;
    Cycles = 3;
    ZPAddr = ReadMem (PC+1);
    YR = ReadMem (ZPAddr);
    TEST_ZF (YR);
    TEST_SF (YR);
    PC += 2;
}



static void OPC_6502_A5 (void)
/* Opcode $A5: LDA zp */
{
    unsigned char ZPAddr;
    Cycles = 3;
    ZPAddr = ReadMem (PC+1);
    AC = ReadMem (ZPAddr);
    TEST_ZF (AC);
    TEST_SF (AC);
    PC += 2;
}



static void OPC_6502_A6 (void)
/* Opcode $A6: LDX zp */
{
    unsigned char ZPAddr;
    Cycles = 3;
    ZPAddr = ReadMem (PC+1);
    XR = ReadMem (ZPAddr);
    TEST_ZF (XR);
    TEST_SF (XR);
    PC += 2;
}



static void OPC_6502_A7 (void)
/* Opcode $A7 */
{
    Illegal ();
}



static void OPC_6502_A8 (void)
/* Opcode $A8: TAY */
{
    Cycles = 2;
    YR = AC;
    TEST_ZF (YR);
    TEST_SF (YR);
    PC += 1;
}



static void OPC_6502_A9 (void)
/* Opcode $A9: LDA #imm */
{
    Cycles = 2;
    AC = ReadMem (PC+1);
    TEST_ZF (AC);
    TEST_SF (AC);
    PC += 2;
}



static void OPC_6502_AA (void)
/* Opcode $AA: TAX */
{
    Cycles = 2;
    XR = AC;
    TEST_ZF (XR);
    TEST_SF (XR);
    PC += 1;
}



static void OPC_6502_AB (void)
/* Opcode $AB */
{
    Illegal ();
}



static void OPC_6502_AC (void)
/* Opcode $AC: LDY abs */
{
    unsigned Addr;
    Cycles = 4;
    Addr   = ReadMemW (PC+1);
    YR     = ReadMem (Addr);
    TEST_ZF (YR);
    TEST_SF (YR);
    PC += 3;
}



static void OPC_6502_AD (void)
/* Opcode $AD: LDA abs */
{
    unsigned Addr;
    Cycles = 4;
    Addr   = ReadMemW (PC+1);
    AC     = ReadMem (Addr);
    TEST_ZF (AC);
    TEST_SF (AC);
    PC += 3;
}



static void OPC_6502_AE (void)
/* Opcode $AE: LDX abs */
{
    unsigned Addr;
    Cycles = 4;
    Addr   = ReadMemW (PC+1);
    XR     = ReadMem (Addr);
    TEST_ZF (XR);
    TEST_SF (XR);
    PC += 3;
}



static void OPC_6502_AF (void)
/* Opcode $AF */
{
    Illegal ();
}



static void OPC_6502_B0 (void)
/* Opcode $B0: BCS */
{
    BRANCH (GET_CF ());
}



static void OPC_6502_B1 (void)
/* Opcode $B1: LDA (zp),y */
{
    unsigned char ZPAddr;
    unsigned Addr;
    Cycles = 5;
    ZPAddr = ReadMem (PC+1);
    Addr   = ReadZeroPageW (ZPAddr);
    if (PAGE_CROSS (Addr, YR)) {
        ++Cycles;
    }
    AC = ReadMem (Addr + YR);
    TEST_ZF (AC);
    TEST_SF (AC);
    PC += 2;
}



static void OPC_6502_B2 (void)
/* Opcode $B2 */
{
    Illegal ();
}



static void OPC_6502_B3 (void)
/* Opcode $B3 */
{
    Illegal ();
}



static void OPC_6502_B4 (void)
/* Opcode $B4: LDY zp,x */
{
    unsigned char ZPAddr;
    Cycles = 4;
    ZPAddr = ReadMem (PC+1) + XR;
    YR     = ReadMem (ZPAddr);
    TEST_ZF (YR);
    TEST_SF (YR);
    PC += 2;
}



static void OPC_6502_B5 (void)
/* Opcode $B5: LDA zp,x */
{
    unsigned char ZPAddr;
    Cycles = 4;
    ZPAddr = ReadMem (PC+1) + XR;
    AC     = ReadMem (ZPAddr);
    TEST_ZF (AC);
    TEST_SF (AC);
    PC += 2;
}



static void OPC_6502_B6 (void)
/* Opcode $B6: LDX zp,y */
{
    unsigned char ZPAddr;
    Cycles = 4;
    ZPAddr = ReadMem (PC+1) + YR;
    XR     = ReadMem (ZPAddr);
    TEST_ZF (XR);
    TEST_SF (XR);
    PC += 2;
}



static void OPC_6502_B7 (void)
/* Opcode $B7 */
{
    Illegal ();
}



static void OPC_6502_B8 (void)
/* Opcode $B8: CLV */
{
    Cycles = 2;
    SET_OF (0);
    PC += 1;
}



static void OPC_6502_B9 (void)
/* Opcode $B9: LDA abs,y */
{
    unsigned Addr;
    Cycles = 4;
    Addr = ReadMemW (PC+1);
    if (PAGE_CROSS (Addr, YR)) {
        ++Cycles;
    }
    AC = ReadMem (Addr + YR);
    TEST_ZF (AC);
    TEST_SF (AC);
    PC += 3;
}



static void OPC_6502_BA (void)
/* Opcode $BA: TSX */
{
    Cycles = 2;
    XR = SP;
    TEST_ZF (XR);
    TEST_SF (XR);
    PC += 1;
}



static void OPC_6502_BB (void)
/* Opcode $BB */
{
    Illegal ();
}



static void OPC_6502_BC (void)
/* Opcode $BC: LDY abs,x */
{
    unsigned Addr;
    Cycles = 4;
    Addr = ReadMemW (PC+1);
    if (PAGE_CROSS (Addr, XR)) {
        ++Cycles;
    }
    YR = ReadMem (Addr + XR);
    TEST_ZF (YR);
    TEST_SF (YR);
    PC += 3;
}



static void OPC_6502_BD (void)
/* Opcode $BD: LDA abs,x */
{
    unsigned Addr;
    Cycles = 4;
    Addr = ReadMemW (PC+1);
    if (PAGE_CROSS (Addr, XR)) {
        ++Cycles;
    }
    AC = ReadMem (Addr + XR);
    TEST_ZF (AC);
    TEST_SF (AC);
    PC += 3;
}



static void OPC_6502_BE (void)
/* Opcode $BE: LDX abs,y */
{
    unsigned Addr;
    Cycles = 4;
    Addr = ReadMemW (PC+1);
    if (PAGE_CROSS (Addr, YR)) {
        ++Cycles;
    }
    XR = ReadMem (Addr + YR);
    TEST_ZF (XR);
    TEST_SF (XR);
    PC += 3;
}



static void OPC_6502_BF (void)
/* Opcode $BF */
{
    Illegal ();
}



static void OPC_6502_C0 (void)
/* Opcode $C0: CPY #imm */
{
    Cycles = 2;
    CMP (YR, ReadMem (PC+1));
    PC += 2;
}



static void OPC_6502_C1 (void)
/* Opcode $C1: CMP (zp,x) */
{
    unsigned char ZPAddr;
    unsigned Addr;
    Cycles = 6;
    ZPAddr = ReadMem (PC+1) + XR;
    Addr   = ReadZeroPageW (ZPAddr);
    CMP (AC, ReadMem (Addr));
    PC += 2;
}



static void OPC_6502_C2 (void)
/* Opcode $C2 */
{
    Illegal ();
}



static void OPC_6502_C3 (void)
/* Opcode $C3 */
{
    Illegal ();
}



static void OPC_6502_C4 (void)
/* Opcode $C4: CPY zp */
{
    unsigned char ZPAddr;
    Cycles = 3;
    ZPAddr = ReadMem (PC+1);
    CMP (YR, ReadMem (ZPAddr));
    PC += 2;
}



static void OPC_6502_C5 (void)
/* Opcode $C5: CMP zp */
{
    unsigned char ZPAddr;
    Cycles = 3;
    ZPAddr = ReadMem (PC+1);
    CMP (AC, ReadMem (ZPAddr));
    PC += 2;
}



static void OPC_6502_C6 (void)
/* Opcode $C6: DEC zp */
{
    unsigned char ZPAddr;
    unsigned char Val;
    Cycles = 5;
    ZPAddr = ReadMem (PC+1);
    Val    = ReadMem (ZPAddr) - 1;
    WriteMem (ZPAddr, Val);
    TEST_ZF (Val);
    TEST_SF (Val);
    PC += 2;
}



static void OPC_6502_C7 (void)
/* Opcode $C7 */
{
    Illegal ();
}



static void OPC_6502_C8 (void)
/* Opcode $C8: INY */
{
    Cycles = 2;
    ++YR;
    TEST_ZF (YR);
    TEST_SF (YR);
    PC += 1;
}



static void OPC_6502_C9 (void)
/* Opcode $C9: CMP #imm */
{
    Cycles = 2;
    CMP (AC, ReadMem (PC+1));
    PC += 2;
}



static void OPC_6502_CA (void)
/* Opcode $CA: DEX */
{
    Cycles = 2;
    --XR;
    TEST_ZF (XR);
    TEST_SF (XR);
    PC += 1;
}



static void OPC_6502_CB (void)
/* Opcode $CB */
{
    Illegal ();
}



static void OPC_6502_CC (void)
/* Opcode $CC: CPY abs */
{
    unsigned Addr;
    Cycles = 4;
    Addr   = ReadMemW (PC+1);
    CMP (YR, ReadMem (Addr));
    PC += 3;
}



static void OPC_6502_CD (void)
/* Opcode $CD: CMP abs */
{
    unsigned Addr;
    Cycles = 4;
    Addr   = ReadMemW (PC+1);
    CMP (AC, ReadMem (Addr));
    PC += 3;
}



static void OPC_6502_CE (void)
/* Opcode $CE: DEC abs */
{
    unsigned Addr;
    unsigned char Val;
    Cycles = 6;
    Addr = ReadMemW (PC+1);
    Val  = ReadMem (Addr) - 1;
    WriteMem (Addr, Val);
    TEST_ZF (Val);
    TEST_SF (Val);
    PC += 3;
}



static void OPC_6502_CF (void)
/* Opcode $CF */
{
    Illegal ();
}



static void OPC_6502_D0 (void)
/* Opcode $D0: BNE */
{
    BRANCH (!GET_ZF ());
}



static void OPC_6502_D1 (void)
/* Opcode $D1: CMP (zp),y */
{
    unsigned ZPAddr;
    unsigned Addr;
    Cycles = 5;
    ZPAddr = ReadMem (PC+1);
    Addr   = ReadMemW (ZPAddr);
    if (PAGE_CROSS (Addr, YR)) {
        ++Cycles;
    }
    CMP (AC, ReadMem (Addr + YR));
    PC += 2;
}



static void OPC_6502_D2 (void)
/* Opcode $D2 */
{
    Illegal ();
}



static void OPC_6502_D3 (void)
/* Opcode $D3 */
{
    Illegal ();
}



static void OPC_6502_D4 (void)
/* Opcode $D4 */
{
    Illegal ();
}



static void OPC_6502_D5 (void)
/* Opcode $D5: CMP zp,x */
{
    unsigned char ZPAddr;
    Cycles = 4;
    ZPAddr = ReadMem (PC+1) + XR;
    CMP (AC, ReadMem (ZPAddr));
    PC += 2;
}



static void OPC_6502_D6 (void)
/* Opcode $D6: DEC zp,x */
{
    unsigned char ZPAddr;
    unsigned char Val;
    Cycles = 6;
    ZPAddr = ReadMem (PC+1) + XR;
    Val  = ReadMem (ZPAddr) - 1;
    WriteMem (ZPAddr, Val);
    TEST_ZF (Val);
    TEST_SF (Val);
    PC += 2;
}



static void OPC_6502_D7 (void)
/* Opcode $D7 */
{
    Illegal ();
}



static void OPC_6502_D8 (void)
/* Opcode $D8: CLD */
{
    Cycles = 2;
    SET_DF (0);
    PC += 1;
}



static void OPC_6502_D9 (void)
/* Opcode $D9: CMP abs,y */
{
    unsigned Addr;
    Cycles = 4;
    Addr = ReadMemW (PC+1);
    if (PAGE_CROSS (Addr, YR)) {
        ++Cycles;
    }
    CMP (AC, ReadMem (Addr + YR));
    PC += 3;
}



static void OPC_6502_DA (void)
/* Opcode $DA */
{
    Illegal ();
}



static void OPC_6502_DB (void)
/* Opcode $DB */
{
    Illegal ();
}



static void OPC_6502_DC (void)
/* Opcode $DC */
{
    Illegal ();
}



static void OPC_6502_DD (void)
/* Opcode $DD: CMP abs,x */
{
    unsigned Addr;
    Cycles = 4;
    Addr = ReadMemW (PC+1);
    if (PAGE_CROSS (Addr, XR)) {
        ++Cycles;
    }
    CMP (AC, ReadMem (Addr + XR));
    PC += 3;
}



static void OPC_6502_DE (void)
/* Opcode $DE: DEC abs,x */
{
    unsigned Addr;
    unsigned char Val;
    Cycles = 7;
    Addr = ReadMemW (PC+1) + XR;
    Val  = ReadMem (Addr) - 1;
    WriteMem (Addr, Val);
    TEST_ZF (Val);
    TEST_SF (Val);
    PC += 3;
}



static void OPC_6502_DF (void)
/* Opcode $DF */
{
    Illegal ();
}



static void OPC_6502_E0 (void)
/* Opcode $E0: CPX #imm */
{
    Cycles = 2;
    CMP (XR, ReadMem (PC+1));
    PC += 2;
}



static void OPC_6502_E1 (void)
/* Opcode $E1: SBC (zp,x) */
{
    NotImplemented ();
}



static void OPC_6502_E2 (void)
/* Opcode $E2 */
{
    Illegal ();
}



static void OPC_6502_E3 (void)
/* Opcode $E3 */
{
    Illegal ();
}



static void OPC_6502_E4 (void)
/* Opcode $E4: CPX zp */
{
    unsigned char ZPAddr;
    Cycles = 3;
    ZPAddr = ReadMem (PC+1);
    CMP (XR, ReadMem (ZPAddr));
    PC += 2;
}



static void OPC_6502_E5 (void)
/* Opcode $E5: SBC zp */
{
    NotImplemented ();
}



static void OPC_6502_E6 (void)
/* Opcode $E6: INC zp */
{
    unsigned char ZPAddr;
    unsigned char Val;
    Cycles = 5;
    ZPAddr = ReadMem (PC+1);
    Val    = ReadMem (ZPAddr) + 1;
    WriteMem (ZPAddr, Val);
    TEST_ZF (Val);
    TEST_SF (Val);
    PC += 2;
}



static void OPC_6502_E7 (void)
/* Opcode $E7 */
{
    Illegal ();
}



static void OPC_6502_E8 (void)
/* Opcode $E8: INX */
{
    Cycles = 2;
    ++XR;
    TEST_ZF (XR);
    TEST_SF (XR);
    PC += 1;
}



static void OPC_6502_E9 (void)
/* Opcode $E9: SBC #imm */
{
    NotImplemented ();
}



static void OPC_6502_EA (void)
/* Opcode $EA: NOP */
{
    /* This one is easy... */
    Cycles = 2;
    PC += 1;
}



static void OPC_6502_EB (void)
/* Opcode $EB */
{
    Illegal ();
}



static void OPC_6502_EC (void)
/* Opcode $EC: CPX abs */
{
    unsigned Addr;
    Cycles = 4;
    Addr   = ReadMemW (PC+1);
    CMP (XR, ReadMem (Addr));
    PC += 3;
}



static void OPC_6502_ED (void)
/* Opcode $ED: SBC abs */
{
    NotImplemented ();
}



static void OPC_6502_EE (void)
/* Opcode $EE: INC abs */
{
    unsigned Addr;
    unsigned char Val;
    Cycles = 6;
    Addr = ReadMemW (PC+1);
    Val  = ReadMem (Addr) + 1;
    WriteMem (Addr, Val);
    TEST_ZF (Val);
    TEST_SF (Val);
    PC += 3;
}



static void OPC_6502_EF (void)
/* Opcode $EF */
{
    Illegal ();
}



static void OPC_6502_F0 (void)
/* Opcode $F0: BEQ */
{
    BRANCH (GET_ZF ());
}



static void OPC_6502_F1 (void)
/* Opcode $F1: SBC (zp),y */
{
    NotImplemented ();
}



static void OPC_6502_F2 (void)
/* Opcode $F2 */
{
    Illegal ();
}



static void OPC_6502_F3 (void)
/* Opcode $F3 */
{
    Illegal ();
}



static void OPC_6502_F4 (void)
/* Opcode $F4 */
{
    Illegal ();
}



static void OPC_6502_F5 (void)
/* Opcode $F5: SBC zp,x */
{
    NotImplemented ();
}



static void OPC_6502_F6 (void)
/* Opcode $F6: INC zp,x */
{
    unsigned char ZPAddr;
    unsigned char Val;
    Cycles = 6;
    ZPAddr = ReadMem (PC+1) + XR;
    Val  = ReadMem (ZPAddr) + 1;
    WriteMem (ZPAddr, Val);
    TEST_ZF (Val);
    TEST_SF (Val);
    PC += 2;
}



static void OPC_6502_F7 (void)
/* Opcode $F7 */
{
    Illegal ();
}



static void OPC_6502_F8 (void)
/* Opcode $F8: SED */
{
    SET_DF (1);
}



static void OPC_6502_F9 (void)
/* Opcode $F9: SBC abs,y */
{
    NotImplemented ();
}



static void OPC_6502_FA (void)
/* Opcode $FA */
{
    Illegal ();
}



static void OPC_6502_FB (void)
/* Opcode $FB */
{
    Illegal ();
}



static void OPC_6502_FC (void)
/* Opcode $FC */
{
    Illegal ();
}



static void OPC_6502_FD (void)
/* Opcode $FD: SBC abs,x */
{
    NotImplemented ();
}



static void OPC_6502_FE (void)
/* Opcode $FE: INC abs,x */
{
    unsigned Addr;
    unsigned char Val;
    Cycles = 7;
    Addr = ReadMemW (PC+1) + XR;
    Val  = ReadMem (Addr) + 1;
    WriteMem (Addr, Val);
    TEST_ZF (Val);
    TEST_SF (Val);
    PC += 3;
}



static void OPC_6502_FF (void)
/* Opcode $FF */
{
    Illegal ();
}



/*****************************************************************************/
/*				     Data				     */
/*****************************************************************************/



/* Opcode handler table */
typedef void (*OPCFunc) (void);
static OPCFunc OPCTable[256] = {
    OPC_6502_00,
    OPC_6502_01,
    OPC_6502_02,
    OPC_6502_03,
    OPC_6502_04,
    OPC_6502_05,
    OPC_6502_06,
    OPC_6502_07,
    OPC_6502_08,
    OPC_6502_09,
    OPC_6502_0A,
    OPC_6502_0B,
    OPC_6502_0C,
    OPC_6502_0D,
    OPC_6502_0E,
    OPC_6502_0F,
    OPC_6502_10,
    OPC_6502_11,
    OPC_6502_12,
    OPC_6502_13,
    OPC_6502_14,
    OPC_6502_15,
    OPC_6502_16,
    OPC_6502_17,
    OPC_6502_18,
    OPC_6502_19,
    OPC_6502_1A,
    OPC_6502_1B,
    OPC_6502_1C,
    OPC_6502_1D,
    OPC_6502_1E,
    OPC_6502_1F,
    OPC_6502_20,
    OPC_6502_21,
    OPC_6502_22,
    OPC_6502_23,
    OPC_6502_24,
    OPC_6502_25,
    OPC_6502_26,
    OPC_6502_27,
    OPC_6502_28,
    OPC_6502_29,
    OPC_6502_2A,
    OPC_6502_2B,
    OPC_6502_2C,
    OPC_6502_2D,
    OPC_6502_2E,
    OPC_6502_2F,
    OPC_6502_30,
    OPC_6502_31,
    OPC_6502_32,
    OPC_6502_33,
    OPC_6502_34,
    OPC_6502_35,
    OPC_6502_36,
    OPC_6502_37,
    OPC_6502_38,
    OPC_6502_39,
    OPC_6502_3A,
    OPC_6502_3B,
    OPC_6502_3C,
    OPC_6502_3D,
    OPC_6502_3E,
    OPC_6502_3F,
    OPC_6502_40,
    OPC_6502_41,
    OPC_6502_42,
    OPC_6502_43,
    OPC_6502_44,
    OPC_6502_45,
    OPC_6502_46,
    OPC_6502_47,
    OPC_6502_48,
    OPC_6502_49,
    OPC_6502_4A,
    OPC_6502_4B,
    OPC_6502_4C,
    OPC_6502_4D,
    OPC_6502_4E,
    OPC_6502_4F,
    OPC_6502_50,
    OPC_6502_51,
    OPC_6502_52,
    OPC_6502_53,
    OPC_6502_54,
    OPC_6502_55,
    OPC_6502_56,
    OPC_6502_57,
    OPC_6502_58,
    OPC_6502_59,
    OPC_6502_5A,
    OPC_6502_5B,
    OPC_6502_5C,
    OPC_6502_5D,
    OPC_6502_5E,
    OPC_6502_5F,
    OPC_6502_60,
    OPC_6502_61,
    OPC_6502_62,
    OPC_6502_63,
    OPC_6502_64,
    OPC_6502_65,
    OPC_6502_66,
    OPC_6502_67,
    OPC_6502_68,
    OPC_6502_69,
    OPC_6502_6A,
    OPC_6502_6B,
    OPC_6502_6C,
    OPC_6502_6D,
    OPC_6502_6E,
    OPC_6502_6F,
    OPC_6502_70,
    OPC_6502_71,
    OPC_6502_72,
    OPC_6502_73,
    OPC_6502_74,
    OPC_6502_75,
    OPC_6502_76,
    OPC_6502_77,
    OPC_6502_78,
    OPC_6502_79,
    OPC_6502_7A,
    OPC_6502_7B,
    OPC_6502_7C,
    OPC_6502_7D,
    OPC_6502_7E,
    OPC_6502_7F,
    OPC_6502_80,
    OPC_6502_81,
    OPC_6502_82,
    OPC_6502_83,
    OPC_6502_84,
    OPC_6502_85,
    OPC_6502_86,
    OPC_6502_87,
    OPC_6502_88,
    OPC_6502_89,
    OPC_6502_8A,
    OPC_6502_8B,
    OPC_6502_8C,
    OPC_6502_8D,
    OPC_6502_8E,
    OPC_6502_8F,
    OPC_6502_90,
    OPC_6502_91,
    OPC_6502_92,
    OPC_6502_93,
    OPC_6502_94,
    OPC_6502_95,
    OPC_6502_96,
    OPC_6502_97,
    OPC_6502_98,
    OPC_6502_99,
    OPC_6502_9A,
    OPC_6502_9B,
    OPC_6502_9C,
    OPC_6502_9D,
    OPC_6502_9E,
    OPC_6502_9F,
    OPC_6502_A0,
    OPC_6502_A1,
    OPC_6502_A2,
    OPC_6502_A3,
    OPC_6502_A4,
    OPC_6502_A5,
    OPC_6502_A6,
    OPC_6502_A7,
    OPC_6502_A8,
    OPC_6502_A9,
    OPC_6502_AA,
    OPC_6502_AB,
    OPC_6502_AC,
    OPC_6502_AD,
    OPC_6502_AE,
    OPC_6502_AF,
    OPC_6502_B0,
    OPC_6502_B1,
    OPC_6502_B2,
    OPC_6502_B3,
    OPC_6502_B4,
    OPC_6502_B5,
    OPC_6502_B6,
    OPC_6502_B7,
    OPC_6502_B8,
    OPC_6502_B9,
    OPC_6502_BA,
    OPC_6502_BB,
    OPC_6502_BC,
    OPC_6502_BD,
    OPC_6502_BE,
    OPC_6502_BF,
    OPC_6502_C0,
    OPC_6502_C1,
    OPC_6502_C2,
    OPC_6502_C3,
    OPC_6502_C4,
    OPC_6502_C5,
    OPC_6502_C6,
    OPC_6502_C7,
    OPC_6502_C8,
    OPC_6502_C9,
    OPC_6502_CA,
    OPC_6502_CB,
    OPC_6502_CC,
    OPC_6502_CD,
    OPC_6502_CE,
    OPC_6502_CF,
    OPC_6502_D0,
    OPC_6502_D1,
    OPC_6502_D2,
    OPC_6502_D3,
    OPC_6502_D4,
    OPC_6502_D5,
    OPC_6502_D6,
    OPC_6502_D7,
    OPC_6502_D8,
    OPC_6502_D9,
    OPC_6502_DA,
    OPC_6502_DB,
    OPC_6502_DC,
    OPC_6502_DD,
    OPC_6502_DE,
    OPC_6502_DF,
    OPC_6502_E0,
    OPC_6502_E1,
    OPC_6502_E2,
    OPC_6502_E3,
    OPC_6502_E4,
    OPC_6502_E5,
    OPC_6502_E6,
    OPC_6502_E7,
    OPC_6502_E8,
    OPC_6502_E9,
    OPC_6502_EA,
    OPC_6502_EB,
    OPC_6502_EC,
    OPC_6502_ED,
    OPC_6502_EE,
    OPC_6502_EF,
    OPC_6502_F0,
    OPC_6502_F1,
    OPC_6502_F2,
    OPC_6502_F3,
    OPC_6502_F4,
    OPC_6502_F5,
    OPC_6502_F6,
    OPC_6502_F7,
    OPC_6502_F8,
    OPC_6502_F9,
    OPC_6502_FA,
    OPC_6502_FB,
    OPC_6502_FC,
    OPC_6502_FD,
    OPC_6502_FE,
    OPC_6502_FF,
};



/*****************************************************************************/
/*				     Code				     */
/*****************************************************************************/



void RunCPU (void)
/* Run the CPU */
{
    while (!CPUHalted) {

	/* Get the next opcode */
	unsigned char B = 0x00;

	/* Execute it */
	OPCTable[B] ();

        /* Count cycles */
        TotalCycles += Cycles;
    }
}



