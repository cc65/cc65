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
#include "attrib.h"
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



/* Return the flags as a boolean value (0/1) */
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
#define TEST_ZF(v)              SET_ZF (((v) & 0xFF) == 0)
#define TEST_SF(v)              SET_SF (((v) & 0x80) != 0)
#define TEST_CF(v)              SET_CF (((v) & 0xFF00) != 0)

/* Program counter halves */
#define PCL		(PC & 0xFF)
#define PCH		((PC >> 8) & 0xFF)

/* Stack operations */
#define PUSH(Val)       MemWriteByte (StackPage + SP--, Val)
#define POP()           MemReadByte (StackPage + ++SP)

/* Test for page cross */
#define PAGE_CROSS(addr,offs)   ((((addr) & 0xFF) + offs) >= 0x100)

/* #imm */
#define AC_OP_IMM(op)                           \
    Cycles = 2;                                 \
    AC = AC op MemReadByte (PC+1);              \
    TEST_ZF (AC);                               \
    TEST_SF (AC);                               \
    PC += 2

/* zp */
#define AC_OP_ZP(op)                            \
    Cycles = 3;                                 \
    AC = AC op MemReadByte (MemReadByte (PC+1));\
    TEST_ZF (AC);                               \
    TEST_SF (AC);                               \
    PC += 2

/* zp,x */
#define AC_OP_ZPX(op)                           \
    unsigned char ZPAddr;                       \
    Cycles = 4;                                 \
    ZPAddr = MemReadByte (PC+1) + XR;           \
    AC = AC op MemReadByte (ZPAddr);            \
    TEST_ZF (AC);                               \
    TEST_SF (AC);                               \
    PC += 2

/* zp,y */
#define AC_OP_ZPY(op)                           \
    unsigned char ZPAddr;                       \
    Cycles = 4;                                 \
    ZPAddr = MemReadByte (PC+1) + YR;           \
    AC = AC op MemReadByte (ZPAddr);            \
    TEST_ZF (AC);                               \
    TEST_SF (AC);                               \
    PC += 2

/* abs */
#define AC_OP_ABS(op)                           \
    unsigned Addr;                              \
    Cycles = 4;                                 \
    Addr = MemReadWord (PC+1);                  \
    AC = AC op MemReadByte (Addr);              \
    TEST_ZF (AC);                               \
    TEST_SF (AC);                               \
    PC += 3

/* abs,x */
#define AC_OP_ABSX(op)                          \
    unsigned Addr;                              \
    Cycles = 4;                                 \
    Addr = MemReadWord (PC+1);                  \
    if (PAGE_CROSS (Addr, XR)) {                \
        ++Cycles;                               \
    }                                           \
    AC = AC | MemReadByte (Addr + XR);          \
    TEST_ZF (AC);                               \
    TEST_SF (AC);                               \
    PC += 3

/* abs,y */
#define AC_OP_ABSY(op)                          \
    unsigned Addr;                              \
    Cycles = 4;                                 \
    Addr = MemReadWord (PC+1);                  \
    if (PAGE_CROSS (Addr, YR)) {                \
        ++Cycles;                               \
    }                                           \
    AC = AC | MemReadByte (Addr + YR);          \
    TEST_ZF (AC);                               \
    TEST_SF (AC);                               \
    PC += 3

/* (zp,x) */
#define AC_OP_ZPXIND(op)                        \
    unsigned char ZPAddr;                       \
    unsigned Addr;                              \
    Cycles = 6;                                 \
    ZPAddr = MemReadByte (PC+1) + XR;           \
    Addr = MemReadZPWord (ZPAddr);              \
    AC = AC op MemReadByte (Addr);              \
    TEST_ZF (AC);                               \
    TEST_SF (AC);                               \
    PC += 2

/* (zp),y */
#define AC_OP_ZPINDY(op)                        \
    unsigned char ZPAddr;                       \
    unsigned Addr;                              \
    Cycles = 5;                                 \
    ZPAddr = MemReadByte (PC+1);                \
    Addr = MemReadZPWord (ZPAddr) + YR;         \
    AC = AC op MemReadByte (Addr);              \
    TEST_ZF (AC);                               \
    TEST_SF (AC);                               \
    PC += 2

/* ADC */
#define ADC(v)                                  \
    if (GET_DF ()) {                            \
        NotImplemented ();                      \
    } else {                                    \
        unsigned Val;                           \
        unsigned char rhs = v;                  \
        Val = AC + rhs + GET_CF ();             \
        AC = (unsigned char) Val;               \
        TEST_ZF (AC);                           \
        TEST_SF (AC);                           \
        TEST_CF (Val);                          \
        SET_OF (!((AC ^ rhs) & 0x80) &&         \
                ((AC ^ Val) & 0x80));           \
    }

/* branches */
#define BRANCH(cond)                            \
    Cycles = 2;                                 \
    if (cond) {                                 \
        signed char Offs;                       \
        unsigned char OldPCH;                   \
        ++Cycles;                               \
        Offs = (signed char) MemReadByte (PC+1);\
        OldPCH = PCH;                           \
        PC += 2 + (int) Offs;                   \
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


/* ROL */
#define ROL(Val)                                \
    Val <<= 1;                                  \
    if (GET_CF ()) {                            \
        Val |= 0x01;                            \
    }                                           \
    TEST_ZF (Val);                              \
    TEST_SF (Val);                              \
    TEST_CF (Val)

/* ROR */
#define ROR(Val)                                \
    if (GET_CF ()) {                            \
        Val |= 0x100;                           \
    }                                           \
    SET_CF (Val & 0x01);                        \
    Val >>= 1;                                  \
    TEST_ZF (Val);                              \
    TEST_SF (Val)

/* SBC */
#define SBC(v)                                  \
    if (GET_DF ()) {                            \
        NotImplemented ();                      \
    } else {                                    \
        unsigned Val;                           \
        unsigned char rhs = v;                  \
        Val = AC - rhs - (!GET_CF ());          \
        AC = (unsigned char) Val;               \
        TEST_ZF (AC);                           \
        TEST_SF (AC);                           \
        SET_CF (Val <= 0xFF);                   \
        SET_OF (((AC^rhs) & (AC^Val) & 0x80));  \
    }



/*****************************************************************************/
/*                               Helper functions                            */
/*****************************************************************************/



static void OPC_Illegal (void) attribute ((noreturn));
static void OPC_Illegal (void)
{
    fprintf (stderr, "Illegal: $%02X\n", MemReadByte (PC));
    exit (EXIT_FAILURE);
}



static void NotImplemented (void) attribute ((noreturn));
static void NotImplemented (void)
{
    fprintf (stderr, "Not implemented: $%02X\n", MemReadByte (PC));
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
    PC = MemReadWord (0xFFFE);
}



static void OPC_6502_01 (void)
/* Opcode $01: ORA (ind,x) */
{
    AC_OP_ZPXIND (|);
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
    ZPAddr = MemReadByte (PC+1);
    Val    = MemReadByte (ZPAddr) << 1;
    MemWriteByte (ZPAddr, (unsigned char) Val);
    TEST_ZF (Val & 0xFF);
    TEST_SF (Val);
    SET_CF (Val & 0x100);
    PC += 2;
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
    Addr = MemReadWord (PC+1);
    Val  = MemReadByte (Addr) << 1;
    MemWriteByte (Addr, (unsigned char) Val);
    TEST_ZF (Val & 0xFF);
    TEST_SF (Val);
    SET_CF (Val & 0x100);
    PC += 3;
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
    ZPAddr = MemReadByte (PC+1) + XR;
    Val    = MemReadByte (ZPAddr) << 1;
    MemWriteByte (ZPAddr, (unsigned char) Val);
    TEST_ZF (Val & 0xFF);
    TEST_SF (Val);
    SET_CF (Val & 0x100);
    PC += 2;
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
    Addr = MemReadWord (PC+1) + XR;
    Val  = MemReadByte (Addr) << 1;
    MemWriteByte (Addr, (unsigned char) Val);
    TEST_ZF (Val & 0xFF);
    TEST_SF (Val);
    SET_CF (Val & 0x100);
    PC += 3;
}



static void OPC_6502_20 (void)
/* Opcode $20: JSR */
{
    unsigned Addr;
    Cycles = 6;
    Addr   = MemReadWord (PC+1);
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



static void OPC_6502_24 (void)
/* Opcode $24: BIT zp */
{
    unsigned char ZPAddr;
    unsigned char Val;
    Cycles = 3;
    ZPAddr = MemReadByte (PC+1);
    Val    = MemReadByte (ZPAddr);
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
    ZPAddr = MemReadByte (PC+1);
    Val    = MemReadByte (ZPAddr);
    ROL (Val);
    MemWriteByte (ZPAddr, Val);
    PC += 2;
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
    Val    = AC;
    ROL (Val);
    AC = (unsigned char) Val;
    PC += 1;
}



static void OPC_6502_2C (void)
/* Opcode $2C: BIT abs */
{
    unsigned Addr;
    unsigned char Val;
    Cycles = 4;
    Addr = MemReadByte (PC+1);
    Val  = MemReadByte (Addr);
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
    Addr = MemReadWord (PC+1);
    Val  = MemReadByte (Addr);
    ROL (Val);
    MemWriteByte (Addr, Val);
    PC += 3;
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
    ZPAddr = MemReadByte (PC+1) + XR;
    Val    = MemReadByte (ZPAddr);
    ROL (Val);
    MemWriteByte (ZPAddr, Val);
    PC += 2;
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
    Addr = MemReadWord (PC+1) + XR;
    Val  = MemReadByte (Addr);
    ROL (Val);
    MemWriteByte (Addr, Val);
    PC += 2;
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
    ZPAddr = MemReadByte (PC+1);
    Val    = MemReadByte (ZPAddr);
    SET_CF (Val & 0x01);
    Val >>= 1;
    MemWriteByte (ZPAddr, Val);
    TEST_ZF (Val);
    TEST_SF (Val);
    PC += 2;
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



static void OPC_6502_4C (void)
/* Opcode $4C: JMP abs */
{
    Cycles = 3;
    PC = MemReadWord (PC+1);
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
    Addr = MemReadWord (PC+1);
    Val  = MemReadByte (Addr);
    SET_CF (Val & 0x01);
    Val >>= 1;
    MemWriteByte (Addr, Val);
    TEST_ZF (Val);
    TEST_SF (Val);
    PC += 3;
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
    ZPAddr = MemReadByte (PC+1) + XR;
    Val    = MemReadByte (ZPAddr);
    SET_CF (Val & 0x01);
    Val >>= 1;
    MemWriteByte (ZPAddr, Val);
    TEST_ZF (Val);
    TEST_SF (Val);
    PC += 2;
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
    Addr = MemReadWord (PC+1) + XR;
    Val  = MemReadByte (Addr);
    SET_CF (Val & 0x01);
    Val >>= 1;
    MemWriteByte (Addr, Val);
    TEST_ZF (Val);
    TEST_SF (Val);
    PC += 3;
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
    unsigned char ZPAddr;
    unsigned Addr;
    Cycles = 6;
    ZPAddr = MemReadByte (PC+1) + XR;
    Addr   = MemReadZPWord (ZPAddr);
    ADC (MemReadByte (Addr));
    PC += 2;
}



static void OPC_6502_65 (void)
/* Opcode $65: ADC zp */
{
    unsigned char ZPAddr;
    Cycles = 3;
    ZPAddr = MemReadByte (PC+1);
    ADC (MemReadByte (ZPAddr));
    PC += 2;
}



static void OPC_6502_66 (void)
/* Opcode $66: ROR zp */
{
    unsigned char ZPAddr;
    unsigned Val;
    Cycles = 5;
    ZPAddr = MemReadByte (PC+1);
    Val    = MemReadByte (ZPAddr);
    ROR (Val);
    MemWriteByte (ZPAddr, Val);
    PC += 2;
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
    Cycles = 2;
    ADC (MemReadByte (PC+1));
    PC += 2;
}



static void OPC_6502_6A (void)
/* Opcode $6A: ROR a */
{
    unsigned Val;
    Cycles = 2;
    Val = AC;
    ROR (Val);
    AC = (unsigned char) Val;
    PC += 1;
}



static void OPC_6502_6C (void)
/* Opcode $6C: JMP (ind) */
{
    unsigned Addr;
    Cycles = 5;
    Addr = MemReadWord (PC+1);
    if (CPU == CPU_6502) {
        /* Emulate the 6502 bug */
        PC = MemReadByte (Addr);
        Addr = (Addr & 0xFF00) | ((Addr + 1) & 0xFF);
        PC |= (MemReadByte (Addr) << 8);
    } else {
        /* 65C02 and above have this bug fixed */
        PC = MemReadWord (Addr);
    }
    PC += 3;
}



static void OPC_6502_6D (void)
/* Opcode $6D: ADC abs */
{
    unsigned Addr;
    Cycles = 4;
    Addr   = MemReadWord (PC+1);
    ADC (MemReadByte (Addr));
    PC += 3;
}



static void OPC_6502_6E (void)
/* Opcode $6E: ROR abs */
{
    unsigned Addr;
    unsigned Val;
    Cycles = 6;
    Addr = MemReadWord (PC+1);
    Val  = MemReadByte (Addr);
    ROR (Val);
    MemWriteByte (Addr, Val);
    PC += 3;
}



static void OPC_6502_70 (void)
/* Opcode $70: BVS */
{
    BRANCH (GET_OF ());
}



static void OPC_6502_71 (void)
/* Opcode $71: ADC (zp),y */
{
    unsigned char ZPAddr;
    unsigned Addr;
    Cycles = 5;
    ZPAddr = MemReadByte (PC+1);
    Addr   = MemReadZPWord (ZPAddr);
    if (PAGE_CROSS (Addr, YR)) {
        ++Cycles;
    }
    ADC (MemReadByte (Addr + YR));
    PC += 2;
}



static void OPC_6502_75 (void)
/* Opcode $75: ADC zp,x */
{
    unsigned char ZPAddr;
    Cycles = 4;
    ZPAddr = MemReadByte (PC+1) + XR;
    ADC (MemReadByte (ZPAddr));
    PC += 2;
}



static void OPC_6502_76 (void)
/* Opcode $76: ROR zp,x */
{
    unsigned char ZPAddr;
    unsigned Val;
    Cycles = 6;
    ZPAddr = MemReadByte (PC+1) + XR;
    Val    = MemReadByte (ZPAddr);
    ROR (Val);
    MemWriteByte (ZPAddr, Val);
    PC += 2;
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
    unsigned Addr;
    Cycles = 4;
    Addr   = MemReadWord (PC+1);
    if (PAGE_CROSS (Addr, YR)) {
        ++Cycles;
    }
    ADC (MemReadByte (Addr + YR));
    PC += 3;
}



static void OPC_6502_7D (void)
/* Opcode $7D: ADC abs,x */
{
    unsigned Addr;
    Cycles = 4;
    Addr   = MemReadWord (PC+1);
    if (PAGE_CROSS (Addr, XR)) {
        ++Cycles;
    }
    ADC (MemReadByte (Addr + XR));
    PC += 3;
}



static void OPC_6502_7E (void)
/* Opcode $7E: ROR abs,x */
{
    unsigned Addr;
    unsigned Val;
    Cycles = 7;
    Addr = MemReadByte (PC+1) + XR;
    Val  = MemReadByte (Addr);
    ROR (Val);
    MemWriteByte (Addr, Val);
    PC += 3;
}



static void OPC_6502_81 (void)
/* Opcode $81: STA (zp,x) */
{
    unsigned char ZPAddr;
    unsigned Addr;
    Cycles = 6;
    ZPAddr = MemReadByte (PC+1) + XR;
    Addr   = MemReadZPWord (ZPAddr);
    MemWriteByte (Addr, AC);
    PC += 2;
}



static void OPC_6502_84 (void)
/* Opcode $84: STY zp */
{
    unsigned char ZPAddr;
    Cycles = 3;
    ZPAddr = MemReadByte (PC+1);
    MemWriteByte (ZPAddr, YR);
    PC += 2;
}



static void OPC_6502_85 (void)
/* Opcode $85: STA zp */
{
    unsigned char ZPAddr;
    Cycles = 3;
    ZPAddr = MemReadByte (PC+1);
    MemWriteByte (ZPAddr, AC);
    PC += 2;
}



static void OPC_6502_86 (void)
/* Opcode $86: STX zp */
{
    unsigned char ZPAddr;
    Cycles = 3;
    ZPAddr = MemReadByte (PC+1);
    MemWriteByte (ZPAddr, XR);
    PC += 2;
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



static void OPC_6502_8A (void)
/* Opcode $8A: TXA */
{
    Cycles = 2;
    AC = XR;
    TEST_ZF (AC);
    TEST_SF (AC);
    PC += 1;
}



static void OPC_6502_8C (void)
/* Opcode $8C: STY abs */
{
    unsigned Addr;
    Cycles = 4;
    Addr = MemReadWord (PC+1);
    MemWriteByte (Addr, YR);
    PC += 3;
}



static void OPC_6502_8D (void)
/* Opcode $8D: STA abs */
{
    unsigned Addr;
    Cycles = 4;
    Addr = MemReadWord (PC+1);
    MemWriteByte (Addr, AC);
    PC += 3;
}



static void OPC_6502_8E (void)
/* Opcode $8E: STX abs */
{
    unsigned Addr;
    Cycles = 4;
    Addr = MemReadWord (PC+1);
    MemWriteByte (Addr, XR);
    PC += 3;
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
    ZPAddr = MemReadByte (PC+1);
    Addr   = MemReadZPWord (ZPAddr) + YR;
    MemWriteByte (Addr, AC);
    PC += 2;
}



static void OPC_6502_94 (void)
/* Opcode $94: STY zp,x */
{
    unsigned char ZPAddr;
    Cycles = 4;
    ZPAddr = MemReadByte (PC+1) + XR;
    MemWriteByte (ZPAddr, YR);
    PC += 2;
}



static void OPC_6502_95 (void)
/* Opcode $95: STA zp,x */
{
    unsigned char ZPAddr;
    Cycles = 4;
    ZPAddr = MemReadByte (PC+1) + XR;
    MemWriteByte (ZPAddr, AC);
    PC += 2;
}



static void OPC_6502_96 (void)
/* Opcode $96: stx zp,y */
{
    unsigned char ZPAddr;
    Cycles = 4;
    ZPAddr = MemReadByte (PC+1) + YR;
    MemWriteByte (ZPAddr, XR);
    PC += 2;
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
    Addr   = MemReadWord (PC+1) + YR;
    MemWriteByte (Addr, AC);
    PC += 3;
}



static void OPC_6502_9A (void)
/* Opcode $9A: TXS */
{
    Cycles = 2;
    SP = XR;
    PC += 1;
}



static void OPC_6502_9D (void)
/* Opcode $9D: STA abs,x */
{
    unsigned Addr;
    Cycles = 5;
    Addr   = MemReadWord (PC+1) + XR;
    MemWriteByte (Addr, AC);
    PC += 3;
}



static void OPC_6502_A0 (void)
/* Opcode $A0: LDY #imm */
{
    Cycles = 2;
    YR = MemReadByte (PC+1);
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
    ZPAddr = MemReadByte (PC+1) + XR;
    Addr = MemReadZPWord (ZPAddr);
    AC = MemReadByte (Addr);
    TEST_ZF (AC);
    TEST_SF (AC);
    PC += 2;
}



static void OPC_6502_A2 (void)
/* Opcode $A2: LDX #imm */
{
    Cycles = 2;
    XR = MemReadByte (PC+1);
    TEST_ZF (XR);
    TEST_SF (XR);
    PC += 2;
}



static void OPC_6502_A4 (void)
/* Opcode $A4: LDY zp */
{
    unsigned char ZPAddr;
    Cycles = 3;
    ZPAddr = MemReadByte (PC+1);
    YR = MemReadByte (ZPAddr);
    TEST_ZF (YR);
    TEST_SF (YR);
    PC += 2;
}



static void OPC_6502_A5 (void)
/* Opcode $A5: LDA zp */
{
    unsigned char ZPAddr;
    Cycles = 3;
    ZPAddr = MemReadByte (PC+1);
    AC = MemReadByte (ZPAddr);
    TEST_ZF (AC);
    TEST_SF (AC);
    PC += 2;
}



static void OPC_6502_A6 (void)
/* Opcode $A6: LDX zp */
{
    unsigned char ZPAddr;
    Cycles = 3;
    ZPAddr = MemReadByte (PC+1);
    XR = MemReadByte (ZPAddr);
    TEST_ZF (XR);
    TEST_SF (XR);
    PC += 2;
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
    AC = MemReadByte (PC+1);
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



static void OPC_6502_AC (void)
/* Opcode $AC: LDY abs */
{
    unsigned Addr;
    Cycles = 4;
    Addr   = MemReadWord (PC+1);
    YR     = MemReadByte (Addr);
    TEST_ZF (YR);
    TEST_SF (YR);
    PC += 3;
}



static void OPC_6502_AD (void)
/* Opcode $AD: LDA abs */
{
    unsigned Addr;
    Cycles = 4;
    Addr   = MemReadWord (PC+1);
    AC     = MemReadByte (Addr);
    TEST_ZF (AC);
    TEST_SF (AC);
    PC += 3;
}



static void OPC_6502_AE (void)
/* Opcode $AE: LDX abs */
{
    unsigned Addr;
    Cycles = 4;
    Addr   = MemReadWord (PC+1);
    XR     = MemReadByte (Addr);
    TEST_ZF (XR);
    TEST_SF (XR);
    PC += 3;
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
    ZPAddr = MemReadByte (PC+1);
    Addr   = MemReadZPWord (ZPAddr);
    if (PAGE_CROSS (Addr, YR)) {
        ++Cycles;
    }
    AC = MemReadByte (Addr + YR);
    TEST_ZF (AC);
    TEST_SF (AC);
    PC += 2;
}



static void OPC_6502_B4 (void)
/* Opcode $B4: LDY zp,x */
{
    unsigned char ZPAddr;
    Cycles = 4;
    ZPAddr = MemReadByte (PC+1) + XR;
    YR     = MemReadByte (ZPAddr);
    TEST_ZF (YR);
    TEST_SF (YR);
    PC += 2;
}



static void OPC_6502_B5 (void)
/* Opcode $B5: LDA zp,x */
{
    unsigned char ZPAddr;
    Cycles = 4;
    ZPAddr = MemReadByte (PC+1) + XR;
    AC     = MemReadByte (ZPAddr);
    TEST_ZF (AC);
    TEST_SF (AC);
    PC += 2;
}



static void OPC_6502_B6 (void)
/* Opcode $B6: LDX zp,y */
{
    unsigned char ZPAddr;
    Cycles = 4;
    ZPAddr = MemReadByte (PC+1) + YR;
    XR     = MemReadByte (ZPAddr);
    TEST_ZF (XR);
    TEST_SF (XR);
    PC += 2;
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
    Addr = MemReadWord (PC+1);
    if (PAGE_CROSS (Addr, YR)) {
        ++Cycles;
    }
    AC = MemReadByte (Addr + YR);
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



static void OPC_6502_BC (void)
/* Opcode $BC: LDY abs,x */
{
    unsigned Addr;
    Cycles = 4;
    Addr = MemReadWord (PC+1);
    if (PAGE_CROSS (Addr, XR)) {
        ++Cycles;
    }
    YR = MemReadByte (Addr + XR);
    TEST_ZF (YR);
    TEST_SF (YR);
    PC += 3;
}



static void OPC_6502_BD (void)
/* Opcode $BD: LDA abs,x */
{
    unsigned Addr;
    Cycles = 4;
    Addr = MemReadWord (PC+1);
    if (PAGE_CROSS (Addr, XR)) {
        ++Cycles;
    }
    AC = MemReadByte (Addr + XR);
    TEST_ZF (AC);
    TEST_SF (AC);
    PC += 3;
}



static void OPC_6502_BE (void)
/* Opcode $BE: LDX abs,y */
{
    unsigned Addr;
    Cycles = 4;
    Addr = MemReadWord (PC+1);
    if (PAGE_CROSS (Addr, YR)) {
        ++Cycles;
    }
    XR = MemReadByte (Addr + YR);
    TEST_ZF (XR);
    TEST_SF (XR);
    PC += 3;
}



static void OPC_6502_C0 (void)
/* Opcode $C0: CPY #imm */
{
    Cycles = 2;
    CMP (YR, MemReadByte (PC+1));
    PC += 2;
}



static void OPC_6502_C1 (void)
/* Opcode $C1: CMP (zp,x) */
{
    unsigned char ZPAddr;
    unsigned Addr;
    Cycles = 6;
    ZPAddr = MemReadByte (PC+1) + XR;
    Addr   = MemReadZPWord (ZPAddr);
    CMP (AC, MemReadByte (Addr));
    PC += 2;
}



static void OPC_6502_C4 (void)
/* Opcode $C4: CPY zp */
{
    unsigned char ZPAddr;
    Cycles = 3;
    ZPAddr = MemReadByte (PC+1);
    CMP (YR, MemReadByte (ZPAddr));
    PC += 2;
}



static void OPC_6502_C5 (void)
/* Opcode $C5: CMP zp */
{
    unsigned char ZPAddr;
    Cycles = 3;
    ZPAddr = MemReadByte (PC+1);
    CMP (AC, MemReadByte (ZPAddr));
    PC += 2;
}



static void OPC_6502_C6 (void)
/* Opcode $C6: DEC zp */
{
    unsigned char ZPAddr;
    unsigned char Val;
    Cycles = 5;
    ZPAddr = MemReadByte (PC+1);
    Val    = MemReadByte (ZPAddr) - 1;
    MemWriteByte (ZPAddr, Val);
    TEST_ZF (Val);
    TEST_SF (Val);
    PC += 2;
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
    CMP (AC, MemReadByte (PC+1));
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



static void OPC_6502_CC (void)
/* Opcode $CC: CPY abs */
{
    unsigned Addr;
    Cycles = 4;
    Addr   = MemReadWord (PC+1);
    CMP (YR, MemReadByte (Addr));
    PC += 3;
}



static void OPC_6502_CD (void)
/* Opcode $CD: CMP abs */
{
    unsigned Addr;
    Cycles = 4;
    Addr   = MemReadWord (PC+1);
    CMP (AC, MemReadByte (Addr));
    PC += 3;
}



static void OPC_6502_CE (void)
/* Opcode $CE: DEC abs */
{
    unsigned Addr;
    unsigned char Val;
    Cycles = 6;
    Addr = MemReadWord (PC+1);
    Val  = MemReadByte (Addr) - 1;
    MemWriteByte (Addr, Val);
    TEST_ZF (Val);
    TEST_SF (Val);
    PC += 3;
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
    ZPAddr = MemReadByte (PC+1);
    Addr   = MemReadWord (ZPAddr);
    if (PAGE_CROSS (Addr, YR)) {
        ++Cycles;
    }
    CMP (AC, MemReadByte (Addr + YR));
    PC += 2;
}



static void OPC_6502_D5 (void)
/* Opcode $D5: CMP zp,x */
{
    unsigned char ZPAddr;
    Cycles = 4;
    ZPAddr = MemReadByte (PC+1) + XR;
    CMP (AC, MemReadByte (ZPAddr));
    PC += 2;
}



static void OPC_6502_D6 (void)
/* Opcode $D6: DEC zp,x */
{
    unsigned char ZPAddr;
    unsigned char Val;
    Cycles = 6;
    ZPAddr = MemReadByte (PC+1) + XR;
    Val  = MemReadByte (ZPAddr) - 1;
    MemWriteByte (ZPAddr, Val);
    TEST_ZF (Val);
    TEST_SF (Val);
    PC += 2;
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
    Addr = MemReadWord (PC+1);
    if (PAGE_CROSS (Addr, YR)) {
        ++Cycles;
    }
    CMP (AC, MemReadByte (Addr + YR));
    PC += 3;
}



static void OPC_6502_DD (void)
/* Opcode $DD: CMP abs,x */
{
    unsigned Addr;
    Cycles = 4;
    Addr = MemReadWord (PC+1);
    if (PAGE_CROSS (Addr, XR)) {
        ++Cycles;
    }
    CMP (AC, MemReadByte (Addr + XR));
    PC += 3;
}



static void OPC_6502_DE (void)
/* Opcode $DE: DEC abs,x */
{
    unsigned Addr;
    unsigned char Val;
    Cycles = 7;
    Addr = MemReadWord (PC+1) + XR;
    Val  = MemReadByte (Addr) - 1;
    MemWriteByte (Addr, Val);
    TEST_ZF (Val);
    TEST_SF (Val);
    PC += 3;
}



static void OPC_6502_E0 (void)
/* Opcode $E0: CPX #imm */
{
    Cycles = 2;
    CMP (XR, MemReadByte (PC+1));
    PC += 2;
}



static void OPC_6502_E1 (void)
/* Opcode $E1: SBC (zp,x) */
{
    unsigned char ZPAddr;
    unsigned Addr;
    Cycles = 6;
    ZPAddr = MemReadByte (PC+1) + XR;
    Addr   = MemReadZPWord (ZPAddr);
    SBC (MemReadByte (Addr));
    PC += 2;
}



static void OPC_6502_E4 (void)
/* Opcode $E4: CPX zp */
{
    unsigned char ZPAddr;
    Cycles = 3;
    ZPAddr = MemReadByte (PC+1);
    CMP (XR, MemReadByte (ZPAddr));
    PC += 2;
}



static void OPC_6502_E5 (void)
/* Opcode $E5: SBC zp */
{
    unsigned char ZPAddr;
    Cycles = 3;
    ZPAddr = MemReadByte (PC+1);
    SBC (MemReadByte (ZPAddr));
    PC += 2;
}



static void OPC_6502_E6 (void)
/* Opcode $E6: INC zp */
{
    unsigned char ZPAddr;
    unsigned char Val;
    Cycles = 5;
    ZPAddr = MemReadByte (PC+1);
    Val    = MemReadByte (ZPAddr) + 1;
    MemWriteByte (ZPAddr, Val);
    TEST_ZF (Val);
    TEST_SF (Val);
    PC += 2;
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
    Cycles = 2;
    SBC (MemReadByte (PC+1));
    PC += 2;
}



static void OPC_6502_EA (void)
/* Opcode $EA: NOP */
{
    /* This one is easy... */
    Cycles = 2;
    PC += 1;
}



static void OPC_6502_EC (void)
/* Opcode $EC: CPX abs */
{
    unsigned Addr;
    Cycles = 4;
    Addr   = MemReadWord (PC+1);
    CMP (XR, MemReadByte (Addr));
    PC += 3;
}



static void OPC_6502_ED (void)
/* Opcode $ED: SBC abs */
{
    unsigned Addr;
    Cycles = 4;
    Addr   = MemReadWord (PC+1);
    SBC (MemReadByte (Addr));
    PC += 3;
}



static void OPC_6502_EE (void)
/* Opcode $EE: INC abs */
{
    unsigned Addr;
    unsigned char Val;
    Cycles = 6;
    Addr = MemReadWord (PC+1);
    Val  = MemReadByte (Addr) + 1;
    MemWriteByte (Addr, Val);
    TEST_ZF (Val);
    TEST_SF (Val);
    PC += 3;
}



static void OPC_6502_F0 (void)
/* Opcode $F0: BEQ */
{
    BRANCH (GET_ZF ());
}



static void OPC_6502_F1 (void)
/* Opcode $F1: SBC (zp),y */
{
    unsigned char ZPAddr;
    unsigned Addr;
    Cycles = 5;
    ZPAddr = MemReadByte (PC+1);
    Addr   = MemReadZPWord (ZPAddr);
    if (PAGE_CROSS (Addr, YR)) {
        ++Cycles;
    }
    SBC (MemReadByte (Addr + YR));
    PC += 2;
}



static void OPC_6502_F5 (void)
/* Opcode $F5: SBC zp,x */
{
    unsigned char ZPAddr;
    Cycles = 4;
    ZPAddr = MemReadByte (PC+1) + XR;
    SBC (MemReadByte (ZPAddr));
    PC += 2;
}



static void OPC_6502_F6 (void)
/* Opcode $F6: INC zp,x */
{
    unsigned char ZPAddr;
    unsigned char Val;
    Cycles = 6;
    ZPAddr = MemReadByte (PC+1) + XR;
    Val  = MemReadByte (ZPAddr) + 1;
    MemWriteByte (ZPAddr, Val);
    TEST_ZF (Val);
    TEST_SF (Val);
    PC += 2;
}



static void OPC_6502_F8 (void)
/* Opcode $F8: SED */
{
    SET_DF (1);
}



static void OPC_6502_F9 (void)
/* Opcode $F9: SBC abs,y */
{
    unsigned Addr;
    Cycles = 4;
    Addr   = MemReadWord (PC+1);
    if (PAGE_CROSS (Addr, YR)) {
        ++Cycles;
    }
    SBC (MemReadByte (Addr + YR));
    PC += 3;
}



static void OPC_6502_FD (void)
/* Opcode $FD: SBC abs,x */
{
    unsigned Addr;
    Cycles = 4;
    Addr   = MemReadWord (PC+1);
    if (PAGE_CROSS (Addr, XR)) {
        ++Cycles;
    }
    SBC (MemReadByte (Addr + XR));
    PC += 3;
}



static void OPC_6502_FE (void)
/* Opcode $FE: INC abs,x */
{
    unsigned Addr;
    unsigned char Val;
    Cycles = 7;
    Addr = MemReadWord (PC+1) + XR;
    Val  = MemReadByte (Addr) + 1;
    MemWriteByte (Addr, Val);
    TEST_ZF (Val);
    TEST_SF (Val);
    PC += 3;
}



/*****************************************************************************/
/*				     Data				     */
/*****************************************************************************/



/* Opcode handler table */
typedef void (*OPCFunc) (void);
static OPCFunc OPCTable[256] = {
    OPC_6502_00,
    OPC_6502_01,
    OPC_Illegal,
    OPC_Illegal,
    OPC_Illegal,
    OPC_6502_05,
    OPC_6502_06,
    OPC_Illegal,
    OPC_6502_08,
    OPC_6502_09,
    OPC_6502_0A,
    OPC_Illegal,
    OPC_Illegal,
    OPC_6502_0D,
    OPC_6502_0E,
    OPC_Illegal,
    OPC_6502_10,
    OPC_6502_11,
    OPC_Illegal,
    OPC_Illegal,
    OPC_Illegal,
    OPC_6502_15,
    OPC_6502_16,
    OPC_Illegal,
    OPC_6502_18,
    OPC_6502_19,
    OPC_Illegal,
    OPC_Illegal,
    OPC_Illegal,
    OPC_6502_1D,
    OPC_6502_1E,
    OPC_Illegal,
    OPC_6502_20,
    OPC_6502_21,
    OPC_Illegal,
    OPC_Illegal,
    OPC_6502_24,
    OPC_6502_25,
    OPC_6502_26,
    OPC_Illegal,
    OPC_6502_28,
    OPC_6502_29,
    OPC_6502_2A,
    OPC_Illegal,
    OPC_6502_2C,
    OPC_6502_2D,
    OPC_6502_2E,
    OPC_Illegal,
    OPC_6502_30,
    OPC_6502_31,
    OPC_Illegal,
    OPC_Illegal,
    OPC_Illegal,
    OPC_6502_35,
    OPC_6502_36,
    OPC_Illegal,
    OPC_6502_38,
    OPC_6502_39,
    OPC_Illegal,
    OPC_Illegal,
    OPC_Illegal,
    OPC_6502_3D,
    OPC_6502_3E,
    OPC_Illegal,
    OPC_6502_40,
    OPC_6502_41,
    OPC_Illegal,
    OPC_Illegal,
    OPC_Illegal,
    OPC_6502_45,
    OPC_6502_46,
    OPC_Illegal,
    OPC_6502_48,
    OPC_6502_49,
    OPC_6502_4A,
    OPC_Illegal,
    OPC_6502_4C,
    OPC_6502_4D,
    OPC_6502_4E,
    OPC_Illegal,
    OPC_6502_50,
    OPC_6502_51,
    OPC_Illegal,
    OPC_Illegal,
    OPC_Illegal,
    OPC_6502_55,
    OPC_6502_56,
    OPC_Illegal,
    OPC_6502_58,
    OPC_6502_59,
    OPC_Illegal,
    OPC_Illegal,
    OPC_Illegal,
    OPC_6502_5D,
    OPC_6502_5E,
    OPC_Illegal,
    OPC_6502_60,
    OPC_6502_61,
    OPC_Illegal,
    OPC_Illegal,
    OPC_Illegal,
    OPC_6502_65,
    OPC_6502_66,
    OPC_Illegal,
    OPC_6502_68,
    OPC_6502_69,
    OPC_6502_6A,
    OPC_Illegal,
    OPC_6502_6C,
    OPC_6502_6D,
    OPC_6502_6E,
    OPC_Illegal,
    OPC_6502_70,
    OPC_6502_71,
    OPC_Illegal,
    OPC_Illegal,
    OPC_Illegal,
    OPC_6502_75,
    OPC_6502_76,
    OPC_Illegal,
    OPC_6502_78,
    OPC_6502_79,
    OPC_Illegal,
    OPC_Illegal,
    OPC_Illegal,
    OPC_6502_7D,
    OPC_6502_7E,
    OPC_Illegal,
    OPC_Illegal,
    OPC_6502_81,
    OPC_Illegal,
    OPC_Illegal,
    OPC_6502_84,
    OPC_6502_85,
    OPC_6502_86,
    OPC_Illegal,
    OPC_6502_88,
    OPC_Illegal,
    OPC_6502_8A,
    OPC_Illegal,
    OPC_6502_8C,
    OPC_6502_8D,
    OPC_6502_8E,
    OPC_Illegal,
    OPC_6502_90,
    OPC_6502_91,
    OPC_Illegal,
    OPC_Illegal,
    OPC_6502_94,
    OPC_6502_95,
    OPC_6502_96,
    OPC_Illegal,
    OPC_6502_98,
    OPC_6502_99,
    OPC_6502_9A,
    OPC_Illegal,
    OPC_Illegal,
    OPC_6502_9D,
    OPC_Illegal,
    OPC_Illegal,
    OPC_6502_A0,
    OPC_6502_A1,
    OPC_6502_A2,
    OPC_Illegal,
    OPC_6502_A4,
    OPC_6502_A5,
    OPC_6502_A6,
    OPC_Illegal,
    OPC_6502_A8,
    OPC_6502_A9,
    OPC_6502_AA,
    OPC_Illegal,
    OPC_6502_AC,
    OPC_6502_AD,
    OPC_6502_AE,
    OPC_Illegal,
    OPC_6502_B0,
    OPC_6502_B1,
    OPC_Illegal,
    OPC_Illegal,
    OPC_6502_B4,
    OPC_6502_B5,
    OPC_6502_B6,
    OPC_Illegal,
    OPC_6502_B8,
    OPC_6502_B9,
    OPC_6502_BA,
    OPC_Illegal,
    OPC_6502_BC,
    OPC_6502_BD,
    OPC_6502_BE,
    OPC_Illegal,
    OPC_6502_C0,
    OPC_6502_C1,
    OPC_Illegal,
    OPC_Illegal,
    OPC_6502_C4,
    OPC_6502_C5,
    OPC_6502_C6,
    OPC_Illegal,
    OPC_6502_C8,
    OPC_6502_C9,
    OPC_6502_CA,
    OPC_Illegal,
    OPC_6502_CC,
    OPC_6502_CD,
    OPC_6502_CE,
    OPC_Illegal,
    OPC_6502_D0,
    OPC_6502_D1,
    OPC_Illegal,
    OPC_Illegal,
    OPC_Illegal,
    OPC_6502_D5,
    OPC_6502_D6,
    OPC_Illegal,
    OPC_6502_D8,
    OPC_6502_D9,
    OPC_Illegal,
    OPC_Illegal,
    OPC_Illegal,
    OPC_6502_DD,
    OPC_6502_DE,
    OPC_Illegal,
    OPC_6502_E0,
    OPC_6502_E1,
    OPC_Illegal,
    OPC_Illegal,
    OPC_6502_E4,
    OPC_6502_E5,
    OPC_6502_E6,
    OPC_Illegal,
    OPC_6502_E8,
    OPC_6502_E9,
    OPC_6502_EA,
    OPC_Illegal,
    OPC_6502_EC,
    OPC_6502_ED,
    OPC_6502_EE,
    OPC_Illegal,
    OPC_6502_F0,
    OPC_6502_F1,
    OPC_Illegal,
    OPC_Illegal,
    OPC_Illegal,
    OPC_6502_F5,
    OPC_6502_F6,
    OPC_Illegal,
    OPC_6502_F8,
    OPC_6502_F9,
    OPC_Illegal,
    OPC_Illegal,
    OPC_Illegal,
    OPC_6502_FD,
    OPC_6502_FE,
    OPC_Illegal,
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



