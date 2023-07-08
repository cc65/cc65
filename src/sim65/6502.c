/*****************************************************************************/
/*                                                                           */
/*                                  6502.c                                   */
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
/* Mar-2017, Christian Krueger, added support for 65SC02                     */
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

/* Known bugs and limitations of the 65C02 simulation:
 * support currently only on the level of 65SC02:
   BBRx, BBSx, RMBx, SMBx, WAI, and STP are unsupported
 * BCD flag handling equals 6502 (unchecked if bug is simulated or wrong for
   6502)
*/

#include "memory.h"
#include "error.h"
#include "6502.h"
#include "paravirt.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Current CPU */
CPUType CPU;

/* Type of an opcode handler function */
typedef void (*OPFunc) (void);

/* The CPU registers */
static CPURegs Regs;

/* Cycles for the current insn */
static unsigned Cycles;

/* NMI request active */
static unsigned HaveNMIRequest;

/* IRQ request active */
static unsigned HaveIRQRequest;


/*****************************************************************************/
/*                        Helper functions and macros                        */
/*****************************************************************************/



/* Return the flags as boolean values (0/1) */
#define GET_CF()        ((Regs.SR & CF) != 0)
#define GET_ZF()        ((Regs.SR & ZF) != 0)
#define GET_IF()        ((Regs.SR & IF) != 0)
#define GET_DF()        ((Regs.SR & DF) != 0)
#define GET_OF()        ((Regs.SR & OF) != 0)
#define GET_SF()        ((Regs.SR & SF) != 0)

/* Set the flags. The parameter is a boolean flag that says if the flag should be
** set or reset.
*/
#define SET_CF(f)       do { if (f) { Regs.SR |= CF; } else { Regs.SR &= ~CF; } } while (0)
#define SET_ZF(f)       do { if (f) { Regs.SR |= ZF; } else { Regs.SR &= ~ZF; } } while (0)
#define SET_IF(f)       do { if (f) { Regs.SR |= IF; } else { Regs.SR &= ~IF; } } while (0)
#define SET_DF(f)       do { if (f) { Regs.SR |= DF; } else { Regs.SR &= ~DF; } } while (0)
#define SET_OF(f)       do { if (f) { Regs.SR |= OF; } else { Regs.SR &= ~OF; } } while (0)
#define SET_SF(f)       do { if (f) { Regs.SR |= SF; } else { Regs.SR &= ~SF; } } while (0)

/* Special test and set macros. The meaning of the parameter depends on the
** actual flag that should be set or reset.
*/
#define TEST_ZF(v)      SET_ZF (((v) & 0xFF) == 0)
#define TEST_SF(v)      SET_SF (((v) & 0x80) != 0)
#define TEST_CF(v)      SET_CF (((v) & 0xFF00) != 0)

/* Program counter halves */
#define PCL             (Regs.PC & 0xFF)
#define PCH             ((Regs.PC >> 8) & 0xFF)

/* Stack operations */
#define PUSH(Val)       MemWriteByte (0x0100 | (Regs.SP-- & 0xFF), Val)
#define POP()           MemReadByte (0x0100 | (++Regs.SP & 0xFF))

/* Test for page cross */
#define PAGE_CROSS(addr,offs)   ((((addr) & 0xFF) + offs) >= 0x100)

/* #imm */
#define AC_OP_IMM(op)                                           \
    Cycles = 2;                                                 \
    Regs.AC = Regs.AC op MemReadByte (Regs.PC+1);               \
    TEST_ZF (Regs.AC);                                          \
    TEST_SF (Regs.AC);                                          \
    Regs.PC += 2

/* zp */
#define AC_OP_ZP(op)                                            \
    Cycles = 3;                                                 \
    Regs.AC = Regs.AC op MemReadByte (MemReadByte (Regs.PC+1)); \
    TEST_ZF (Regs.AC);                                          \
    TEST_SF (Regs.AC);                                          \
    Regs.PC += 2

/* zp,x */
#define AC_OP_ZPX(op)                                           \
    unsigned char ZPAddr;                                       \
    Cycles = 4;                                                 \
    ZPAddr = MemReadByte (Regs.PC+1) + Regs.XR;                 \
    Regs.AC = Regs.AC op MemReadByte (ZPAddr);                  \
    TEST_ZF (Regs.AC);                                          \
    TEST_SF (Regs.AC);                                          \
    Regs.PC += 2

/* zp,y */
#define AC_OP_ZPY(op)                                           \
    unsigned char ZPAddr;                                       \
    Cycles = 4;                                                 \
    ZPAddr = MemReadByte (Regs.PC+1) + Regs.YR;                 \
    Regs.AC = Regs.AC op MemReadByte (ZPAddr);                  \
    TEST_ZF (Regs.AC);                                          \
    TEST_SF (Regs.AC);                                          \
    Regs.PC += 2

/* abs */
#define AC_OP_ABS(op)                                           \
    unsigned Addr;                                              \
    Cycles = 4;                                                 \
    Addr = MemReadWord (Regs.PC+1);                             \
    Regs.AC = Regs.AC op MemReadByte (Addr);                    \
    TEST_ZF (Regs.AC);                                          \
    TEST_SF (Regs.AC);                                          \
    Regs.PC += 3

/* abs,x */
#define AC_OP_ABSX(op)                                          \
    unsigned Addr;                                              \
    Cycles = 4;                                                 \
    Addr = MemReadWord (Regs.PC+1);                             \
    if (PAGE_CROSS (Addr, Regs.XR)) {                           \
        ++Cycles;                                               \
    }                                                           \
    Regs.AC = Regs.AC op MemReadByte (Addr + Regs.XR);          \
    TEST_ZF (Regs.AC);                                          \
    TEST_SF (Regs.AC);                                          \
    Regs.PC += 3

/* abs,y */
#define AC_OP_ABSY(op)                                          \
    unsigned Addr;                                              \
    Cycles = 4;                                                 \
    Addr = MemReadWord (Regs.PC+1);                             \
    if (PAGE_CROSS (Addr, Regs.YR)) {                           \
        ++Cycles;                                               \
    }                                                           \
    Regs.AC = Regs.AC op MemReadByte (Addr + Regs.YR);          \
    TEST_ZF (Regs.AC);                                          \
    TEST_SF (Regs.AC);                                          \
    Regs.PC += 3

/* (zp,x) */
#define AC_OP_ZPXIND(op)                                        \
    unsigned char ZPAddr;                                       \
    unsigned Addr;                                              \
    Cycles = 6;                                                 \
    ZPAddr = MemReadByte (Regs.PC+1) + Regs.XR;                 \
    Addr = MemReadZPWord (ZPAddr);                              \
    Regs.AC = Regs.AC op MemReadByte (Addr);                    \
    TEST_ZF (Regs.AC);                                          \
    TEST_SF (Regs.AC);                                          \
    Regs.PC += 2

/* (zp),y */
#define AC_OP_ZPINDY(op)                                        \
    unsigned char ZPAddr;                                       \
    unsigned Addr;                                              \
    Cycles = 5;                                                 \
    ZPAddr = MemReadByte (Regs.PC+1);                           \
    Addr = MemReadZPWord (ZPAddr);                              \
    if (PAGE_CROSS (Addr, Regs.YR)) {                           \
        ++Cycles;                                               \
    }                                                           \
    Addr += Regs.YR;                                            \
    Regs.AC = Regs.AC op MemReadByte (Addr);                    \
    TEST_ZF (Regs.AC);                                          \
    TEST_SF (Regs.AC);                                          \
    Regs.PC += 2

/* (zp) */
#define AC_OP_ZPIND(op)                                         \
    unsigned char ZPAddr;                                       \
    unsigned Addr;                                              \
    Cycles = 5;                                                 \
    ZPAddr = MemReadByte (Regs.PC+1);                           \
    Addr = MemReadZPWord (ZPAddr);                              \
    Regs.AC = Regs.AC op MemReadByte (Addr);                    \
    TEST_ZF (Regs.AC);                                          \
    TEST_SF (Regs.AC);                                          \
    Regs.PC += 2

/* ADC */
#define ADC(v)                                                  \
    do {                                                        \
        unsigned old = Regs.AC;                                 \
        unsigned rhs = (v & 0xFF);                              \
        if (GET_DF ()) {                                        \
            unsigned lo;                                        \
            int res;                                            \
            lo = (old & 0x0F) + (rhs & 0x0F) + GET_CF ();       \
            if (lo >= 0x0A) {                                   \
                lo = ((lo + 0x06) & 0x0F) + 0x10;               \
            }                                                   \
            Regs.AC = (old & 0xF0) + (rhs & 0xF0) + lo;         \
            res = (signed char)(old & 0xF0) +                   \
                  (signed char)(rhs & 0xF0) +                   \
                  (signed char)lo;                              \
            TEST_ZF (old + rhs + GET_CF ());                    \
            TEST_SF (Regs.AC);                                  \
            if (Regs.AC >= 0xA0) {                              \
                Regs.AC += 0x60;                                \
            }                                                   \
            TEST_CF (Regs.AC);                                  \
            SET_OF ((res < -128) || (res > 127));               \
            if (CPU != CPU_6502) {                              \
                ++Cycles;                                       \
            }                                                   \
        } else {                                                \
            Regs.AC += rhs + GET_CF ();                         \
            TEST_ZF (Regs.AC);                                  \
            TEST_SF (Regs.AC);                                  \
            TEST_CF (Regs.AC);                                  \
            SET_OF (!((old ^ rhs) & 0x80) &&                    \
                    ((old ^ Regs.AC) & 0x80));                  \
            Regs.AC &= 0xFF;                                    \
        }                                                       \
    } while (0)

/* branches */
#define BRANCH(cond)                                            \
    Cycles = 2;                                                 \
    if (cond) {                                                 \
        signed char Offs;                                       \
        unsigned char OldPCH;                                   \
        ++Cycles;                                               \
        Offs = (signed char) MemReadByte (Regs.PC+1);           \
        OldPCH = PCH;                                           \
        Regs.PC += 2 + (int) Offs;                              \
        if (PCH != OldPCH) {                                    \
            ++Cycles;                                           \
        }                                                       \
    } else {                                                    \
        Regs.PC += 2;                                           \
    }

/* compares */
#define CMP(v1, v2)                                             \
    do {                                                        \
        unsigned Result = v1 - v2;                              \
        TEST_ZF (Result & 0xFF);                                \
        TEST_SF (Result);                                       \
        SET_CF (Result <= 0xFF);                                \
    } while (0)


/* ROL */
#define ROL(Val)                                                \
    Val <<= 1;                                                  \
    if (GET_CF ()) {                                            \
        Val |= 0x01;                                            \
    }                                                           \
    TEST_ZF (Val);                                              \
    TEST_SF (Val);                                              \
    TEST_CF (Val)

/* ROR */
#define ROR(Val)                                                \
    if (GET_CF ()) {                                            \
        Val |= 0x100;                                           \
    }                                                           \
    SET_CF (Val & 0x01);                                        \
    Val >>= 1;                                                  \
    TEST_ZF (Val);                                              \
    TEST_SF (Val)

/* SBC */
#define SBC(v)                                                  \
    do {                                                        \
        unsigned old = Regs.AC;                                 \
        unsigned rhs = (v & 0xFF);                              \
        if (GET_DF ()) {                                        \
            unsigned lo;                                        \
            int res;                                            \
            lo = (old & 0x0F) - (rhs & 0x0F) + GET_CF () - 1;   \
            if (lo & 0x80) {                                    \
                lo = ((lo - 0x06) & 0x0F) - 0x10;               \
            }                                                   \
            Regs.AC = (old & 0xF0) - (rhs & 0xF0) + lo;         \
            if (Regs.AC & 0x80) {                               \
                Regs.AC -= 0x60;                                \
            }                                                   \
            res = Regs.AC - rhs + (!GET_CF ());                 \
            TEST_ZF (res);                                      \
            TEST_SF (res);                                      \
            SET_CF (res <= 0xFF);                               \
            SET_OF (((old^rhs) & (old^res) & 0x80));            \
            if (CPU != CPU_6502) {                              \
                ++Cycles;                                       \
            }                                                   \
        } else {                                                \
            Regs.AC -= rhs + (!GET_CF ());                      \
            TEST_ZF (Regs.AC);                                  \
            TEST_SF (Regs.AC);                                  \
            SET_CF (Regs.AC <= 0xFF);                           \
            SET_OF (((old^rhs) & (old^Regs.AC) & 0x80));        \
            Regs.AC &= 0xFF;                                    \
        }                                                       \
    } while (0)



/*****************************************************************************/
/*                         Opcode handling functions                         */
/*****************************************************************************/



static void OPC_Illegal (void)
{
    Error ("Illegal opcode $%02X at address $%04X",
           MemReadByte (Regs.PC), Regs.PC);
}



static void OPC_6502_00 (void)
/* Opcode $00: BRK */
{
    Cycles = 7;
    Regs.PC += 2;
    PUSH (PCH);
    PUSH (PCL);
    PUSH (Regs.SR);
    SET_IF (1);
    if (CPU != CPU_6502)
    {
        SET_DF (0);
    }
    Regs.PC = MemReadWord (0xFFFE);
}



static void OPC_6502_01 (void)
/* Opcode $01: ORA (ind,x) */
{
    AC_OP_ZPXIND (|);
}



static void OPC_65SC02_04 (void)
/* Opcode $04: TSB zp */
{
    unsigned char ZPAddr;
    unsigned char Val;
    Cycles = 5;
    ZPAddr = MemReadByte (Regs.PC+1);
    Val = MemReadByte (ZPAddr);
    SET_ZF ((Val & Regs.AC) == 0);
    MemWriteByte (ZPAddr, (unsigned char)(Val | Regs.AC));
    Regs.PC += 2;
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
    ZPAddr = MemReadByte (Regs.PC+1);
    Val = MemReadByte (ZPAddr) << 1;
    MemWriteByte (ZPAddr, (unsigned char) Val);
    TEST_ZF (Val & 0xFF);
    TEST_SF (Val);
    SET_CF (Val & 0x100);
    Regs.PC += 2;
}



static void OPC_6502_08 (void)
/* Opcode $08: PHP */
{
    Cycles = 3;
    PUSH (Regs.SR);
    Regs.PC += 1;
}



static void OPC_6502_09 (void)
/* Opcode $09: ORA #imm */
{
    AC_OP_IMM (|);
}



static void OPC_6502_0A (void)
/* Opcode $0A: ASL a */
{
    Cycles = 2;
    Regs.AC <<= 1;
    TEST_ZF (Regs.AC & 0xFF);
    TEST_SF (Regs.AC);
    SET_CF (Regs.AC & 0x100);
    Regs.AC &= 0xFF;
    Regs.PC += 1;
}



static void OPC_65SC02_0C (void)
/* Opcode $0C: TSB abs */
{
    unsigned Addr;
    unsigned char Val;
    Cycles = 6;
    Addr = MemReadWord (Regs.PC+1);
    Val = MemReadByte (Addr);
    SET_ZF ((Val & Regs.AC) == 0);
    MemWriteByte (Addr, (unsigned char) (Val | Regs.AC));
    Regs.PC += 3;
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
    Addr = MemReadWord (Regs.PC+1);
    Val = MemReadByte (Addr) << 1;
    MemWriteByte (Addr, (unsigned char) Val);
    TEST_ZF (Val & 0xFF);
    TEST_SF (Val);
    SET_CF (Val & 0x100);
    Regs.PC += 3;
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



static void OPC_65SC02_12 (void)
/* Opcode $12: ORA (zp) */
{
    AC_OP_ZPIND (|);
}



static void OPC_65SC02_14 (void)
/* Opcode $14: TRB zp */
{
    unsigned char ZPAddr;
    unsigned char Val;
    Cycles = 5;
    ZPAddr = MemReadByte (Regs.PC+1);
    Val = MemReadByte (ZPAddr);
    SET_ZF ((Val & Regs.AC) == 0);
    MemWriteByte (ZPAddr, (unsigned char)(Val & ~Regs.AC));
    Regs.PC += 2;
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
    ZPAddr = MemReadByte (Regs.PC+1) + Regs.XR;
    Val = MemReadByte (ZPAddr) << 1;
    MemWriteByte (ZPAddr, (unsigned char) Val);
    TEST_ZF (Val & 0xFF);
    TEST_SF (Val);
    SET_CF (Val & 0x100);
    Regs.PC += 2;
}



static void OPC_6502_18 (void)
/* Opcode $18: CLC */
{
    Cycles = 2;
    SET_CF (0);
    Regs.PC += 1;
}



static void OPC_6502_19 (void)
/* Opcode $19: ORA abs,y */
{
    AC_OP_ABSY (|);
}



static void OPC_65SC02_1A (void)
/* Opcode $1A: INC a */
{
    Cycles = 2;
    Regs.AC = (Regs.AC + 1) & 0xFF;
    TEST_ZF (Regs.AC);
    TEST_SF (Regs.AC);
    Regs.PC += 1;
}



static void OPC_65SC02_1C (void)
/* Opcode $1C: TRB abs */
{
    unsigned Addr;
    unsigned char Val;
    Cycles = 6;
    Addr = MemReadWord (Regs.PC+1);
    Val = MemReadByte (Addr);
    SET_ZF ((Val & Regs.AC) == 0);
    MemWriteByte (Addr, (unsigned char) (Val & ~Regs.AC));
    Regs.PC += 3;
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
    Addr = MemReadWord (Regs.PC+1) + Regs.XR;
    if (CPU != CPU_6502 && !PAGE_CROSS (Addr, Regs.XR))
        --Cycles;
    Val = MemReadByte (Addr) << 1;
    MemWriteByte (Addr, (unsigned char) Val);
    TEST_ZF (Val & 0xFF);
    TEST_SF (Val);
    SET_CF (Val & 0x100);
    Regs.PC += 3;
}



static void OPC_6502_20 (void)
/* Opcode $20: JSR */
{
    unsigned Addr;
    Cycles = 6;
    Addr = MemReadWord (Regs.PC+1);
    Regs.PC += 2;
    PUSH (PCH);
    PUSH (PCL);
    Regs.PC = Addr;

    ParaVirtHooks (&Regs);
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
    ZPAddr = MemReadByte (Regs.PC+1);
    Val = MemReadByte (ZPAddr);
    SET_SF (Val & 0x80);
    SET_OF (Val & 0x40);
    SET_ZF ((Val & Regs.AC) == 0);
    Regs.PC += 2;
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
    ZPAddr = MemReadByte (Regs.PC+1);
    Val = MemReadByte (ZPAddr);
    ROL (Val);
    MemWriteByte (ZPAddr, Val);
    Regs.PC += 2;
}



static void OPC_6502_28 (void)
/* Opcode $28: PLP */
{
    Cycles = 4;

    /* Bits 5 and 4 aren't used, and always are 1! */
    Regs.SR = (POP () | 0x30);
    Regs.PC += 1;
}



static void OPC_6502_29 (void)
/* Opcode $29: AND #imm */
{
    AC_OP_IMM (&);
}



static void OPC_6502_2A (void)
/* Opcode $2A: ROL a */
{
    Cycles = 2;
    ROL (Regs.AC);
    Regs.AC &= 0xFF;
    Regs.PC += 1;
}



static void OPC_6502_2C (void)
/* Opcode $2C: BIT abs */
{
    unsigned Addr;
    unsigned char Val;
    Cycles = 4;
    Addr = MemReadWord (Regs.PC+1);
    Val = MemReadByte (Addr);
    SET_SF (Val & 0x80);
    SET_OF (Val & 0x40);
    SET_ZF ((Val & Regs.AC) == 0);
    Regs.PC += 3;
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
    Addr = MemReadWord (Regs.PC+1);
    Val = MemReadByte (Addr);
    ROL (Val);
    MemWriteByte (Addr, Val);
    Regs.PC += 3;
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



static void OPC_65SC02_32 (void)
/* Opcode $32: AND (zp) */
{
    AC_OP_ZPIND (&);
}



static void OPC_65SC02_34 (void)
/* Opcode $34: BIT zp,x */
{
    unsigned char ZPAddr;
    unsigned char Val;
    Cycles = 4;
    ZPAddr = MemReadByte (Regs.PC+1) + Regs.XR;
    Val = MemReadByte (ZPAddr);
    SET_SF (Val & 0x80);
    SET_OF (Val & 0x40);
    SET_ZF ((Val & Regs.AC) == 0);
    Regs.PC += 2;
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
    ZPAddr = MemReadByte (Regs.PC+1) + Regs.XR;
    Val = MemReadByte (ZPAddr);
    ROL (Val);
    MemWriteByte (ZPAddr, Val);
    Regs.PC += 2;
}



static void OPC_6502_38 (void)
/* Opcode $38: SEC */
{
    Cycles = 2;
    SET_CF (1);
    Regs.PC += 1;
}



static void OPC_6502_39 (void)
/* Opcode $39: AND abs,y */
{
    AC_OP_ABSY (&);
}



static void OPC_65SC02_3A (void)
/* Opcode $3A: DEC a */
{
    Cycles = 2;
    Regs.AC = (Regs.AC - 1) & 0xFF;
    TEST_ZF (Regs.AC);
    TEST_SF (Regs.AC);
    Regs.PC += 1;
}



static void OPC_65SC02_3C (void)
/* Opcode $3C: BIT abs,x */
{
    unsigned Addr;
    unsigned char Val;
    Cycles = 4;
    Addr = MemReadWord (Regs.PC+1);
    if (PAGE_CROSS (Addr, Regs.XR))
        ++Cycles;
    Val  = MemReadByte (Addr + Regs.XR);
    SET_SF (Val & 0x80);
    SET_OF (Val & 0x40);
    SET_ZF ((Val & Regs.AC) == 0);
    Regs.PC += 3;
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
    Addr = MemReadWord (Regs.PC+1) + Regs.XR;
    if (CPU != CPU_6502 && !PAGE_CROSS (Addr, Regs.XR))
        --Cycles;
    Val = MemReadByte (Addr);
    ROL (Val);
    MemWriteByte (Addr, Val);
    Regs.PC += 2;
}



static void OPC_6502_40 (void)
/* Opcode $40: RTI */
{
    Cycles = 6;

    /* Bits 5 and 4 aren't used, and always are 1! */
    Regs.SR = POP () | 0x30;
    Regs.PC = POP ();                /* PCL */
    Regs.PC |= (POP () << 8);        /* PCH */
}



static void OPC_6502_41 (void)
/* Opcode $41: EOR (zp,x) */
{
    AC_OP_ZPXIND (^);
}



static void OPC_65C02_44 (void)
/* Opcode $44: 'zp' 3 cycle NOP */
{
    Cycles = 3;
    Regs.PC += 2;
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
    ZPAddr = MemReadByte (Regs.PC+1);
    Val = MemReadByte (ZPAddr);
    SET_CF (Val & 0x01);
    Val >>= 1;
    MemWriteByte (ZPAddr, Val);
    TEST_ZF (Val);
    TEST_SF (Val);
    Regs.PC += 2;
}



static void OPC_6502_48 (void)
/* Opcode $48: PHA */
{
    Cycles = 3;
    PUSH (Regs.AC);
    Regs.PC += 1;
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
    SET_CF (Regs.AC & 0x01);
    Regs.AC >>= 1;
    TEST_ZF (Regs.AC);
    TEST_SF (Regs.AC);
    Regs.PC += 1;
}



static void OPC_6502_4C (void)
/* Opcode $4C: JMP abs */
{
    Cycles = 3;
    Regs.PC = MemReadWord (Regs.PC+1);

    ParaVirtHooks (&Regs);
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
    Addr = MemReadWord (Regs.PC+1);
    Val = MemReadByte (Addr);
    SET_CF (Val & 0x01);
    Val >>= 1;
    MemWriteByte (Addr, Val);
    TEST_ZF (Val);
    TEST_SF (Val);
    Regs.PC += 3;
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



static void OPC_65SC02_52 (void)
/* Opcode $52: EOR (zp) */
{
    AC_OP_ZPIND (^);
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
    ZPAddr = MemReadByte (Regs.PC+1) + Regs.XR;
    Val = MemReadByte (ZPAddr);
    SET_CF (Val & 0x01);
    Val >>= 1;
    MemWriteByte (ZPAddr, Val);
    TEST_ZF (Val);
    TEST_SF (Val);
    Regs.PC += 2;
}



static void OPC_6502_58 (void)
/* Opcode $58: CLI */
{
    Cycles = 2;
    SET_IF (0);
    Regs.PC += 1;
}



static void OPC_6502_59 (void)
/* Opcode $59: EOR abs,y */
{
    AC_OP_ABSY (^);
}



static void OPC_65SC02_5A (void)
/* Opcode $5A: PHY */
{
    Cycles = 3;
    PUSH (Regs.YR);
    Regs.PC += 1;
}



static void OPC_65C02_5C (void)
/* Opcode $5C: 'Absolute' 8 cycle NOP */
{
    Cycles = 8;
    Regs.PC += 3;
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
    Addr = MemReadWord (Regs.PC+1) + Regs.XR;
    if (CPU != CPU_6502 && !PAGE_CROSS (Addr, Regs.XR))
        --Cycles;
    Val = MemReadByte (Addr);
    SET_CF (Val & 0x01);
    Val >>= 1;
    MemWriteByte (Addr, Val);
    TEST_ZF (Val);
    TEST_SF (Val);
    Regs.PC += 3;
}



static void OPC_6502_60 (void)
/* Opcode $60: RTS */
{
    Cycles = 6;
    Regs.PC = POP ();                /* PCL */
    Regs.PC |= (POP () << 8);        /* PCH */
    Regs.PC += 1;
}



static void OPC_6502_61 (void)
/* Opcode $61: ADC (zp,x) */
{
    unsigned char ZPAddr;
    unsigned Addr;
    Cycles = 6;
    ZPAddr = MemReadByte (Regs.PC+1) + Regs.XR;
    Addr = MemReadZPWord (ZPAddr);
    ADC (MemReadByte (Addr));
    Regs.PC += 2;
}



static void OPC_65SC02_64 (void)
/* Opcode $64: STZ zp */
{
    unsigned char ZPAddr;
    Cycles = 3;
    ZPAddr = MemReadByte (Regs.PC+1);
    MemWriteByte (ZPAddr, 0);
    Regs.PC += 2;
}



static void OPC_6502_65 (void)
/* Opcode $65: ADC zp */
{
    unsigned char ZPAddr;
    Cycles = 3;
    ZPAddr = MemReadByte (Regs.PC+1);
    ADC (MemReadByte (ZPAddr));
    Regs.PC += 2;
}



static void OPC_6502_66 (void)
/* Opcode $66: ROR zp */
{
    unsigned char ZPAddr;
    unsigned Val;
    Cycles = 5;
    ZPAddr = MemReadByte (Regs.PC+1);
    Val = MemReadByte (ZPAddr);
    ROR (Val);
    MemWriteByte (ZPAddr, Val);
    Regs.PC += 2;
}



static void OPC_6502_68 (void)
/* Opcode $68: PLA */
{
    Cycles = 4;
    Regs.AC = POP ();
    TEST_ZF (Regs.AC);
    TEST_SF (Regs.AC);
    Regs.PC += 1;
}



static void OPC_6502_69 (void)
/* Opcode $69: ADC #imm */
{
    Cycles = 2;
    ADC (MemReadByte (Regs.PC+1));
    Regs.PC += 2;
}



static void OPC_6502_6A (void)
/* Opcode $6A: ROR a */
{
    Cycles = 2;
    ROR (Regs.AC);
    Regs.PC += 1;
}



static void OPC_6502_6C (void)
/* Opcode $6C: JMP (ind) */
{
    unsigned PC, Lo, Hi;
    PC = Regs.PC;
    Lo = MemReadWord (PC+1);

    if (CPU == CPU_6502)
    {
         /* Emulate the 6502 bug */
        Cycles = 5;
        Regs.PC = MemReadByte (Lo);
        Hi = (Lo & 0xFF00) | ((Lo + 1) & 0xFF);
        Regs.PC |= (MemReadByte (Hi) << 8);

        /* Output a warning if the bug is triggered */
        if (Hi != Lo + 1)
        {
            Warning ("6502 indirect jump bug triggered at $%04X, ind addr = $%04X",
                     PC, Lo);
        }
    }
    else
    {
        Cycles = 6;
        Regs.PC = MemReadWord(Lo);
    }

    ParaVirtHooks (&Regs);
}



static void OPC_65C02_6C (void)
/* Opcode $6C: JMP (ind) */
{
    /* 6502 bug fixed here */
    Cycles = 5;
    Regs.PC = MemReadWord (MemReadWord (Regs.PC+1));

    ParaVirtHooks (&Regs);
}



static void OPC_6502_6D (void)
/* Opcode $6D: ADC abs */
{
    unsigned Addr;
    Cycles = 4;
    Addr   = MemReadWord (Regs.PC+1);
    ADC (MemReadByte (Addr));
    Regs.PC += 3;
}



static void OPC_6502_6E (void)
/* Opcode $6E: ROR abs */
{
    unsigned Addr;
    unsigned Val;
    Cycles = 6;
    Addr = MemReadWord (Regs.PC+1);
    Val  = MemReadByte (Addr);
    ROR (Val);
    MemWriteByte (Addr, Val);
    Regs.PC += 3;
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
    ZPAddr = MemReadByte (Regs.PC+1);
    Addr   = MemReadZPWord (ZPAddr);
    if (PAGE_CROSS (Addr, Regs.YR)) {
        ++Cycles;
    }
    ADC (MemReadByte (Addr + Regs.YR));
    Regs.PC += 2;
}



static void OPC_65SC02_72 (void)
/* Opcode $72: ADC (zp) */
{
    unsigned char ZPAddr;
    unsigned Addr;
    Cycles = 5;
    ZPAddr = MemReadByte (Regs.PC+1);
    Addr   = MemReadZPWord (ZPAddr);
    ADC (MemReadByte (Addr));
    Regs.PC += 2;
}



static void OPC_65SC02_74 (void)
/* Opcode $74: STZ zp,x */
{
    unsigned char ZPAddr;
    Cycles = 4;
    ZPAddr = MemReadByte (Regs.PC+1) + Regs.XR;
    MemWriteByte (ZPAddr, 0);
    Regs.PC += 2;
}



static void OPC_6502_75 (void)
/* Opcode $75: ADC zp,x */
{
    unsigned char ZPAddr;
    Cycles = 4;
    ZPAddr = MemReadByte (Regs.PC+1) + Regs.XR;
    ADC (MemReadByte (ZPAddr));
    Regs.PC += 2;
}



static void OPC_6502_76 (void)
/* Opcode $76: ROR zp,x */
{
    unsigned char ZPAddr;
    unsigned Val;
    Cycles = 6;
    ZPAddr = MemReadByte (Regs.PC+1) + Regs.XR;
    Val = MemReadByte (ZPAddr);
    ROR (Val);
    MemWriteByte (ZPAddr, Val);
    Regs.PC += 2;
}



static void OPC_6502_78 (void)
/* Opcode $78: SEI */
{
    Cycles = 2;
    SET_IF (1);
    Regs.PC += 1;
}



static void OPC_6502_79 (void)
/* Opcode $79: ADC abs,y */
{
    unsigned Addr;
    Cycles = 4;
    Addr = MemReadWord (Regs.PC+1);
    if (PAGE_CROSS (Addr, Regs.YR)) {
        ++Cycles;
    }
    ADC (MemReadByte (Addr + Regs.YR));
    Regs.PC += 3;
}



static void OPC_65SC02_7A (void)
/* Opcode $7A: PLY */
{
    Cycles = 4;
    Regs.YR = POP ();
    TEST_ZF (Regs.YR);
    TEST_SF (Regs.YR);
    Regs.PC += 1;
}



static void OPC_65SC02_7C (void)
/* Opcode $7C: JMP (ind,X) */
{
    unsigned PC, Adr;
    Cycles = 6;
    PC = Regs.PC;
    Adr = MemReadWord (PC+1);
    Regs.PC = MemReadWord(Adr+Regs.XR);

    ParaVirtHooks (&Regs);
}



static void OPC_6502_7D (void)
/* Opcode $7D: ADC abs,x */
{
    unsigned Addr;
    Cycles = 4;
    Addr = MemReadWord (Regs.PC+1);
    if (PAGE_CROSS (Addr, Regs.XR)) {
        ++Cycles;
    }
    ADC (MemReadByte (Addr + Regs.XR));
    Regs.PC += 3;
}



static void OPC_6502_7E (void)
/* Opcode $7E: ROR abs,x */
{
    unsigned Addr;
    unsigned Val;
    Cycles = 7;
    Addr = MemReadWord (Regs.PC+1) + Regs.XR;
    if (CPU != CPU_6502 && !PAGE_CROSS (Addr, Regs.XR))
        --Cycles;
    Val = MemReadByte (Addr);
    ROR (Val);
    MemWriteByte (Addr, Val);
    Regs.PC += 3;
}



static void OPC_65SC02_80 (void)
/* Opcode $80: BRA */
{
    BRANCH (1);
}



static void OPC_6502_81 (void)
/* Opcode $81: STA (zp,x) */
{
    unsigned char ZPAddr;
    unsigned Addr;
    Cycles = 6;
    ZPAddr = MemReadByte (Regs.PC+1) + Regs.XR;
    Addr = MemReadZPWord (ZPAddr);
    MemWriteByte (Addr, Regs.AC);
    Regs.PC += 2;
}



static void OPC_6502_84 (void)
/* Opcode $84: STY zp */
{
    unsigned char ZPAddr;
    Cycles = 3;
    ZPAddr = MemReadByte (Regs.PC+1);
    MemWriteByte (ZPAddr, Regs.YR);
    Regs.PC += 2;
}



static void OPC_6502_85 (void)
/* Opcode $85: STA zp */
{
    unsigned char ZPAddr;
    Cycles = 3;
    ZPAddr = MemReadByte (Regs.PC+1);
    MemWriteByte (ZPAddr, Regs.AC);
    Regs.PC += 2;
}



static void OPC_6502_86 (void)
/* Opcode $86: STX zp */
{
    unsigned char ZPAddr;
    Cycles = 3;
    ZPAddr = MemReadByte (Regs.PC+1);
    MemWriteByte (ZPAddr, Regs.XR);
    Regs.PC += 2;
}



static void OPC_6502_88 (void)
/* Opcode $88: DEY */
{
    Cycles = 2;
    Regs.YR = (Regs.YR - 1) & 0xFF;
    TEST_ZF (Regs.YR);
    TEST_SF (Regs.YR);
    Regs.PC += 1;
}



static void OPC_65SC02_89 (void)
/* Opcode $89: BIT #imm */
{
    unsigned char Val;
    Cycles = 2;
    Val = MemReadByte (Regs.PC+1);
    SET_SF (Val & 0x80);
    SET_OF (Val & 0x40);
    SET_ZF ((Val & Regs.AC) == 0);
    Regs.PC += 2;
}



static void OPC_6502_8A (void)
/* Opcode $8A: TXA */
{
    Cycles = 2;
    Regs.AC = Regs.XR;
    TEST_ZF (Regs.AC);
    TEST_SF (Regs.AC);
    Regs.PC += 1;
}



static void OPC_6502_8C (void)
/* Opcode $8C: STY abs */
{
    unsigned Addr;
    Cycles = 4;
    Addr = MemReadWord (Regs.PC+1);
    MemWriteByte (Addr, Regs.YR);
    Regs.PC += 3;
}



static void OPC_6502_8D (void)
/* Opcode $8D: STA abs */
{
    unsigned Addr;
    Cycles = 4;
    Addr = MemReadWord (Regs.PC+1);
    MemWriteByte (Addr, Regs.AC);
    Regs.PC += 3;
}



static void OPC_6502_8E (void)
/* Opcode $8E: STX abs */
{
    unsigned Addr;
    Cycles = 4;
    Addr = MemReadWord (Regs.PC+1);
    MemWriteByte (Addr, Regs.XR);
    Regs.PC += 3;
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
    ZPAddr = MemReadByte (Regs.PC+1);
    Addr = MemReadZPWord (ZPAddr) + Regs.YR;
    MemWriteByte (Addr, Regs.AC);
    Regs.PC += 2;
}



static void OPC_65SC02_92 (void)
/* Opcode $92: sta (zp) */
{
    unsigned char ZPAddr;
    unsigned Addr;
    Cycles = 5;
    ZPAddr = MemReadByte (Regs.PC+1);
    Addr = MemReadZPWord (ZPAddr);
    MemWriteByte (Addr, Regs.AC);
    Regs.PC += 2;
}



static void OPC_6502_94 (void)
/* Opcode $94: STY zp,x */
{
    unsigned char ZPAddr;
    Cycles = 4;
    ZPAddr = MemReadByte (Regs.PC+1) + Regs.XR;
    MemWriteByte (ZPAddr, Regs.YR);
    Regs.PC += 2;
}



static void OPC_6502_95 (void)
/* Opcode $95: STA zp,x */
{
    unsigned char ZPAddr;
    Cycles = 4;
    ZPAddr = MemReadByte (Regs.PC+1) + Regs.XR;
    MemWriteByte (ZPAddr, Regs.AC);
    Regs.PC += 2;
}



static void OPC_6502_96 (void)
/* Opcode $96: stx zp,y */
{
    unsigned char ZPAddr;
    Cycles = 4;
    ZPAddr = MemReadByte (Regs.PC+1) + Regs.YR;
    MemWriteByte (ZPAddr, Regs.XR);
    Regs.PC += 2;
}



static void OPC_6502_98 (void)
/* Opcode $98: TYA */
{
    Cycles = 2;
    Regs.AC = Regs.YR;
    TEST_ZF (Regs.AC);
    TEST_SF (Regs.AC);
    Regs.PC += 1;
}



static void OPC_6502_99 (void)
/* Opcode $99: STA abs,y */
{
    unsigned Addr;
    Cycles = 5;
    Addr = MemReadWord (Regs.PC+1) + Regs.YR;
    MemWriteByte (Addr, Regs.AC);
    Regs.PC += 3;
}



static void OPC_6502_9A (void)
/* Opcode $9A: TXS */
{
    Cycles = 2;
    Regs.SP = Regs.XR;
    Regs.PC += 1;
}



static void OPC_65SC02_9C (void)
/* Opcode $9C: STZ abs */
{
    unsigned Addr;
    Cycles = 4;
    Addr = MemReadWord (Regs.PC+1);
    MemWriteByte (Addr, 0);
    Regs.PC += 3;
}



static void OPC_6502_9D (void)
/* Opcode $9D: STA abs,x */
{
    unsigned Addr;
    Cycles = 5;
    Addr = MemReadWord (Regs.PC+1) + Regs.XR;
    MemWriteByte (Addr, Regs.AC);
    Regs.PC += 3;
}



static void OPC_65SC02_9E (void)
/* Opcode $9E: STZ abs,x */
{
    unsigned Addr;
    Cycles = 5;
    Addr = MemReadWord (Regs.PC+1) + Regs.XR;
    MemWriteByte (Addr, 0);
    Regs.PC += 3;
}



static void OPC_6502_A0 (void)
/* Opcode $A0: LDY #imm */
{
    Cycles = 2;
    Regs.YR = MemReadByte (Regs.PC+1);
    TEST_ZF (Regs.YR);
    TEST_SF (Regs.YR);
    Regs.PC += 2;
}



static void OPC_6502_A1 (void)
/* Opcode $A1: LDA (zp,x) */
{
    unsigned char ZPAddr;
    unsigned Addr;
    Cycles = 6;
    ZPAddr = MemReadByte (Regs.PC+1) + Regs.XR;
    Addr = MemReadZPWord (ZPAddr);
    Regs.AC = MemReadByte (Addr);
    TEST_ZF (Regs.AC);
    TEST_SF (Regs.AC);
    Regs.PC += 2;
}



static void OPC_6502_A2 (void)
/* Opcode $A2: LDX #imm */
{
    Cycles = 2;
    Regs.XR = MemReadByte (Regs.PC+1);
    TEST_ZF (Regs.XR);
    TEST_SF (Regs.XR);
    Regs.PC += 2;
}



static void OPC_6502_A4 (void)
/* Opcode $A4: LDY zp */
{
    unsigned char ZPAddr;
    Cycles = 3;
    ZPAddr = MemReadByte (Regs.PC+1);
    Regs.YR = MemReadByte (ZPAddr);
    TEST_ZF (Regs.YR);
    TEST_SF (Regs.YR);
    Regs.PC += 2;
}



static void OPC_6502_A5 (void)
/* Opcode $A5: LDA zp */
{
    unsigned char ZPAddr;
    Cycles = 3;
    ZPAddr = MemReadByte (Regs.PC+1);
    Regs.AC = MemReadByte (ZPAddr);
    TEST_ZF (Regs.AC);
    TEST_SF (Regs.AC);
    Regs.PC += 2;
}



static void OPC_6502_A6 (void)
/* Opcode $A6: LDX zp */
{
    unsigned char ZPAddr;
    Cycles = 3;
    ZPAddr = MemReadByte (Regs.PC+1);
    Regs.XR = MemReadByte (ZPAddr);
    TEST_ZF (Regs.XR);
    TEST_SF (Regs.XR);
    Regs.PC += 2;
}



static void OPC_6502_A8 (void)
/* Opcode $A8: TAY */
{
    Cycles = 2;
    Regs.YR = Regs.AC;
    TEST_ZF (Regs.YR);
    TEST_SF (Regs.YR);
    Regs.PC += 1;
}



static void OPC_6502_A9 (void)
/* Opcode $A9: LDA #imm */
{
    Cycles = 2;
    Regs.AC = MemReadByte (Regs.PC+1);
    TEST_ZF (Regs.AC);
    TEST_SF (Regs.AC);
    Regs.PC += 2;
}



static void OPC_6502_AA (void)
/* Opcode $AA: TAX */
{
    Cycles = 2;
    Regs.XR = Regs.AC;
    TEST_ZF (Regs.XR);
    TEST_SF (Regs.XR);
    Regs.PC += 1;
}



static void OPC_6502_AC (void)
/* Opcode $Regs.AC: LDY abs */
{
    unsigned Addr;
    Cycles = 4;
    Addr = MemReadWord (Regs.PC+1);
    Regs.YR = MemReadByte (Addr);
    TEST_ZF (Regs.YR);
    TEST_SF (Regs.YR);
    Regs.PC += 3;
}



static void OPC_6502_AD (void)
/* Opcode $AD: LDA abs */
{
    unsigned Addr;
    Cycles = 4;
    Addr = MemReadWord (Regs.PC+1);
    Regs.AC = MemReadByte (Addr);
    TEST_ZF (Regs.AC);
    TEST_SF (Regs.AC);
    Regs.PC += 3;
}



static void OPC_6502_AE (void)
/* Opcode $AE: LDX abs */
{
    unsigned Addr;
    Cycles = 4;
    Addr = MemReadWord (Regs.PC+1);
    Regs.XR = MemReadByte (Addr);
    TEST_ZF (Regs.XR);
    TEST_SF (Regs.XR);
    Regs.PC += 3;
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
    ZPAddr = MemReadByte (Regs.PC+1);
    Addr = MemReadZPWord (ZPAddr);
    if (PAGE_CROSS (Addr, Regs.YR)) {
        ++Cycles;
    }
    Regs.AC = MemReadByte (Addr + Regs.YR);
    TEST_ZF (Regs.AC);
    TEST_SF (Regs.AC);
    Regs.PC += 2;
}



static void OPC_65SC02_B2 (void)
/* Opcode $B2: LDA (zp) */
{
    unsigned char ZPAddr;
    unsigned Addr;
    Cycles = 5;
    ZPAddr = MemReadByte (Regs.PC+1);
    Addr = MemReadZPWord (ZPAddr);
    Regs.AC = MemReadByte (Addr);
    TEST_ZF (Regs.AC);
    TEST_SF (Regs.AC);
    Regs.PC += 2;
}



static void OPC_6502_B4 (void)
/* Opcode $B4: LDY zp,x */
{
    unsigned char ZPAddr;
    Cycles = 4;
    ZPAddr = MemReadByte (Regs.PC+1) + Regs.XR;
    Regs.YR = MemReadByte (ZPAddr);
    TEST_ZF (Regs.YR);
    TEST_SF (Regs.YR);
    Regs.PC += 2;
}



static void OPC_6502_B5 (void)
/* Opcode $B5: LDA zp,x */
{
    unsigned char ZPAddr;
    Cycles = 4;
    ZPAddr = MemReadByte (Regs.PC+1) + Regs.XR;
    Regs.AC = MemReadByte (ZPAddr);
    TEST_ZF (Regs.AC);
    TEST_SF (Regs.AC);
    Regs.PC += 2;
}



static void OPC_6502_B6 (void)
/* Opcode $B6: LDX zp,y */
{
    unsigned char ZPAddr;
    Cycles = 4;
    ZPAddr = MemReadByte (Regs.PC+1) + Regs.YR;
    Regs.XR = MemReadByte (ZPAddr);
    TEST_ZF (Regs.XR);
    TEST_SF (Regs.XR);
    Regs.PC += 2;
}



static void OPC_6502_B8 (void)
/* Opcode $B8: CLV */
{
    Cycles = 2;
    SET_OF (0);
    Regs.PC += 1;
}



static void OPC_6502_B9 (void)
/* Opcode $B9: LDA abs,y */
{
    unsigned Addr;
    Cycles = 4;
    Addr = MemReadWord (Regs.PC+1);
    if (PAGE_CROSS (Addr, Regs.YR)) {
        ++Cycles;
    }
    Regs.AC = MemReadByte (Addr + Regs.YR);
    TEST_ZF (Regs.AC);
    TEST_SF (Regs.AC);
    Regs.PC += 3;
}



static void OPC_6502_BA (void)
/* Opcode $BA: TSX */
{
    Cycles = 2;
    Regs.XR = Regs.SP & 0xFF;
    TEST_ZF (Regs.XR);
    TEST_SF (Regs.XR);
    Regs.PC += 1;
}



static void OPC_6502_BC (void)
/* Opcode $BC: LDY abs,x */
{
    unsigned Addr;
    Cycles = 4;
    Addr = MemReadWord (Regs.PC+1);
    if (PAGE_CROSS (Addr, Regs.XR)) {
        ++Cycles;
    }
    Regs.YR = MemReadByte (Addr + Regs.XR);
    TEST_ZF (Regs.YR);
    TEST_SF (Regs.YR);
    Regs.PC += 3;
}



static void OPC_6502_BD (void)
/* Opcode $BD: LDA abs,x */
{
    unsigned Addr;
    Cycles = 4;
    Addr = MemReadWord (Regs.PC+1);
    if (PAGE_CROSS (Addr, Regs.XR)) {
        ++Cycles;
    }
    Regs.AC = MemReadByte (Addr + Regs.XR);
    TEST_ZF (Regs.AC);
    TEST_SF (Regs.AC);
    Regs.PC += 3;
}



static void OPC_6502_BE (void)
/* Opcode $BE: LDX abs,y */
{
    unsigned Addr;
    Cycles = 4;
    Addr = MemReadWord (Regs.PC+1);
    if (PAGE_CROSS (Addr, Regs.YR)) {
        ++Cycles;
    }
    Regs.XR = MemReadByte (Addr + Regs.YR);
    TEST_ZF (Regs.XR);
    TEST_SF (Regs.XR);
    Regs.PC += 3;
}



static void OPC_6502_C0 (void)
/* Opcode $C0: CPY #imm */
{
    Cycles = 2;
    CMP (Regs.YR, MemReadByte (Regs.PC+1));
    Regs.PC += 2;
}



static void OPC_6502_C1 (void)
/* Opcode $C1: CMP (zp,x) */
{
    unsigned char ZPAddr;
    unsigned Addr;
    Cycles = 6;
    ZPAddr = MemReadByte (Regs.PC+1) + Regs.XR;
    Addr = MemReadZPWord (ZPAddr);
    CMP (Regs.AC, MemReadByte (Addr));
    Regs.PC += 2;
}



static void OPC_6502_C4 (void)
/* Opcode $C4: CPY zp */
{
    unsigned char ZPAddr;
    Cycles = 3;
    ZPAddr = MemReadByte (Regs.PC+1);
    CMP (Regs.YR, MemReadByte (ZPAddr));
    Regs.PC += 2;
}



static void OPC_6502_C5 (void)
/* Opcode $C5: CMP zp */
{
    unsigned char ZPAddr;
    Cycles = 3;
    ZPAddr = MemReadByte (Regs.PC+1);
    CMP (Regs.AC, MemReadByte (ZPAddr));
    Regs.PC += 2;
}



static void OPC_6502_C6 (void)
/* Opcode $C6: DEC zp */
{
    unsigned char ZPAddr;
    unsigned char Val;
    Cycles = 5;
    ZPAddr = MemReadByte (Regs.PC+1);
    Val = MemReadByte (ZPAddr) - 1;
    MemWriteByte (ZPAddr, Val);
    TEST_ZF (Val);
    TEST_SF (Val);
    Regs.PC += 2;
}



static void OPC_6502_C8 (void)
/* Opcode $C8: INY */
{
    Cycles = 2;
    Regs.YR = (Regs.YR + 1) & 0xFF;
    TEST_ZF (Regs.YR);
    TEST_SF (Regs.YR);
    Regs.PC += 1;
}



static void OPC_6502_C9 (void)
/* Opcode $C9: CMP #imm */
{
    Cycles = 2;
    CMP (Regs.AC, MemReadByte (Regs.PC+1));
    Regs.PC += 2;
}



static void OPC_6502_CA (void)
/* Opcode $CA: DEX */
{
    Cycles = 2;
    Regs.XR = (Regs.XR - 1) & 0xFF;
    TEST_ZF (Regs.XR);
    TEST_SF (Regs.XR);
    Regs.PC += 1;
}



static void OPC_6502_CC (void)
/* Opcode $CC: CPY abs */
{
    unsigned Addr;
    Cycles = 4;
    Addr = MemReadWord (Regs.PC+1);
    CMP (Regs.YR, MemReadByte (Addr));
    Regs.PC += 3;
}



static void OPC_6502_CD (void)
/* Opcode $CD: CMP abs */
{
    unsigned Addr;
    Cycles = 4;
    Addr = MemReadWord (Regs.PC+1);
    CMP (Regs.AC, MemReadByte (Addr));
    Regs.PC += 3;
}



static void OPC_6502_CE (void)
/* Opcode $CE: DEC abs */
{
    unsigned Addr;
    unsigned char Val;
    Cycles = 6;
    Addr = MemReadWord (Regs.PC+1);
    Val  = MemReadByte (Addr) - 1;
    MemWriteByte (Addr, Val);
    TEST_ZF (Val);
    TEST_SF (Val);
    Regs.PC += 3;
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
    ZPAddr = MemReadByte (Regs.PC+1);
    Addr = MemReadWord (ZPAddr);
    if (PAGE_CROSS (Addr, Regs.YR)) {
        ++Cycles;
    }
    CMP (Regs.AC, MemReadByte (Addr + Regs.YR));
    Regs.PC += 2;
}



static void OPC_65SC02_D2 (void)
/* Opcode $D2: CMP (zp) */
{
    unsigned ZPAddr;
    unsigned Addr;
    Cycles = 5;
    ZPAddr = MemReadByte (Regs.PC+1);
    Addr = MemReadWord (ZPAddr);
    CMP (Regs.AC, MemReadByte (Addr));
    Regs.PC += 2;
}



static void OPC_6502_D5 (void)
/* Opcode $D5: CMP zp,x */
{
    unsigned char ZPAddr;
    Cycles = 4;
    ZPAddr = MemReadByte (Regs.PC+1) + Regs.XR;
    CMP (Regs.AC, MemReadByte (ZPAddr));
    Regs.PC += 2;
}



static void OPC_6502_D6 (void)
/* Opcode $D6: DEC zp,x */
{
    unsigned char ZPAddr;
    unsigned char Val;
    Cycles = 6;
    ZPAddr = MemReadByte (Regs.PC+1) + Regs.XR;
    Val = MemReadByte (ZPAddr) - 1;
    MemWriteByte (ZPAddr, Val);
    TEST_ZF (Val);
    TEST_SF (Val);
    Regs.PC += 2;
}



static void OPC_6502_D8 (void)
/* Opcode $D8: CLD */
{
    Cycles = 2;
    SET_DF (0);
    Regs.PC += 1;
}



static void OPC_6502_D9 (void)
/* Opcode $D9: CMP abs,y */
{
    unsigned Addr;
    Cycles = 4;
    Addr = MemReadWord (Regs.PC+1);
    if (PAGE_CROSS (Addr, Regs.YR)) {
        ++Cycles;
    }
    CMP (Regs.AC, MemReadByte (Addr + Regs.YR));
    Regs.PC += 3;
}



static void OPC_65SC02_DA (void)
/* Opcode $DA: PHX */
{
    Cycles = 3;
    PUSH (Regs.XR);
    Regs.PC += 1;
}



static void OPC_6502_DD (void)
/* Opcode $DD: CMP abs,x */
{
    unsigned Addr;
    Cycles = 4;
    Addr = MemReadWord (Regs.PC+1);
    if (PAGE_CROSS (Addr, Regs.XR)) {
        ++Cycles;
    }
    CMP (Regs.AC, MemReadByte (Addr + Regs.XR));
    Regs.PC += 3;
}



static void OPC_6502_DE (void)
/* Opcode $DE: DEC abs,x */
{
    unsigned Addr;
    unsigned char Val;
    Cycles = 7;
    Addr = MemReadWord (Regs.PC+1) + Regs.XR;
    Val = MemReadByte (Addr) - 1;
    MemWriteByte (Addr, Val);
    TEST_ZF (Val);
    TEST_SF (Val);
    Regs.PC += 3;
}



static void OPC_6502_E0 (void)
/* Opcode $E0: CPX #imm */
{
    Cycles = 2;
    CMP (Regs.XR, MemReadByte (Regs.PC+1));
    Regs.PC += 2;
}



static void OPC_6502_E1 (void)
/* Opcode $E1: SBC (zp,x) */
{
    unsigned char ZPAddr;
    unsigned Addr;
    Cycles = 6;
    ZPAddr = MemReadByte (Regs.PC+1) + Regs.XR;
    Addr = MemReadZPWord (ZPAddr);
    SBC (MemReadByte (Addr));
    Regs.PC += 2;
}



static void OPC_6502_E4 (void)
/* Opcode $E4: CPX zp */
{
    unsigned char ZPAddr;
    Cycles = 3;
    ZPAddr = MemReadByte (Regs.PC+1);
    CMP (Regs.XR, MemReadByte (ZPAddr));
    Regs.PC += 2;
}



static void OPC_6502_E5 (void)
/* Opcode $E5: SBC zp */
{
    unsigned char ZPAddr;
    Cycles = 3;
    ZPAddr = MemReadByte (Regs.PC+1);
    SBC (MemReadByte (ZPAddr));
    Regs.PC += 2;
}



static void OPC_6502_E6 (void)
/* Opcode $E6: INC zp */
{
    unsigned char ZPAddr;
    unsigned char Val;
    Cycles = 5;
    ZPAddr = MemReadByte (Regs.PC+1);
    Val = MemReadByte (ZPAddr) + 1;
    MemWriteByte (ZPAddr, Val);
    TEST_ZF (Val);
    TEST_SF (Val);
    Regs.PC += 2;
}



static void OPC_6502_E8 (void)
/* Opcode $E8: INX */
{
    Cycles = 2;
    Regs.XR = (Regs.XR + 1) & 0xFF;
    TEST_ZF (Regs.XR);
    TEST_SF (Regs.XR);
    Regs.PC += 1;
}



static void OPC_6502_E9 (void)
/* Opcode $E9: SBC #imm */
{
    Cycles = 2;
    SBC (MemReadByte (Regs.PC+1));
    Regs.PC += 2;
}



static void OPC_6502_EA (void)
/* Opcode $EA: NOP */
{
    /* This one is easy... */
    Cycles = 2;
    Regs.PC += 1;
}



static void OPC_65C02_NOP11(void)
/* Opcode 'Illegal' 1 cycle NOP */
{
    Cycles = 1;
    Regs.PC += 1;
}



static void OPC_65C02_NOP22 (void)
/* Opcode 'Illegal' 2 byte 2 cycle NOP */
{
    Cycles = 2;
    Regs.PC += 2;
}



static void OPC_65C02_NOP24 (void)
/* Opcode 'Illegal' 2 byte 4 cycle NOP */
{
    Cycles = 4;
    Regs.PC += 2;
}



static void OPC_65C02_NOP34 (void)
/* Opcode 'Illegal' 3 byte 4 cycle NOP */
{
    Cycles = 4;
    Regs.PC += 3;
}



static void OPC_6502_EC (void)
/* Opcode $EC: CPX abs */
{
    unsigned Addr;
    Cycles = 4;
    Addr   = MemReadWord (Regs.PC+1);
    CMP (Regs.XR, MemReadByte (Addr));
    Regs.PC += 3;
}



static void OPC_6502_ED (void)
/* Opcode $ED: SBC abs */
{
    unsigned Addr;
    Cycles = 4;
    Addr = MemReadWord (Regs.PC+1);
    SBC (MemReadByte (Addr));
    Regs.PC += 3;
}



static void OPC_6502_EE (void)
/* Opcode $EE: INC abs */
{
    unsigned Addr;
    unsigned char Val;
    Cycles = 6;
    Addr = MemReadWord (Regs.PC+1);
    Val = MemReadByte (Addr) + 1;
    MemWriteByte (Addr, Val);
    TEST_ZF (Val);
    TEST_SF (Val);
    Regs.PC += 3;
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
    ZPAddr = MemReadByte (Regs.PC+1);
    Addr = MemReadZPWord (ZPAddr);
    if (PAGE_CROSS (Addr, Regs.YR)) {
        ++Cycles;
    }
    SBC (MemReadByte (Addr + Regs.YR));
    Regs.PC += 2;
}



static void OPC_65SC02_F2 (void)
/* Opcode $F2: SBC (zp) */
{
    unsigned char ZPAddr;
    unsigned Addr;
    Cycles = 5;
    ZPAddr = MemReadByte (Regs.PC+1);
    Addr = MemReadZPWord (ZPAddr);
    SBC (MemReadByte (Addr));
    Regs.PC += 2;
}



static void OPC_6502_F5 (void)
/* Opcode $F5: SBC zp,x */
{
    unsigned char ZPAddr;
    Cycles = 4;
    ZPAddr = MemReadByte (Regs.PC+1) + Regs.XR;
    SBC (MemReadByte (ZPAddr));
    Regs.PC += 2;
}



static void OPC_6502_F6 (void)
/* Opcode $F6: INC zp,x */
{
    unsigned char ZPAddr;
    unsigned char Val;
    Cycles = 6;
    ZPAddr = MemReadByte (Regs.PC+1) + Regs.XR;
    Val = MemReadByte (ZPAddr) + 1;
    MemWriteByte (ZPAddr, Val);
    TEST_ZF (Val);
    TEST_SF (Val);
    Regs.PC += 2;
}



static void OPC_6502_F8 (void)
/* Opcode $F8: SED */
{
    Cycles = 2;
    SET_DF (1);
    Regs.PC += 1;
}



static void OPC_6502_F9 (void)
/* Opcode $F9: SBC abs,y */
{
    unsigned Addr;
    Cycles = 4;
    Addr = MemReadWord (Regs.PC+1);
    if (PAGE_CROSS (Addr, Regs.YR)) {
        ++Cycles;
    }
    SBC (MemReadByte (Addr + Regs.YR));
    Regs.PC += 3;
}



static void OPC_65SC02_FA (void)
/* Opcode $7A: PLX */
{
    Cycles = 4;
    Regs.XR = POP ();
    TEST_ZF (Regs.XR);
    TEST_SF (Regs.XR);
    Regs.PC += 1;
}



static void OPC_6502_FD (void)
/* Opcode $FD: SBC abs,x */
{
    unsigned Addr;
    Cycles = 4;
    Addr = MemReadWord (Regs.PC+1);
    if (PAGE_CROSS (Addr, Regs.XR)) {
        ++Cycles;
    }
    SBC (MemReadByte (Addr + Regs.XR));
    Regs.PC += 3;
}



static void OPC_6502_FE (void)
/* Opcode $FE: INC abs,x */
{
    unsigned Addr;
    unsigned char Val;
    Cycles = 7;
    Addr = MemReadWord (Regs.PC+1) + Regs.XR;
    Val = MemReadByte (Addr) + 1;
    MemWriteByte (Addr, Val);
    TEST_ZF (Val);
    TEST_SF (Val);
    Regs.PC += 3;
}



/*****************************************************************************/
/*                           Opcode handler tables                           */
/*****************************************************************************/



/* Opcode handler table for the 6502 */
static const OPFunc OP6502Table[256] = {
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



/* Opcode handler table for the 65C02 */
static const OPFunc OP65C02Table[256] = {
    OPC_6502_00,
    OPC_6502_01,
    OPC_65C02_NOP22,    // $02
    OPC_65C02_NOP11,    // $03
    OPC_65SC02_04,
    OPC_6502_05,
    OPC_6502_06,
    OPC_Illegal,        // $07: RMB0 currently unsupported
    OPC_6502_08,
    OPC_6502_09,
    OPC_6502_0A,
    OPC_65C02_NOP11,    // $0B
    OPC_65SC02_0C,
    OPC_6502_0D,
    OPC_6502_0E,
    OPC_Illegal,        // $0F: BBR0 currently unsupported
    OPC_6502_10,
    OPC_6502_11,
    OPC_65SC02_12,
    OPC_65C02_NOP11,    // $13
    OPC_65SC02_14,
    OPC_6502_15,
    OPC_6502_16,
    OPC_Illegal,        // $17: RMB1 currently unsupported
    OPC_6502_18,
    OPC_6502_19,
    OPC_65SC02_1A,
    OPC_65C02_NOP11,    // $1B
    OPC_65SC02_1C,
    OPC_6502_1D,
    OPC_6502_1E,
    OPC_Illegal,        // $1F: BBR1 currently unsupported
    OPC_6502_20,
    OPC_6502_21,
    OPC_65C02_NOP22,    // $22
    OPC_65C02_NOP11,    // $23
    OPC_6502_24,
    OPC_6502_25,
    OPC_6502_26,
    OPC_Illegal,        // $27: RMB2 currently unsupported
    OPC_6502_28,
    OPC_6502_29,
    OPC_6502_2A,
    OPC_65C02_NOP11,    // $2B
    OPC_6502_2C,
    OPC_6502_2D,
    OPC_6502_2E,
    OPC_Illegal,        // $2F: BBR2 currently unsupported
    OPC_6502_30,
    OPC_6502_31,
    OPC_65SC02_32,
    OPC_65C02_NOP11,    // $33
    OPC_65SC02_34,
    OPC_6502_35,
    OPC_6502_36,
    OPC_Illegal,        // $37: RMB3 currently unsupported
    OPC_6502_38,
    OPC_6502_39,
    OPC_65SC02_3A,
    OPC_65C02_NOP11,    // $3B
    OPC_65SC02_3C,
    OPC_6502_3D,
    OPC_6502_3E,
    OPC_Illegal,        // $3F: BBR3 currently unsupported
    OPC_6502_40,
    OPC_6502_41,
    OPC_65C02_NOP22,    // $42
    OPC_65C02_NOP11,    // $43
    OPC_65C02_44,       // $44
    OPC_6502_45,
    OPC_6502_46,
    OPC_Illegal,        // $47: RMB4 currently unsupported
    OPC_6502_48,
    OPC_6502_49,
    OPC_6502_4A,
    OPC_65C02_NOP11,    // $4B
    OPC_6502_4C,
    OPC_6502_4D,
    OPC_6502_4E,
    OPC_Illegal,        // $4F: BBR4 currently unsupported
    OPC_6502_50,
    OPC_6502_51,
    OPC_65SC02_52,
    OPC_65C02_NOP11,    // $53
    OPC_65C02_NOP24,    // $54
    OPC_6502_55,
    OPC_6502_56,
    OPC_Illegal,        // $57: RMB5 currently unsupported
    OPC_6502_58,
    OPC_6502_59,
    OPC_65SC02_5A,
    OPC_65C02_NOP11,    // $5B
    OPC_65C02_5C,
    OPC_6502_5D,
    OPC_6502_5E,
    OPC_Illegal,        // $5F: BBR5 currently unsupported
    OPC_6502_60,
    OPC_6502_61,
    OPC_65C02_NOP22,    // $62
    OPC_65C02_NOP11,    // $63
    OPC_65SC02_64,
    OPC_6502_65,
    OPC_6502_66,
    OPC_Illegal,        // $67: RMB6 currently unsupported
    OPC_6502_68,
    OPC_6502_69,
    OPC_6502_6A,
    OPC_65C02_NOP11,    // $6B
    OPC_65C02_6C,
    OPC_6502_6D,
    OPC_6502_6E,
    OPC_Illegal,        // $6F: BBR6 currently unsupported
    OPC_6502_70,
    OPC_6502_71,
    OPC_65SC02_72,
    OPC_65C02_NOP11,    // $73
    OPC_65SC02_74,
    OPC_6502_75,
    OPC_6502_76,
    OPC_Illegal,        // $77: RMB7 currently unsupported
    OPC_6502_78,
    OPC_6502_79,
    OPC_65SC02_7A,
    OPC_65C02_NOP11,    // $7B
    OPC_65SC02_7C,
    OPC_6502_7D,
    OPC_6502_7E,
    OPC_Illegal,        // $7F: BBR7 currently unsupported
    OPC_65SC02_80,
    OPC_6502_81,
    OPC_65C02_NOP22,    // $82
    OPC_65C02_NOP11,    // $83
    OPC_6502_84,
    OPC_6502_85,
    OPC_6502_86,
    OPC_Illegal,        // $87: SMB0 currently unsupported
    OPC_6502_88,
    OPC_65SC02_89,
    OPC_6502_8A,
    OPC_65C02_NOP11,    // $8B
    OPC_6502_8C,
    OPC_6502_8D,
    OPC_6502_8E,
    OPC_Illegal,        // $8F: BBS0 currently unsupported
    OPC_6502_90,
    OPC_6502_91,
    OPC_65SC02_92,
    OPC_65C02_NOP11,    // $93
    OPC_6502_94,
    OPC_6502_95,
    OPC_6502_96,
    OPC_Illegal,        // $97: SMB1 currently unsupported
    OPC_6502_98,
    OPC_6502_99,
    OPC_6502_9A,
    OPC_65C02_NOP11,    // $9B
    OPC_65SC02_9C,
    OPC_6502_9D,
    OPC_65SC02_9E,
    OPC_Illegal,        // $9F: BBS1 currently unsupported
    OPC_6502_A0,
    OPC_6502_A1,
    OPC_6502_A2,
    OPC_65C02_NOP11,    // $A3
    OPC_6502_A4,
    OPC_6502_A5,
    OPC_6502_A6,
    OPC_Illegal,        // $A7: SMB2 currently unsupported
    OPC_6502_A8,
    OPC_6502_A9,
    OPC_6502_AA,
    OPC_65C02_NOP11,    // $AB
    OPC_6502_AC,
    OPC_6502_AD,
    OPC_6502_AE,
    OPC_Illegal,        // $AF: BBS2 currently unsupported
    OPC_6502_B0,
    OPC_6502_B1,
    OPC_65SC02_B2,
    OPC_65C02_NOP11,    // $B3
    OPC_6502_B4,
    OPC_6502_B5,
    OPC_6502_B6,
    OPC_Illegal,        // $B7: SMB3 currently unsupported
    OPC_6502_B8,
    OPC_6502_B9,
    OPC_6502_BA,
    OPC_65C02_NOP11,    // $BB
    OPC_6502_BC,
    OPC_6502_BD,
    OPC_6502_BE,
    OPC_Illegal,        // $BF: BBS3 currently unsupported
    OPC_6502_C0,
    OPC_6502_C1,
    OPC_65C02_NOP22,    // $C2
    OPC_65C02_NOP11,    // $C3
    OPC_6502_C4,
    OPC_6502_C5,
    OPC_6502_C6,
    OPC_Illegal,        // $C7: SMB4 currently unsupported
    OPC_6502_C8,
    OPC_6502_C9,
    OPC_6502_CA,
    OPC_Illegal,        // $CB: WAI currently unsupported
    OPC_6502_CC,
    OPC_6502_CD,
    OPC_6502_CE,
    OPC_Illegal,        // $CF: BBS4 currently unsupported
    OPC_6502_D0,
    OPC_6502_D1,
    OPC_65SC02_D2,
    OPC_65C02_NOP11,    // $D3
    OPC_65C02_NOP24,    // $D4
    OPC_6502_D5,
    OPC_6502_D6,
    OPC_Illegal,        // $D7: SMB5 currently unsupported
    OPC_6502_D8,
    OPC_6502_D9,
    OPC_65SC02_DA,
    OPC_Illegal,        // $DB: STP currently unsupported
    OPC_65C02_NOP34,    // $DC
    OPC_6502_DD,
    OPC_6502_DE,
    OPC_Illegal,        // $DF: BBS5 currently unsupported
    OPC_6502_E0,
    OPC_6502_E1,
    OPC_65C02_NOP22,    // $E2
    OPC_65C02_NOP11,    // $E3
    OPC_6502_E4,
    OPC_6502_E5,
    OPC_6502_E6,
    OPC_Illegal,        // $E7: SMB6 currently unsupported
    OPC_6502_E8,
    OPC_6502_E9,
    OPC_6502_EA,
    OPC_65C02_NOP11,    // $EB
    OPC_6502_EC,
    OPC_6502_ED,
    OPC_6502_EE,
    OPC_Illegal,        // $EF: BBS6 currently unsupported
    OPC_6502_F0,
    OPC_6502_F1,
    OPC_65SC02_F2,
    OPC_65C02_NOP11,    // $F3
    OPC_65C02_NOP24,    // $F4
    OPC_6502_F5,
    OPC_6502_F6,
    OPC_Illegal,        // $F7: SMB7 currently unsupported
    OPC_6502_F8,
    OPC_6502_F9,
    OPC_65SC02_FA,
    OPC_65C02_NOP11,    // $FB
    OPC_65C02_NOP34,    // $FC
    OPC_6502_FD,
    OPC_6502_FE,
    OPC_Illegal,        // $FF: BBS7 currently unsupported
};



/* Tables with opcode handlers */
static const OPFunc* Handlers[2] = {OP6502Table, OP65C02Table};



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void IRQRequest (void)
/* Generate an IRQ */
{
    /* Remember the request */
    HaveIRQRequest = 1;
}



void NMIRequest (void)
/* Generate an NMI */
{
    /* Remember the request */
    HaveNMIRequest = 1;
}



void Reset (void)
/* Generate a CPU RESET */
{
    /* Reset the CPU */
    HaveIRQRequest = 0;
    HaveNMIRequest = 0;

    /* Bits 5 and 4 aren't used, and always are 1! */
    Regs.SR = 0x30;
    Regs.PC = MemReadWord (0xFFFC);
}



unsigned ExecuteInsn (void)
/* Execute one CPU instruction */
{
    /* If we have an NMI request, handle it */
    if (HaveNMIRequest) {

        HaveNMIRequest = 0;
        PUSH (PCH);
        PUSH (PCL);
        PUSH (Regs.SR & ~BF);
        SET_IF (1);
        if (CPU != CPU_6502)
        {
            SET_DF (0);
        }
        Regs.PC = MemReadWord (0xFFFA);
        Cycles = 7;

    } else if (HaveIRQRequest && GET_IF () == 0) {

        HaveIRQRequest = 0;
        PUSH (PCH);
        PUSH (PCL);
        PUSH (Regs.SR & ~BF);
        SET_IF (1);
        if (CPU != CPU_6502)
        {
            SET_DF (0);
        }
        Regs.PC = MemReadWord (0xFFFE);
        Cycles = 7;

    } else {

        /* Normal instruction - read the next opcode */
        unsigned char OPC = MemReadByte (Regs.PC);

        /* Execute it */
        Handlers[CPU][OPC] ();
    }

    /* Return the number of clock cycles needed by this insn */
    return Cycles;
}
