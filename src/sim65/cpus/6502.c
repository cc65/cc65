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



#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

/* common */
#include "abend.h"
#include "attrib.h"
#include "print.h"
#include "strbuf.h"

/* sim65 */
#include "cpuif.h"



/*****************************************************************************/
/*                                 Forwards                                  */
/*****************************************************************************/



static void Init (const SimData* D);
/* Initialize the module */

static void* Create6502Instance (void* CfgInfo);
/* Create an instance of a 6502 CPU */

static void* Create65C02Instance (void* CfgInfo);
/* Create an instance of a 65C02 CPU */

static void DestroyInstance (void* Instance);
/* Destroy an instance of a CPU */

static void IRQRequest (void* Data);
/* Generate an IRQ */

static void NMIRequest (void* Data);
/* Generate an NMI */

static void Reset (void* Data);
/* Generate a CPU RESET */

static unsigned ExecuteInsn (void* Data);
/* Execute one CPU instruction */

static unsigned long GetCycles (void* Data);
/* Return the total number of cycles executed */



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* The SimData pointer we get when InitChip is called */
static const SimData* Sim;

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

/* Type of an opcode handler function */
struct CPUInstance;
typedef void (*OPFunc) (struct CPUInstance* D);



/* Control data passed to the main program */
static const struct CPUData CData[] = {
    {
        "6502",                 /* Name of the CPU */
        CPUDATA_VER_MAJOR,      /* Version information */
        CPUDATA_VER_MINOR,

        /* -- Exported functions -- */
        Init,
        Create6502Instance,
        DestroyInstance,
        Reset,
        IRQRequest,
        NMIRequest,
        ExecuteInsn,
        GetCycles
    },{
        "65C02",                /* Name of the CPU */
        CPUDATA_VER_MAJOR,      /* Version information */
        CPUDATA_VER_MINOR,

        /* -- Exported functions -- */
        Init,
        Create65C02Instance,
        DestroyInstance,
        Reset,
        IRQRequest,
        NMIRequest,
        ExecuteInsn,
        GetCycles
    },
};


/* CPU instance data */
typedef struct CPUInstance CPUInstance;
struct CPUInstance {
    const OPFunc*       Handlers;       /* Table with opcode handlers */

    CPURegs             Regs;           /* The CPU registers */

    unsigned            Cycles;         /* Cycles for the current insn */
    unsigned long       TotalCycles;    /* Total number of CPU cycles exec'd */

    unsigned            StackPage;      /* Allows to move the stack page */

    unsigned            HaveNMIRequest; /* NMI request active */
    unsigned            HaveIRQRequest; /* IRQ request active */
    unsigned            CPUHalted;      /* CPU is halted */
};


/*****************************************************************************/
/*                        Helper functions and macros                        */
/*****************************************************************************/



/* Return the flags as a boolean value (0/1) */
#define GET_CF()        ((D->Regs.SR & CF) != 0)
#define GET_ZF()        ((D->Regs.SR & ZF) != 0)
#define GET_IF()        ((D->Regs.SR & IF) != 0)
#define GET_DF()        ((D->Regs.SR & DF) != 0)
#define GET_BF()        ((D->Regs.SR & BF) != 0)
#define GET_OF()        ((D->Regs.SR & OF) != 0)
#define GET_SF()        ((D->Regs.SR & SF) != 0)

/* Set the flags. The parameter is a boolean flag that says if the flag should be
 * set or reset.
 */
#define SET_CF(f)       do { if (f) { D->Regs.SR |= CF; } else { D->Regs.SR &= ~CF; } } while (0)
#define SET_ZF(f)       do { if (f) { D->Regs.SR |= ZF; } else { D->Regs.SR &= ~ZF; } } while (0)
#define SET_IF(f)       do { if (f) { D->Regs.SR |= IF; } else { D->Regs.SR &= ~IF; } } while (0)
#define SET_DF(f)       do { if (f) { D->Regs.SR |= DF; } else { D->Regs.SR &= ~DF; } } while (0)
#define SET_BF(f)       do { if (f) { D->Regs.SR |= BF; } else { D->Regs.SR &= ~BF; } } while (0)
#define SET_OF(f)       do { if (f) { D->Regs.SR |= OF; } else { D->Regs.SR &= ~OF; } } while (0)
#define SET_SF(f)       do { if (f) { D->Regs.SR |= SF; } else { D->Regs.SR &= ~SF; } } while (0)

/* Special test and set macros. The meaning of the parameter depends on the
 * actual flag that should be set or reset.
 */
#define TEST_ZF(v)      SET_ZF (((v) & 0xFF) == 0)
#define TEST_SF(v)      SET_SF (((v) & 0x80) != 0)
#define TEST_CF(v)      SET_CF (((v) & 0xFF00) != 0)

/* Program counter halves */
#define PCL             (D->Regs.PC & 0xFF)
#define PCH             ((D->Regs.PC >> 8) & 0xFF)

/* Stack operations */
#define PUSH(Val)       MemWriteByte (D->StackPage + D->Regs.SP--, Val)
#define POP()           MemReadByte (D->StackPage + ++D->Regs.SP)

/* Test for page cross */
#define PAGE_CROSS(addr,offs)   ((((addr) & 0xFF) + offs) >= 0x100)

/* #imm */
#define AC_OP_IMM(op)                                           \
    D->Cycles = 2;                                              \
    D->Regs.AC = D->Regs.AC op MemReadByte (D->Regs.PC+1);      \
    TEST_ZF (D->Regs.AC);                                       \
    TEST_SF (D->Regs.AC);                                       \
    D->Regs.PC += 2

/* zp */
#define AC_OP_ZP(op)                                                    \
    D->Cycles = 3;                                                      \
    D->Regs.AC = D->Regs.AC op MemReadByte (MemReadByte (D->Regs.PC+1));\
    TEST_ZF (D->Regs.AC);                                               \
    TEST_SF (D->Regs.AC);                                               \
    D->Regs.PC += 2

/* zp,x */
#define AC_OP_ZPX(op)                                           \
    unsigned char ZPAddr;                                       \
    D->Cycles = 4;                                              \
    ZPAddr = MemReadByte (D->Regs.PC+1) + D->Regs.XR;           \
    D->Regs.AC = D->Regs.AC op MemReadByte (ZPAddr);            \
    TEST_ZF (D->Regs.AC);                                       \
    TEST_SF (D->Regs.AC);                                       \
    D->Regs.PC += 2

/* zp,y */
#define AC_OP_ZPY(op)                                           \
    unsigned char ZPAddr;                                       \
    D->Cycles = 4;                                              \
    ZPAddr = MemReadByte (D->Regs.PC+1) + D->Regs.YR;           \
    D->Regs.AC = D->Regs.AC op MemReadByte (ZPAddr);            \
    TEST_ZF (D->Regs.AC);                                       \
    TEST_SF (D->Regs.AC);                                       \
    D->Regs.PC += 2

/* abs */
#define AC_OP_ABS(op)                                           \
    unsigned Addr;                                              \
    D->Cycles = 4;                                              \
    Addr = MemReadWord (D->Regs.PC+1);                          \
    D->Regs.AC = D->Regs.AC op MemReadByte (Addr);              \
    TEST_ZF (D->Regs.AC);                                       \
    TEST_SF (D->Regs.AC);                                       \
    D->Regs.PC += 3

/* abs,x */
#define AC_OP_ABSX(op)                                          \
    unsigned Addr;                                              \
    D->Cycles = 4;                                              \
    Addr = MemReadWord (D->Regs.PC+1);                          \
    if (PAGE_CROSS (Addr, D->Regs.XR)) {                        \
        ++D->Cycles;                                            \
    }                                                           \
    D->Regs.AC = D->Regs.AC op MemReadByte (Addr + D->Regs.XR); \
    TEST_ZF (D->Regs.AC);                                       \
    TEST_SF (D->Regs.AC);                                       \
    D->Regs.PC += 3

/* abs,y */
#define AC_OP_ABSY(op)                                          \
    unsigned Addr;                                              \
    D->Cycles = 4;                                              \
    Addr = MemReadWord (D->Regs.PC+1);                          \
    if (PAGE_CROSS (Addr, D->Regs.YR)) {                        \
        ++D->Cycles;                                            \
    }                                                           \
    D->Regs.AC = D->Regs.AC op MemReadByte (Addr + D->Regs.YR); \
    TEST_ZF (D->Regs.AC);                                       \
    TEST_SF (D->Regs.AC);                                       \
    D->Regs.PC += 3

/* (zp,x) */
#define AC_OP_ZPXIND(op)                                        \
    unsigned char ZPAddr;                                       \
    unsigned Addr;                                              \
    D->Cycles = 6;                                              \
    ZPAddr = MemReadByte (D->Regs.PC+1) + D->Regs.XR;           \
    Addr = MemReadZPWord (ZPAddr);                              \
    D->Regs.AC = D->Regs.AC op MemReadByte (Addr);              \
    TEST_ZF (D->Regs.AC);                                       \
    TEST_SF (D->Regs.AC);                                       \
    D->Regs.PC += 2

/* (zp),y */
#define AC_OP_ZPINDY(op)                                        \
    unsigned char ZPAddr;                                       \
    unsigned Addr;                                              \
    D->Cycles = 5;                                              \
    ZPAddr = MemReadByte (D->Regs.PC+1);                        \
    Addr = MemReadZPWord (ZPAddr) + D->Regs.YR;                 \
    D->Regs.AC = D->Regs.AC op MemReadByte (Addr);              \
    TEST_ZF (D->Regs.AC);                                       \
    TEST_SF (D->Regs.AC);                                       \
    D->Regs.PC += 2

/* ADC */
#define ADC(v)                                                  \
    do {                                                        \
        unsigned old = D->Regs.AC;                              \
        unsigned rhs = (v & 0xFF);                              \
        if (GET_DF ()) {                                        \
            unsigned lo;                                        \
            int res;                                            \
            lo = (old & 0x0F) + (rhs & 0x0F) + GET_CF ();       \
            if (lo >= 0x0A) {                                   \
                lo = ((lo + 0x06) & 0x0F) + 0x10;               \
            }                                                   \
            D->Regs.AC = (old & 0xF0) + (rhs & 0xF0) + lo;      \
            res = (signed char)(old & 0xF0) +                   \
                  (signed char)(rhs & 0xF0) +                   \
                  (signed char)lo;                              \
            TEST_ZF (old + rhs + GET_CF ());                    \
            TEST_SF (D->Regs.AC);                               \
            if (D->Regs.AC >= 0xA0) {                           \
                D->Regs.AC += 0x60;                             \
            }                                                   \
            TEST_CF (D->Regs.AC);                               \
            SET_OF ((res < -128) || (res > 127));               \
        } else {                                                \
            D->Regs.AC += rhs + GET_CF ();                      \
            TEST_ZF (D->Regs.AC);                               \
            TEST_SF (D->Regs.AC);                               \
            TEST_CF (D->Regs.AC);                               \
            SET_OF (!((old ^ rhs) & 0x80) &&                    \
                    ((old ^ D->Regs.AC) & 0x80));               \
            D->Regs.AC &= 0xFF;                                 \
        }                                                       \
    } while (0)

/* branches */
#define BRANCH(cond)                                            \
    D->Cycles = 2;                                              \
    if (cond) {                                                 \
        signed char Offs;                                       \
        unsigned char OldPCH;                                   \
        ++D->Cycles;                                            \
        Offs = (signed char) MemReadByte (D->Regs.PC+1);        \
        OldPCH = PCH;                                           \
        D->Regs.PC += 2 + (int) Offs;                           \
        if (PCH != OldPCH) {                                    \
            ++D->Cycles;                                        \
        }                                                       \
    } else {                                                    \
        D->Regs.PC += 2;                                        \
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
        unsigned old = D->Regs.AC;                              \
        unsigned rhs = (v & 0xFF);                              \
        if (GET_DF ()) {                                        \
            unsigned lo;                                        \
            int res;                                            \
            lo = (old & 0x0F) - (rhs & 0x0F) + GET_CF () - 1;   \
            if (lo & 0x80) {                                    \
                lo = ((lo - 0x06) & 0x0F) - 0x10;               \
            }                                                   \
            D->Regs.AC = (old & 0xF0) - (rhs & 0xF0) + lo;      \
            if (D->Regs.AC & 0x80) {                            \
                D->Regs.AC -= 0x60;                             \
            }                                                   \
            res = D->Regs.AC - rhs + (!GET_CF ());              \
            TEST_ZF (res);                                      \
            TEST_SF (res);                                      \
            SET_CF (res <= 0xFF);                               \
            SET_OF (((old^rhs) & (old^res) & 0x80));            \
        } else {                                                \
            D->Regs.AC -= rhs - (!GET_CF ());                   \
            TEST_ZF (D->Regs.AC);                               \
            TEST_SF (D->Regs.AC);                               \
            SET_CF (D->Regs.AC <= 0xFF);                        \
            SET_OF (((old^rhs) & (old^D->Regs.AC) & 0x80));     \
            D->Regs.AC &= 0xFF;                                 \
        }                                                       \
    } while (0)



static unsigned char MemReadByte (unsigned Addr)
/* Read a byte from a memory location */
{
    return Sim->Read (Addr);
}



static unsigned MemReadWord (unsigned Addr)
/* Read a word from a memory location */
{
    unsigned W = Sim->Read (Addr);
    return (W | (Sim->Read (Addr + 1) << 8));
}



static unsigned MemReadZPWord (unsigned Addr)
/* Read a word from the zero page. This function differs from ReadMemW in that
 * the read will always be in the zero page, even in case of an address
 * overflow.
 */
{
    unsigned W = Sim->Read (Addr & 0xFF);
    return (W | (Sim->Read ((Addr + 1) & 0xFF) << 8));
}



static void MemWriteByte (unsigned Addr, unsigned char Val)
/* Write a byte to a memory location */
{
    Sim->Write (Addr, Val);
}



/*****************************************************************************/
/*                                CPUInstance                                */
/*****************************************************************************/



static CPUInstance* NewCPUInstance (const OPFunc FuncTable[256], void* CfgInfo)
/* Create and return a new CPU instance struct */
{
    /* Allocate memory */
    CPUInstance* D = Sim->Malloc (sizeof (*D));

    /* Initialize the fields */
    D->Handlers         = FuncTable;    /* Table with opcode handlers */
    D->StackPage        = 0x100;        /* Allows to move the stack page */
    D->HaveNMIRequest   = 0;            /* NMI request active */
    D->HaveIRQRequest   = 0;            /* IRQ request active */
    D->CPUHalted        = 0;            /* CPU is halted */

    /* Return the new struct */
    return D;
}



static void DeleteCPUInstance (CPUInstance* Instance)
/* Delete a CPU instance */
{
    /* Just free the memory */
    Sim->Free (Instance);
}



/*****************************************************************************/
/*                         Opcode handling functions                         */
/*****************************************************************************/



static void OPC_Illegal (CPUInstance* D)
{
    Sim->Warning ("Illegal opcode $%02X at address $%04X\n",
                  MemReadByte (D->Regs.PC), D->Regs.PC);
}



static void OPC_6502_00 (CPUInstance* D)
/* Opcode $00: BRK */
{
    D->Cycles = 7;
    D->Regs.PC += 2;
    SET_BF (1);
    PUSH (PCH);
    PUSH (PCL);
    PUSH (D->Regs.SR);
    SET_IF (1);
    D->Regs.PC = MemReadWord (0xFFFE);
}



static void OPC_6502_01 (CPUInstance* D)
/* Opcode $01: ORA (ind,x) */
{
    AC_OP_ZPXIND (|);
}



static void OPC_6502_05 (CPUInstance* D)
/* Opcode $05: ORA zp */
{
    AC_OP_ZP (|);
}



static void OPC_6502_06 (CPUInstance* D)
/* Opcode $06: ASL zp */
{
    unsigned char ZPAddr;
    unsigned Val;
    D->Cycles = 5;
    ZPAddr = MemReadByte (D->Regs.PC+1);
    Val    = MemReadByte (ZPAddr) << 1;
    MemWriteByte (ZPAddr, (unsigned char) Val);
    TEST_ZF (Val & 0xFF);
    TEST_SF (Val);
    SET_CF (Val & 0x100);
    D->Regs.PC += 2;
}



static void OPC_6502_08 (CPUInstance* D)
/* Opcode $08: PHP */
{
    D->Cycles = 3;
    PUSH (D->Regs.SR & ~BF);
    D->Regs.PC += 1;
}



static void OPC_6502_09 (CPUInstance* D)
/* Opcode $09: ORA #imm */
{
    AC_OP_IMM (|);
}



static void OPC_6502_0A (CPUInstance* D)
/* Opcode $0A: ASL a */
{
    D->Cycles = 2;
    D->Regs.AC <<= 1;
    TEST_ZF (D->Regs.AC & 0xFF);
    TEST_SF (D->Regs.AC);
    SET_CF (D->Regs.AC & 0x100);
    D->Regs.AC &= 0xFF;
    D->Regs.PC += 1;
}



static void OPC_6502_0D (CPUInstance* D)
/* Opcode $0D: ORA abs */
{
    AC_OP_ABS (|);
}



static void OPC_6502_0E (CPUInstance* D)
/* Opcode $0E: ALS abs */
{
    unsigned Addr;
    unsigned Val;
    D->Cycles = 6;
    Addr = MemReadWord (D->Regs.PC+1);
    Val  = MemReadByte (Addr) << 1;
    MemWriteByte (Addr, (unsigned char) Val);
    TEST_ZF (Val & 0xFF);
    TEST_SF (Val);
    SET_CF (Val & 0x100);
    D->Regs.PC += 3;
}



static void OPC_6502_10 (CPUInstance* D)
/* Opcode $10: BPL */
{
    BRANCH (!GET_SF ());
}



static void OPC_6502_11 (CPUInstance* D)
/* Opcode $11: ORA (zp),y */
{
    AC_OP_ZPINDY (|);
}



static void OPC_6502_15 (CPUInstance* D)
/* Opcode $15: ORA zp,x */
{
   AC_OP_ZPX (|);
}



static void OPC_6502_16 (CPUInstance* D)
/* Opcode $16: ASL zp,x */
{
    unsigned char ZPAddr;
    unsigned Val;
    D->Cycles = 6;
    ZPAddr = MemReadByte (D->Regs.PC+1) + D->Regs.XR;
    Val    = MemReadByte (ZPAddr) << 1;
    MemWriteByte (ZPAddr, (unsigned char) Val);
    TEST_ZF (Val & 0xFF);
    TEST_SF (Val);
    SET_CF (Val & 0x100);
    D->Regs.PC += 2;
}



static void OPC_6502_18 (CPUInstance* D)
/* Opcode $18: CLC */
{
    D->Cycles = 2;
    SET_CF (0);
    D->Regs.PC += 1;
}



static void OPC_6502_19 (CPUInstance* D)
/* Opcode $19: ORA abs,y */
{
    AC_OP_ABSY (|);
}



static void OPC_6502_1D (CPUInstance* D)
/* Opcode $1D: ORA abs,x */
{
    AC_OP_ABSX (|);
}



static void OPC_6502_1E (CPUInstance* D)
/* Opcode $1E: ASL abs,x */
{
    unsigned Addr;
    unsigned Val;
    D->Cycles = 7;
    Addr = MemReadWord (D->Regs.PC+1) + D->Regs.XR;
    Val  = MemReadByte (Addr) << 1;
    MemWriteByte (Addr, (unsigned char) Val);
    TEST_ZF (Val & 0xFF);
    TEST_SF (Val);
    SET_CF (Val & 0x100);
    D->Regs.PC += 3;
}



static void OPC_6502_20 (CPUInstance* D)
/* Opcode $20: JSR */
{
    unsigned Addr;
    D->Cycles = 6;
    Addr   = MemReadWord (D->Regs.PC+1);
    D->Regs.PC += 2;
    PUSH (PCH);
    PUSH (PCL);
    D->Regs.PC = Addr;
}



static void OPC_6502_21 (CPUInstance* D)
/* Opcode $21: AND (zp,x) */
{
    AC_OP_ZPXIND (&);
}



static void OPC_6502_24 (CPUInstance* D)
/* Opcode $24: BIT zp */
{
    unsigned char ZPAddr;
    unsigned char Val;
    D->Cycles = 3;
    ZPAddr = MemReadByte (D->Regs.PC+1);
    Val    = MemReadByte (ZPAddr);
    SET_SF (Val & 0x80);
    SET_OF (Val & 0x40);
    SET_ZF ((Val & D->Regs.AC) == 0);
    D->Regs.PC += 2;
}



static void OPC_6502_25 (CPUInstance* D)
/* Opcode $25: AND zp */
{
    AC_OP_ZP (&);
}



static void OPC_6502_26 (CPUInstance* D)
/* Opcode $26: ROL zp */
{
    unsigned char ZPAddr;
    unsigned Val;
    D->Cycles = 5;
    ZPAddr = MemReadByte (D->Regs.PC+1);
    Val    = MemReadByte (ZPAddr);
    ROL (Val);
    MemWriteByte (ZPAddr, Val);
    D->Regs.PC += 2;
}



static void OPC_6502_28 (CPUInstance* D)
/* Opcode $28: PLP */
{
    D->Cycles = 4;
    D->Regs.SR = (POP () & ~BF);
    D->Regs.PC += 1;
}



static void OPC_6502_29 (CPUInstance* D)
/* Opcode $29: AND #imm */
{
    AC_OP_IMM (&);
}



static void OPC_6502_2A (CPUInstance* D)
/* Opcode $2A: ROL a */
{
    D->Cycles = 2;
    ROL (D->Regs.AC);
    D->Regs.AC &= 0xFF;
    D->Regs.PC += 1;
}



static void OPC_6502_2C (CPUInstance* D)
/* Opcode $2C: BIT abs */
{
    unsigned Addr;
    unsigned char Val;
    D->Cycles = 4;
    Addr = MemReadByte (D->Regs.PC+1);
    Val  = MemReadByte (Addr);
    SET_SF (Val & 0x80);
    SET_OF (Val & 0x40);
    SET_ZF ((Val & D->Regs.AC) == 0);
    D->Regs.PC += 3;
}



static void OPC_6502_2D (CPUInstance* D)
/* Opcode $2D: AND abs */
{
    AC_OP_ABS (&);
}



static void OPC_6502_2E (CPUInstance* D)
/* Opcode $2E: ROL abs */
{
    unsigned Addr;
    unsigned Val;
    D->Cycles = 6;
    Addr = MemReadWord (D->Regs.PC+1);
    Val  = MemReadByte (Addr);
    ROL (Val);
    MemWriteByte (Addr, Val);
    D->Regs.PC += 3;
}



static void OPC_6502_30 (CPUInstance* D)
/* Opcode $30: BMI */
{
    BRANCH (GET_SF ());
}



static void OPC_6502_31 (CPUInstance* D)
/* Opcode $31: AND (zp),y */
{
    AC_OP_ZPINDY (&);
}



static void OPC_6502_35 (CPUInstance* D)
/* Opcode $35: AND zp,x */
{
    AC_OP_ZPX (&);
}



static void OPC_6502_36 (CPUInstance* D)
/* Opcode $36: ROL zp,x */
{
    unsigned char ZPAddr;
    unsigned Val;
    D->Cycles = 6;
    ZPAddr = MemReadByte (D->Regs.PC+1) + D->Regs.XR;
    Val    = MemReadByte (ZPAddr);
    ROL (Val);
    MemWriteByte (ZPAddr, Val);
    D->Regs.PC += 2;
}



static void OPC_6502_38 (CPUInstance* D)
/* Opcode $38: SEC */
{
    D->Cycles = 2;
    SET_CF (1);
    D->Regs.PC += 1;
}



static void OPC_6502_39 (CPUInstance* D)
/* Opcode $39: AND abs,y */
{
    AC_OP_ABSY (&);
}



static void OPC_6502_3D (CPUInstance* D)
/* Opcode $3D: AND abs,x */
{
    AC_OP_ABSX (&);
}



static void OPC_6502_3E (CPUInstance* D)
/* Opcode $3E: ROL abs,x */
{
    unsigned Addr;
    unsigned Val;
    D->Cycles = 7;
    Addr = MemReadWord (D->Regs.PC+1) + D->Regs.XR;
    Val  = MemReadByte (Addr);
    ROL (Val);
    MemWriteByte (Addr, Val);
    D->Regs.PC += 2;
}



static void OPC_6502_40 (CPUInstance* D)
/* Opcode $40: RTI */
{
    D->Cycles = 6;
    D->Regs.SR = POP ();
    D->Regs.PC = POP ();                /* PCL */
    D->Regs.PC |= (POP () << 8);        /* PCH */
}



static void OPC_6502_41 (CPUInstance* D)
/* Opcode $41: EOR (zp,x) */
{
    AC_OP_ZPXIND (^);
}



static void OPC_6502_45 (CPUInstance* D)
/* Opcode $45: EOR zp */
{
    AC_OP_ZP (^);
}



static void OPC_6502_46 (CPUInstance* D)
/* Opcode $46: LSR zp */
{
    unsigned char ZPAddr;
    unsigned char Val;
    D->Cycles = 5;
    ZPAddr = MemReadByte (D->Regs.PC+1);
    Val    = MemReadByte (ZPAddr);
    SET_CF (Val & 0x01);
    Val >>= 1;
    MemWriteByte (ZPAddr, Val);
    TEST_ZF (Val);
    TEST_SF (Val);
    D->Regs.PC += 2;
}



static void OPC_6502_48 (CPUInstance* D)
/* Opcode $48: PHA */
{
    D->Cycles = 3;
    PUSH (D->Regs.AC);
    D->Regs.PC += 1;
}



static void OPC_6502_49 (CPUInstance* D)
/* Opcode $49: EOR #imm */
{
    AC_OP_IMM (^);
}



static void OPC_6502_4A (CPUInstance* D)
/* Opcode $4A: LSR a */
{
    D->Cycles = 2;
    SET_CF (D->Regs.AC & 0x01);
    D->Regs.AC >>= 1;
    TEST_ZF (D->Regs.AC);
    TEST_SF (D->Regs.AC);
    D->Regs.PC += 1;
}



static void OPC_6502_4C (CPUInstance* D)
/* Opcode $4C: JMP abs */
{
    D->Cycles = 3;
    D->Regs.PC = MemReadWord (D->Regs.PC+1);
}



static void OPC_6502_4D (CPUInstance* D)
/* Opcode $4D: EOR abs */
{
    AC_OP_ABS (^);
}



static void OPC_6502_4E (CPUInstance* D)
/* Opcode $4E: LSR abs */
{
    unsigned Addr;
    unsigned char Val;
    D->Cycles = 6;
    Addr = MemReadWord (D->Regs.PC+1);
    Val  = MemReadByte (Addr);
    SET_CF (Val & 0x01);
    Val >>= 1;
    MemWriteByte (Addr, Val);
    TEST_ZF (Val);
    TEST_SF (Val);
    D->Regs.PC += 3;
}



static void OPC_6502_50 (CPUInstance* D)
/* Opcode $50: BVC */
{
    BRANCH (!GET_OF ());
}



static void OPC_6502_51 (CPUInstance* D)
/* Opcode $51: EOR (zp),y */
{
    AC_OP_ZPINDY (^);
}



static void OPC_6502_55 (CPUInstance* D)
/* Opcode $55: EOR zp,x */
{
    AC_OP_ZPX (^);
}



static void OPC_6502_56 (CPUInstance* D)
/* Opcode $56: LSR zp,x */
{
    unsigned char ZPAddr;
    unsigned char Val;
    D->Cycles = 6;
    ZPAddr = MemReadByte (D->Regs.PC+1) + D->Regs.XR;
    Val    = MemReadByte (ZPAddr);
    SET_CF (Val & 0x01);
    Val >>= 1;
    MemWriteByte (ZPAddr, Val);
    TEST_ZF (Val);
    TEST_SF (Val);
    D->Regs.PC += 2;
}



static void OPC_6502_58 (CPUInstance* D)
/* Opcode $58: CLI */
{
    D->Cycles = 2;
    SET_IF (0);
    D->Regs.PC += 1;
}



static void OPC_6502_59 (CPUInstance* D)
/* Opcode $59: EOR abs,y */
{
    AC_OP_ABSY (^);
}



static void OPC_6502_5D (CPUInstance* D)
/* Opcode $5D: EOR abs,x */
{
    AC_OP_ABSX (^);
}



static void OPC_6502_5E (CPUInstance* D)
/* Opcode $5E: LSR abs,x */
{
    unsigned Addr;
    unsigned char Val;
    D->Cycles = 7;
    Addr = MemReadWord (D->Regs.PC+1) + D->Regs.XR;
    Val  = MemReadByte (Addr);
    SET_CF (Val & 0x01);
    Val >>= 1;
    MemWriteByte (Addr, Val);
    TEST_ZF (Val);
    TEST_SF (Val);
    D->Regs.PC += 3;
}



static void OPC_6502_60 (CPUInstance* D)
/* Opcode $60: RTS */
{
    D->Cycles = 6;
    D->Regs.PC = POP ();                /* PCL */
    D->Regs.PC |= (POP () << 8);        /* PCH */
    D->Regs.PC += 1;
}



static void OPC_6502_61 (CPUInstance* D)
/* Opcode $61: ADC (zp,x) */
{
    unsigned char ZPAddr;
    unsigned Addr;
    D->Cycles = 6;
    ZPAddr = MemReadByte (D->Regs.PC+1) + D->Regs.XR;
    Addr   = MemReadZPWord (ZPAddr);
    ADC (MemReadByte (Addr));
    D->Regs.PC += 2;
}



static void OPC_6502_65 (CPUInstance* D)
/* Opcode $65: ADC zp */
{
    unsigned char ZPAddr;
    D->Cycles = 3;
    ZPAddr = MemReadByte (D->Regs.PC+1);
    ADC (MemReadByte (ZPAddr));
    D->Regs.PC += 2;
}



static void OPC_6502_66 (CPUInstance* D)
/* Opcode $66: ROR zp */
{
    unsigned char ZPAddr;
    unsigned Val;
    D->Cycles = 5;
    ZPAddr = MemReadByte (D->Regs.PC+1);
    Val    = MemReadByte (ZPAddr);
    ROR (Val);
    MemWriteByte (ZPAddr, Val);
    D->Regs.PC += 2;
}



static void OPC_6502_68 (CPUInstance* D)
/* Opcode $68: PLA */
{
    D->Cycles = 4;
    D->Regs.AC = POP ();
    TEST_ZF (D->Regs.AC);
    TEST_SF (D->Regs.AC);
    D->Regs.PC += 1;
}



static void OPC_6502_69 (CPUInstance* D)
/* Opcode $69: ADC #imm */
{
    D->Cycles = 2;
    ADC (MemReadByte (D->Regs.PC+1));
    D->Regs.PC += 2;
}



static void OPC_6502_6A (CPUInstance* D)
/* Opcode $6A: ROR a */
{
    D->Cycles = 2;
    ROR (D->Regs.AC);
    D->Regs.PC += 1;
}



static void OPC_6502_6C (CPUInstance* D)
/* Opcode $6C: JMP (ind) */
{
    unsigned PC, Lo, Hi;
    D->Cycles = 5;
    PC = D->Regs.PC;
    Lo = MemReadWord (PC+1);

    /* Emulate the 6502 bug */
    D->Regs.PC = MemReadByte (Lo);
    Hi = (Lo & 0xFF00) | ((Lo + 1) & 0xFF);
    D->Regs.PC |= (MemReadByte (Hi) << 8);

    /* Output a warning if the bug is triggered */
    if (Hi != Lo + 1) {
        Sim->Warning ("6502 indirect jump bug triggered at $%04X, ind addr = $%04X",
                      PC, Lo);
    }
}



static void OPC_65C02_6C (CPUInstance* D)
/* Opcode $6C: JMP (ind) */
{
    /* 6502 bug fixed here */
    D->Cycles = 5;
    D->Regs.PC = MemReadWord (MemReadWord (D->Regs.PC+1));
}



static void OPC_6502_6D (CPUInstance* D)
/* Opcode $6D: ADC abs */
{
    unsigned Addr;
    D->Cycles = 4;
    Addr   = MemReadWord (D->Regs.PC+1);
    ADC (MemReadByte (Addr));
    D->Regs.PC += 3;
}



static void OPC_6502_6E (CPUInstance* D)
/* Opcode $6E: ROR abs */
{
    unsigned Addr;
    unsigned Val;
    D->Cycles = 6;
    Addr = MemReadWord (D->Regs.PC+1);
    Val  = MemReadByte (Addr);
    ROR (Val);
    MemWriteByte (Addr, Val);
    D->Regs.PC += 3;
}



static void OPC_6502_70 (CPUInstance* D)
/* Opcode $70: BVS */
{
    BRANCH (GET_OF ());
}



static void OPC_6502_71 (CPUInstance* D)
/* Opcode $71: ADC (zp),y */
{
    unsigned char ZPAddr;
    unsigned Addr;
    D->Cycles = 5;
    ZPAddr = MemReadByte (D->Regs.PC+1);
    Addr   = MemReadZPWord (ZPAddr);
    if (PAGE_CROSS (Addr, D->Regs.YR)) {
        ++D->Cycles;
    }
    ADC (MemReadByte (Addr + D->Regs.YR));
    D->Regs.PC += 2;
}



static void OPC_6502_75 (CPUInstance* D)
/* Opcode $75: ADC zp,x */
{
    unsigned char ZPAddr;
    D->Cycles = 4;
    ZPAddr = MemReadByte (D->Regs.PC+1) + D->Regs.XR;
    ADC (MemReadByte (ZPAddr));
    D->Regs.PC += 2;
}



static void OPC_6502_76 (CPUInstance* D)
/* Opcode $76: ROR zp,x */
{
    unsigned char ZPAddr;
    unsigned Val;
    D->Cycles = 6;
    ZPAddr = MemReadByte (D->Regs.PC+1) + D->Regs.XR;
    Val    = MemReadByte (ZPAddr);
    ROR (Val);
    MemWriteByte (ZPAddr, Val);
    D->Regs.PC += 2;
}



static void OPC_6502_78 (CPUInstance* D)
/* Opcode $78: SEI */
{
    D->Cycles = 2;
    SET_IF (1);
    D->Regs.PC += 1;
}



static void OPC_6502_79 (CPUInstance* D)
/* Opcode $79: ADC abs,y */
{
    unsigned Addr;
    D->Cycles = 4;
    Addr   = MemReadWord (D->Regs.PC+1);
    if (PAGE_CROSS (Addr, D->Regs.YR)) {
        ++D->Cycles;
    }
    ADC (MemReadByte (Addr + D->Regs.YR));
    D->Regs.PC += 3;
}



static void OPC_6502_7D (CPUInstance* D)
/* Opcode $7D: ADC abs,x */
{
    unsigned Addr;
    D->Cycles = 4;
    Addr   = MemReadWord (D->Regs.PC+1);
    if (PAGE_CROSS (Addr, D->Regs.XR)) {
        ++D->Cycles;
    }
    ADC (MemReadByte (Addr + D->Regs.XR));
    D->Regs.PC += 3;
}



static void OPC_6502_7E (CPUInstance* D)
/* Opcode $7E: ROR abs,x */
{
    unsigned Addr;
    unsigned Val;
    D->Cycles = 7;
    Addr = MemReadByte (D->Regs.PC+1) + D->Regs.XR;
    Val  = MemReadByte (Addr);
    ROR (Val);
    MemWriteByte (Addr, Val);
    D->Regs.PC += 3;
}



static void OPC_6502_81 (CPUInstance* D)
/* Opcode $81: STA (zp,x) */
{
    unsigned char ZPAddr;
    unsigned Addr;
    D->Cycles = 6;
    ZPAddr = MemReadByte (D->Regs.PC+1) + D->Regs.XR;
    Addr   = MemReadZPWord (ZPAddr);
    MemWriteByte (Addr, D->Regs.AC);
    D->Regs.PC += 2;
}



static void OPC_6502_84 (CPUInstance* D)
/* Opcode $84: STY zp */
{
    unsigned char ZPAddr;
    D->Cycles = 3;
    ZPAddr = MemReadByte (D->Regs.PC+1);
    MemWriteByte (ZPAddr, D->Regs.YR);
    D->Regs.PC += 2;
}



static void OPC_6502_85 (CPUInstance* D)
/* Opcode $85: STA zp */
{
    unsigned char ZPAddr;
    D->Cycles = 3;
    ZPAddr = MemReadByte (D->Regs.PC+1);
    MemWriteByte (ZPAddr, D->Regs.AC);
    D->Regs.PC += 2;
}



static void OPC_6502_86 (CPUInstance* D)
/* Opcode $86: STX zp */
{
    unsigned char ZPAddr;
    D->Cycles = 3;
    ZPAddr = MemReadByte (D->Regs.PC+1);
    MemWriteByte (ZPAddr, D->Regs.XR);
    D->Regs.PC += 2;
}



static void OPC_6502_88 (CPUInstance* D)
/* Opcode $88: DEY */
{
    D->Cycles = 2;
    D->Regs.YR = (D->Regs.YR - 1) & 0xFF;
    TEST_ZF (D->Regs.YR);
    TEST_SF (D->Regs.YR);
    D->Regs.PC += 1;
}



static void OPC_6502_8A (CPUInstance* D)
/* Opcode $8A: TXA */
{
    D->Cycles = 2;
    D->Regs.AC = D->Regs.XR;
    TEST_ZF (D->Regs.AC);
    TEST_SF (D->Regs.AC);
    D->Regs.PC += 1;
}



static void OPC_6502_8C (CPUInstance* D)
/* Opcode $8C: STY abs */
{
    unsigned Addr;
    D->Cycles = 4;
    Addr = MemReadWord (D->Regs.PC+1);
    MemWriteByte (Addr, D->Regs.YR);
    D->Regs.PC += 3;
}



static void OPC_6502_8D (CPUInstance* D)
/* Opcode $8D: STA abs */
{
    unsigned Addr;
    D->Cycles = 4;
    Addr = MemReadWord (D->Regs.PC+1);
    MemWriteByte (Addr, D->Regs.AC);
    D->Regs.PC += 3;
}



static void OPC_6502_8E (CPUInstance* D)
/* Opcode $8E: STX abs */
{
    unsigned Addr;
    D->Cycles = 4;
    Addr = MemReadWord (D->Regs.PC+1);
    MemWriteByte (Addr, D->Regs.XR);
    D->Regs.PC += 3;
}



static void OPC_6502_90 (CPUInstance* D)
/* Opcode $90: BCC */
{
    BRANCH (!GET_CF ());
}



static void OPC_6502_91 (CPUInstance* D)
/* Opcode $91: sta (zp),y */
{
    unsigned char ZPAddr;
    unsigned Addr;
    D->Cycles = 6;
    ZPAddr = MemReadByte (D->Regs.PC+1);
    Addr   = MemReadZPWord (ZPAddr) + D->Regs.YR;
    MemWriteByte (Addr, D->Regs.AC);
    D->Regs.PC += 2;
}



static void OPC_6502_94 (CPUInstance* D)
/* Opcode $94: STY zp,x */
{
    unsigned char ZPAddr;
    D->Cycles = 4;
    ZPAddr = MemReadByte (D->Regs.PC+1) + D->Regs.XR;
    MemWriteByte (ZPAddr, D->Regs.YR);
    D->Regs.PC += 2;
}



static void OPC_6502_95 (CPUInstance* D)
/* Opcode $95: STA zp,x */
{
    unsigned char ZPAddr;
    D->Cycles = 4;
    ZPAddr = MemReadByte (D->Regs.PC+1) + D->Regs.XR;
    MemWriteByte (ZPAddr, D->Regs.AC);
    D->Regs.PC += 2;
}



static void OPC_6502_96 (CPUInstance* D)
/* Opcode $96: stx zp,y */
{
    unsigned char ZPAddr;
    D->Cycles = 4;
    ZPAddr = MemReadByte (D->Regs.PC+1) + D->Regs.YR;
    MemWriteByte (ZPAddr, D->Regs.XR);
    D->Regs.PC += 2;
}



static void OPC_6502_98 (CPUInstance* D)
/* Opcode $98: TYA */
{
    D->Cycles = 2;
    D->Regs.AC = D->Regs.YR;
    TEST_ZF (D->Regs.AC);
    TEST_SF (D->Regs.AC);
    D->Regs.PC += 1;
}



static void OPC_6502_99 (CPUInstance* D)
/* Opcode $99: STA abs,y */
{
    unsigned Addr;
    D->Cycles = 5;
    Addr   = MemReadWord (D->Regs.PC+1) + D->Regs.YR;
    MemWriteByte (Addr, D->Regs.AC);
    D->Regs.PC += 3;
}



static void OPC_6502_9A (CPUInstance* D)
/* Opcode $9A: TXS */
{
    D->Cycles = 2;
    D->Regs.SP = D->Regs.XR;
    D->Regs.PC += 1;
}



static void OPC_6502_9D (CPUInstance* D)
/* Opcode $9D: STA abs,x */
{
    unsigned Addr;
    D->Cycles = 5;
    Addr   = MemReadWord (D->Regs.PC+1) + D->Regs.XR;
    MemWriteByte (Addr, D->Regs.AC);
    D->Regs.PC += 3;
}



static void OPC_6502_A0 (CPUInstance* D)
/* Opcode $A0: LDY #imm */
{
    D->Cycles = 2;
    D->Regs.YR = MemReadByte (D->Regs.PC+1);
    TEST_ZF (D->Regs.YR);
    TEST_SF (D->Regs.YR);
    D->Regs.PC += 2;
}



static void OPC_6502_A1 (CPUInstance* D)
/* Opcode $A1: LDA (zp,x) */
{
    unsigned char ZPAddr;
    unsigned Addr;
    D->Cycles = 6;
    ZPAddr = MemReadByte (D->Regs.PC+1) + D->Regs.XR;
    Addr = MemReadZPWord (ZPAddr);
    D->Regs.AC = MemReadByte (Addr);
    TEST_ZF (D->Regs.AC);
    TEST_SF (D->Regs.AC);
    D->Regs.PC += 2;
}



static void OPC_6502_A2 (CPUInstance* D)
/* Opcode $A2: LDX #imm */
{
    D->Cycles = 2;
    D->Regs.XR = MemReadByte (D->Regs.PC+1);
    TEST_ZF (D->Regs.XR);
    TEST_SF (D->Regs.XR);
    D->Regs.PC += 2;
}



static void OPC_6502_A4 (CPUInstance* D)
/* Opcode $A4: LDY zp */
{
    unsigned char ZPAddr;
    D->Cycles = 3;
    ZPAddr = MemReadByte (D->Regs.PC+1);
    D->Regs.YR = MemReadByte (ZPAddr);
    TEST_ZF (D->Regs.YR);
    TEST_SF (D->Regs.YR);
    D->Regs.PC += 2;
}



static void OPC_6502_A5 (CPUInstance* D)
/* Opcode $A5: LDA zp */
{
    unsigned char ZPAddr;
    D->Cycles = 3;
    ZPAddr = MemReadByte (D->Regs.PC+1);
    D->Regs.AC = MemReadByte (ZPAddr);
    TEST_ZF (D->Regs.AC);
    TEST_SF (D->Regs.AC);
    D->Regs.PC += 2;
}



static void OPC_6502_A6 (CPUInstance* D)
/* Opcode $A6: LDX zp */
{
    unsigned char ZPAddr;
    D->Cycles = 3;
    ZPAddr = MemReadByte (D->Regs.PC+1);
    D->Regs.XR = MemReadByte (ZPAddr);
    TEST_ZF (D->Regs.XR);
    TEST_SF (D->Regs.XR);
    D->Regs.PC += 2;
}



static void OPC_6502_A8 (CPUInstance* D)
/* Opcode $A8: TAY */
{
    D->Cycles = 2;
    D->Regs.YR = D->Regs.AC;
    TEST_ZF (D->Regs.YR);
    TEST_SF (D->Regs.YR);
    D->Regs.PC += 1;
}



static void OPC_6502_A9 (CPUInstance* D)
/* Opcode $A9: LDA #imm */
{
    D->Cycles = 2;
    D->Regs.AC = MemReadByte (D->Regs.PC+1);
    TEST_ZF (D->Regs.AC);
    TEST_SF (D->Regs.AC);
    D->Regs.PC += 2;
}



static void OPC_6502_AA (CPUInstance* D)
/* Opcode $AA: TAX */
{
    D->Cycles = 2;
    D->Regs.XR = D->Regs.AC;
    TEST_ZF (D->Regs.XR);
    TEST_SF (D->Regs.XR);
    D->Regs.PC += 1;
}



static void OPC_6502_AC (CPUInstance* D)
/* Opcode $D->Regs.AC: LDY abs */
{
    unsigned Addr;
    D->Cycles = 4;
    Addr   = MemReadWord (D->Regs.PC+1);
    D->Regs.YR     = MemReadByte (Addr);
    TEST_ZF (D->Regs.YR);
    TEST_SF (D->Regs.YR);
    D->Regs.PC += 3;
}



static void OPC_6502_AD (CPUInstance* D)
/* Opcode $AD: LDA abs */
{
    unsigned Addr;
    D->Cycles = 4;
    Addr   = MemReadWord (D->Regs.PC+1);
    D->Regs.AC     = MemReadByte (Addr);
    TEST_ZF (D->Regs.AC);
    TEST_SF (D->Regs.AC);
    D->Regs.PC += 3;
}



static void OPC_6502_AE (CPUInstance* D)
/* Opcode $AE: LDX abs */
{
    unsigned Addr;
    D->Cycles = 4;
    Addr   = MemReadWord (D->Regs.PC+1);
    D->Regs.XR     = MemReadByte (Addr);
    TEST_ZF (D->Regs.XR);
    TEST_SF (D->Regs.XR);
    D->Regs.PC += 3;
}



static void OPC_6502_B0 (CPUInstance* D)
/* Opcode $B0: BCS */
{
    BRANCH (GET_CF ());
}



static void OPC_6502_B1 (CPUInstance* D)
/* Opcode $B1: LDA (zp),y */
{
    unsigned char ZPAddr;
    unsigned Addr;
    D->Cycles = 5;
    ZPAddr = MemReadByte (D->Regs.PC+1);
    Addr   = MemReadZPWord (ZPAddr);
    if (PAGE_CROSS (Addr, D->Regs.YR)) {
        ++D->Cycles;
    }
    D->Regs.AC = MemReadByte (Addr + D->Regs.YR);
    TEST_ZF (D->Regs.AC);
    TEST_SF (D->Regs.AC);
    D->Regs.PC += 2;
}



static void OPC_6502_B4 (CPUInstance* D)
/* Opcode $B4: LDY zp,x */
{
    unsigned char ZPAddr;
    D->Cycles = 4;
    ZPAddr = MemReadByte (D->Regs.PC+1) + D->Regs.XR;
    D->Regs.YR     = MemReadByte (ZPAddr);
    TEST_ZF (D->Regs.YR);
    TEST_SF (D->Regs.YR);
    D->Regs.PC += 2;
}



static void OPC_6502_B5 (CPUInstance* D)
/* Opcode $B5: LDA zp,x */
{
    unsigned char ZPAddr;
    D->Cycles = 4;
    ZPAddr = MemReadByte (D->Regs.PC+1) + D->Regs.XR;
    D->Regs.AC     = MemReadByte (ZPAddr);
    TEST_ZF (D->Regs.AC);
    TEST_SF (D->Regs.AC);
    D->Regs.PC += 2;
}



static void OPC_6502_B6 (CPUInstance* D)
/* Opcode $B6: LDX zp,y */
{
    unsigned char ZPAddr;
    D->Cycles = 4;
    ZPAddr = MemReadByte (D->Regs.PC+1) + D->Regs.YR;
    D->Regs.XR     = MemReadByte (ZPAddr);
    TEST_ZF (D->Regs.XR);
    TEST_SF (D->Regs.XR);
    D->Regs.PC += 2;
}



static void OPC_6502_B8 (CPUInstance* D)
/* Opcode $B8: CLV */
{
    D->Cycles = 2;
    SET_OF (0);
    D->Regs.PC += 1;
}



static void OPC_6502_B9 (CPUInstance* D)
/* Opcode $B9: LDA abs,y */
{
    unsigned Addr;
    D->Cycles = 4;
    Addr = MemReadWord (D->Regs.PC+1);
    if (PAGE_CROSS (Addr, D->Regs.YR)) {
        ++D->Cycles;
    }
    D->Regs.AC = MemReadByte (Addr + D->Regs.YR);
    TEST_ZF (D->Regs.AC);
    TEST_SF (D->Regs.AC);
    D->Regs.PC += 3;
}



static void OPC_6502_BA (CPUInstance* D)
/* Opcode $BA: TSX */
{
    D->Cycles = 2;
    D->Regs.XR = D->Regs.SP;
    TEST_ZF (D->Regs.XR);
    TEST_SF (D->Regs.XR);
    D->Regs.PC += 1;
}



static void OPC_6502_BC (CPUInstance* D)
/* Opcode $BC: LDY abs,x */
{
    unsigned Addr;
    D->Cycles = 4;
    Addr = MemReadWord (D->Regs.PC+1);
    if (PAGE_CROSS (Addr, D->Regs.XR)) {
        ++D->Cycles;
    }
    D->Regs.YR = MemReadByte (Addr + D->Regs.XR);
    TEST_ZF (D->Regs.YR);
    TEST_SF (D->Regs.YR);
    D->Regs.PC += 3;
}



static void OPC_6502_BD (CPUInstance* D)
/* Opcode $BD: LDA abs,x */
{
    unsigned Addr;
    D->Cycles = 4;
    Addr = MemReadWord (D->Regs.PC+1);
    if (PAGE_CROSS (Addr, D->Regs.XR)) {
        ++D->Cycles;
    }
    D->Regs.AC = MemReadByte (Addr + D->Regs.XR);
    TEST_ZF (D->Regs.AC);
    TEST_SF (D->Regs.AC);
    D->Regs.PC += 3;
}



static void OPC_6502_BE (CPUInstance* D)
/* Opcode $BE: LDX abs,y */
{
    unsigned Addr;
    D->Cycles = 4;
    Addr = MemReadWord (D->Regs.PC+1);
    if (PAGE_CROSS (Addr, D->Regs.YR)) {
        ++D->Cycles;
    }
    D->Regs.XR = MemReadByte (Addr + D->Regs.YR);
    TEST_ZF (D->Regs.XR);
    TEST_SF (D->Regs.XR);
    D->Regs.PC += 3;
}



static void OPC_6502_C0 (CPUInstance* D)
/* Opcode $C0: CPY #imm */
{
    D->Cycles = 2;
    CMP (D->Regs.YR, MemReadByte (D->Regs.PC+1));
    D->Regs.PC += 2;
}



static void OPC_6502_C1 (CPUInstance* D)
/* Opcode $C1: CMP (zp,x) */
{
    unsigned char ZPAddr;
    unsigned Addr;
    D->Cycles = 6;
    ZPAddr = MemReadByte (D->Regs.PC+1) + D->Regs.XR;
    Addr   = MemReadZPWord (ZPAddr);
    CMP (D->Regs.AC, MemReadByte (Addr));
    D->Regs.PC += 2;
}



static void OPC_6502_C4 (CPUInstance* D)
/* Opcode $C4: CPY zp */
{
    unsigned char ZPAddr;
    D->Cycles = 3;
    ZPAddr = MemReadByte (D->Regs.PC+1);
    CMP (D->Regs.YR, MemReadByte (ZPAddr));
    D->Regs.PC += 2;
}



static void OPC_6502_C5 (CPUInstance* D)
/* Opcode $C5: CMP zp */
{
    unsigned char ZPAddr;
    D->Cycles = 3;
    ZPAddr = MemReadByte (D->Regs.PC+1);
    CMP (D->Regs.AC, MemReadByte (ZPAddr));
    D->Regs.PC += 2;
}



static void OPC_6502_C6 (CPUInstance* D)
/* Opcode $C6: DEC zp */
{
    unsigned char ZPAddr;
    unsigned char Val;
    D->Cycles = 5;
    ZPAddr = MemReadByte (D->Regs.PC+1);
    Val    = MemReadByte (ZPAddr) - 1;
    MemWriteByte (ZPAddr, Val);
    TEST_ZF (Val);
    TEST_SF (Val);
    D->Regs.PC += 2;
}



static void OPC_6502_C8 (CPUInstance* D)
/* Opcode $C8: INY */
{
    D->Cycles = 2;
    D->Regs.YR = (D->Regs.YR + 1) & 0xFF;
    TEST_ZF (D->Regs.YR);
    TEST_SF (D->Regs.YR);
    D->Regs.PC += 1;
}



static void OPC_6502_C9 (CPUInstance* D)
/* Opcode $C9: CMP #imm */
{
    D->Cycles = 2;
    CMP (D->Regs.AC, MemReadByte (D->Regs.PC+1));
    D->Regs.PC += 2;
}



static void OPC_6502_CA (CPUInstance* D)
/* Opcode $CA: DEX */
{
    D->Cycles = 2;
    D->Regs.XR = (D->Regs.XR - 1) & 0xFF;
    TEST_ZF (D->Regs.XR);
    TEST_SF (D->Regs.XR);
    D->Regs.PC += 1;
}



static void OPC_6502_CC (CPUInstance* D)
/* Opcode $CC: CPY abs */
{
    unsigned Addr;
    D->Cycles = 4;
    Addr   = MemReadWord (D->Regs.PC+1);
    CMP (D->Regs.YR, MemReadByte (Addr));
    D->Regs.PC += 3;
}



static void OPC_6502_CD (CPUInstance* D)
/* Opcode $CD: CMP abs */
{
    unsigned Addr;
    D->Cycles = 4;
    Addr   = MemReadWord (D->Regs.PC+1);
    CMP (D->Regs.AC, MemReadByte (Addr));
    D->Regs.PC += 3;
}



static void OPC_6502_CE (CPUInstance* D)
/* Opcode $CE: DEC abs */
{
    unsigned Addr;
    unsigned char Val;
    D->Cycles = 6;
    Addr = MemReadWord (D->Regs.PC+1);
    Val  = MemReadByte (Addr) - 1;
    MemWriteByte (Addr, Val);
    TEST_ZF (Val);
    TEST_SF (Val);
    D->Regs.PC += 3;
}



static void OPC_6502_D0 (CPUInstance* D)
/* Opcode $D0: BNE */
{
    BRANCH (!GET_ZF ());
}



static void OPC_6502_D1 (CPUInstance* D)
/* Opcode $D1: CMP (zp),y */
{
    unsigned ZPAddr;
    unsigned Addr;
    D->Cycles = 5;
    ZPAddr = MemReadByte (D->Regs.PC+1);
    Addr   = MemReadWord (ZPAddr);
    if (PAGE_CROSS (Addr, D->Regs.YR)) {
        ++D->Cycles;
    }
    CMP (D->Regs.AC, MemReadByte (Addr + D->Regs.YR));
    D->Regs.PC += 2;
}



static void OPC_6502_D5 (CPUInstance* D)
/* Opcode $D5: CMP zp,x */
{
    unsigned char ZPAddr;
    D->Cycles = 4;
    ZPAddr = MemReadByte (D->Regs.PC+1) + D->Regs.XR;
    CMP (D->Regs.AC, MemReadByte (ZPAddr));
    D->Regs.PC += 2;
}



static void OPC_6502_D6 (CPUInstance* D)
/* Opcode $D6: DEC zp,x */
{
    unsigned char ZPAddr;
    unsigned char Val;
    D->Cycles = 6;
    ZPAddr = MemReadByte (D->Regs.PC+1) + D->Regs.XR;
    Val  = MemReadByte (ZPAddr) - 1;
    MemWriteByte (ZPAddr, Val);
    TEST_ZF (Val);
    TEST_SF (Val);
    D->Regs.PC += 2;
}



static void OPC_6502_D8 (CPUInstance* D)
/* Opcode $D8: CLD */
{
    D->Cycles = 2;
    SET_DF (0);
    D->Regs.PC += 1;
}



static void OPC_6502_D9 (CPUInstance* D)
/* Opcode $D9: CMP abs,y */
{
    unsigned Addr;
    D->Cycles = 4;
    Addr = MemReadWord (D->Regs.PC+1);
    if (PAGE_CROSS (Addr, D->Regs.YR)) {
        ++D->Cycles;
    }
    CMP (D->Regs.AC, MemReadByte (Addr + D->Regs.YR));
    D->Regs.PC += 3;
}



static void OPC_6502_DD (CPUInstance* D)
/* Opcode $DD: CMP abs,x */
{
    unsigned Addr;
    D->Cycles = 4;
    Addr = MemReadWord (D->Regs.PC+1);
    if (PAGE_CROSS (Addr, D->Regs.XR)) {
        ++D->Cycles;
    }
    CMP (D->Regs.AC, MemReadByte (Addr + D->Regs.XR));
    D->Regs.PC += 3;
}



static void OPC_6502_DE (CPUInstance* D)
/* Opcode $DE: DEC abs,x */
{
    unsigned Addr;
    unsigned char Val;
    D->Cycles = 7;
    Addr = MemReadWord (D->Regs.PC+1) + D->Regs.XR;
    Val  = MemReadByte (Addr) - 1;
    MemWriteByte (Addr, Val);
    TEST_ZF (Val);
    TEST_SF (Val);
    D->Regs.PC += 3;
}



static void OPC_6502_E0 (CPUInstance* D)
/* Opcode $E0: CPX #imm */
{
    D->Cycles = 2;
    CMP (D->Regs.XR, MemReadByte (D->Regs.PC+1));
    D->Regs.PC += 2;
}



static void OPC_6502_E1 (CPUInstance* D)
/* Opcode $E1: SBC (zp,x) */
{
    unsigned char ZPAddr;
    unsigned Addr;
    D->Cycles = 6;
    ZPAddr = MemReadByte (D->Regs.PC+1) + D->Regs.XR;
    Addr   = MemReadZPWord (ZPAddr);
    SBC (MemReadByte (Addr));
    D->Regs.PC += 2;
}



static void OPC_6502_E4 (CPUInstance* D)
/* Opcode $E4: CPX zp */
{
    unsigned char ZPAddr;
    D->Cycles = 3;
    ZPAddr = MemReadByte (D->Regs.PC+1);
    CMP (D->Regs.XR, MemReadByte (ZPAddr));
    D->Regs.PC += 2;
}



static void OPC_6502_E5 (CPUInstance* D)
/* Opcode $E5: SBC zp */
{
    unsigned char ZPAddr;
    D->Cycles = 3;
    ZPAddr = MemReadByte (D->Regs.PC+1);
    SBC (MemReadByte (ZPAddr));
    D->Regs.PC += 2;
}



static void OPC_6502_E6 (CPUInstance* D)
/* Opcode $E6: INC zp */
{
    unsigned char ZPAddr;
    unsigned char Val;
    D->Cycles = 5;
    ZPAddr = MemReadByte (D->Regs.PC+1);
    Val    = MemReadByte (ZPAddr) + 1;
    MemWriteByte (ZPAddr, Val);
    TEST_ZF (Val);
    TEST_SF (Val);
    D->Regs.PC += 2;
}



static void OPC_6502_E8 (CPUInstance* D)
/* Opcode $E8: INX */
{
    D->Cycles = 2;
    D->Regs.XR = (D->Regs.XR + 1) & 0xFF;
    TEST_ZF (D->Regs.XR);
    TEST_SF (D->Regs.XR);
    D->Regs.PC += 1;
}



static void OPC_6502_E9 (CPUInstance* D)
/* Opcode $E9: SBC #imm */
{
    D->Cycles = 2;
    SBC (MemReadByte (D->Regs.PC+1));
    D->Regs.PC += 2;
}



static void OPC_6502_EA (CPUInstance* D)
/* Opcode $EA: NOP */
{
    /* This one is easy... */
    D->Cycles = 2;
    D->Regs.PC += 1;
}



static void OPC_6502_EC (CPUInstance* D)
/* Opcode $EC: CPX abs */
{
    unsigned Addr;
    D->Cycles = 4;
    Addr   = MemReadWord (D->Regs.PC+1);
    CMP (D->Regs.XR, MemReadByte (Addr));
    D->Regs.PC += 3;
}



static void OPC_6502_ED (CPUInstance* D)
/* Opcode $ED: SBC abs */
{
    unsigned Addr;
    D->Cycles = 4;
    Addr   = MemReadWord (D->Regs.PC+1);
    SBC (MemReadByte (Addr));
    D->Regs.PC += 3;
}



static void OPC_6502_EE (CPUInstance* D)
/* Opcode $EE: INC abs */
{
    unsigned Addr;
    unsigned char Val;
    D->Cycles = 6;
    Addr = MemReadWord (D->Regs.PC+1);
    Val  = MemReadByte (Addr) + 1;
    MemWriteByte (Addr, Val);
    TEST_ZF (Val);
    TEST_SF (Val);
    D->Regs.PC += 3;
}



static void OPC_6502_F0 (CPUInstance* D)
/* Opcode $F0: BEQ */
{
    BRANCH (GET_ZF ());
}



static void OPC_6502_F1 (CPUInstance* D)
/* Opcode $F1: SBC (zp),y */
{
    unsigned char ZPAddr;
    unsigned Addr;
    D->Cycles = 5;
    ZPAddr = MemReadByte (D->Regs.PC+1);
    Addr   = MemReadZPWord (ZPAddr);
    if (PAGE_CROSS (Addr, D->Regs.YR)) {
        ++D->Cycles;
    }
    SBC (MemReadByte (Addr + D->Regs.YR));
    D->Regs.PC += 2;
}



static void OPC_6502_F5 (CPUInstance* D)
/* Opcode $F5: SBC zp,x */
{
    unsigned char ZPAddr;
    D->Cycles = 4;
    ZPAddr = MemReadByte (D->Regs.PC+1) + D->Regs.XR;
    SBC (MemReadByte (ZPAddr));
    D->Regs.PC += 2;
}



static void OPC_6502_F6 (CPUInstance* D)
/* Opcode $F6: INC zp,x */
{
    unsigned char ZPAddr;
    unsigned char Val;
    D->Cycles = 6;
    ZPAddr = MemReadByte (D->Regs.PC+1) + D->Regs.XR;
    Val  = MemReadByte (ZPAddr) + 1;
    MemWriteByte (ZPAddr, Val);
    TEST_ZF (Val);
    TEST_SF (Val);
    D->Regs.PC += 2;
}



static void OPC_6502_F8 (CPUInstance* D)
/* Opcode $F8: SED */
{
    SET_DF (1);
}



static void OPC_6502_F9 (CPUInstance* D)
/* Opcode $F9: SBC abs,y */
{
    unsigned Addr;
    D->Cycles = 4;
    Addr   = MemReadWord (D->Regs.PC+1);
    if (PAGE_CROSS (Addr, D->Regs.YR)) {
        ++D->Cycles;
    }
    SBC (MemReadByte (Addr + D->Regs.YR));
    D->Regs.PC += 3;
}



static void OPC_6502_FD (CPUInstance* D)
/* Opcode $FD: SBC abs,x */
{
    unsigned Addr;
    D->Cycles = 4;
    Addr   = MemReadWord (D->Regs.PC+1);
    if (PAGE_CROSS (Addr, D->Regs.XR)) {
        ++D->Cycles;
    }
    SBC (MemReadByte (Addr + D->Regs.XR));
    D->Regs.PC += 3;
}



static void OPC_6502_FE (CPUInstance* D)
/* Opcode $FE: INC abs,x */
{
    unsigned Addr;
    unsigned char Val;
    D->Cycles = 7;
    Addr = MemReadWord (D->Regs.PC+1) + D->Regs.XR;
    Val  = MemReadByte (Addr) + 1;
    MemWriteByte (Addr, Val);
    TEST_ZF (Val);
    TEST_SF (Val);
    D->Regs.PC += 3;
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
    OPC_65C02_6C,
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
/*                                   Code                                    */
/*****************************************************************************/



static void Init (const SimData* S)
/* Initialize the module */
{
    /* Remember the data pointer */
    Sim = S;
}



static void* Create6502Instance (void* CfgInfo)
/* Create an instance of a 6502 CPU */
{
    return NewCPUInstance (OP6502Table, CfgInfo);
}



static void* Create65C02Instance (void* CfgInfo)
/* Create an instance of a 65C02 CPU */
{
    return NewCPUInstance (OP65C02Table, CfgInfo);
}



static void DestroyInstance (void* Instance)
/* Destroy an instance of a CPU */
{
    /* Free the instance */
    DeleteCPUInstance (Instance);
}



static void IRQRequest (void* Data)
/* Generate an IRQ */
{
    /* Cast the pointer */
    CPUInstance* D = (CPUInstance*) Data;

    /* Remember the request */
    D->HaveIRQRequest = 1;
}



static void NMIRequest (void* Data)
/* Generate an NMI */
{
    /* Cast the pointer */
    CPUInstance* D = (CPUInstance*) Data;

    /* Remember the request */
    D->HaveNMIRequest = 1;
}



static void Reset (void* Data)
/* Generate a CPU RESET */
{
    /* Cast the pointer */
    CPUInstance* D = (CPUInstance*) Data;

    /* Reset the CPU */
    D->HaveIRQRequest = 0;
    D->HaveNMIRequest = 0;
    D->CPUHalted = 0;
    D->Regs.SR = 0;
    D->Regs.PC = MemReadWord (0xFFFC);
}



static unsigned ExecuteInsn (void* Data)
/* Execute one CPU instruction */
{
    /* Cast the pointer */
    CPUInstance* D = (CPUInstance*) Data;

    /* If the CPU is halted, do nothing */
    if (D->CPUHalted) {
        return 0;
    }

    /* If we have an NMI request, handle it */
    if (D->HaveNMIRequest) {

        D->HaveNMIRequest = 0;
        PUSH (PCH);
        PUSH (PCL);
        PUSH (D->Regs.SR);
        SET_IF (1);
        D->Regs.PC = MemReadWord (0xFFFA);
        D->Cycles = 7;

    } else if (D->HaveIRQRequest && GET_IF () == 0) {

        D->HaveIRQRequest = 0;
        PUSH (PCH);
        PUSH (PCL);
        PUSH (D->Regs.SR);
        SET_IF (1);
        D->Regs.PC = MemReadWord (0xFFFE);
        D->Cycles = 7;

    } else {

        /* Normal instruction - read the next opcode */
        unsigned char OPC = MemReadByte (D->Regs.PC);

        /* Execute it */
        D->Handlers[OPC] (D);

    }

    /* Count cycles */
    D->TotalCycles += D->Cycles;

    /* Return the number of clock cycles needed by this insn */
    return D->Cycles;
}



static unsigned long GetCycles (void* Data)
/* Return the total number of cycles executed */
{
    /* Cast the pointer */
    CPUInstance* D = (CPUInstance*) Data;

    /* Return the total number of cycles */
    return D->TotalCycles;
}



