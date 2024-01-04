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
/* Dec-2023, Carlo Bramini, rewritten for better maintenance and added       */
/*           support for undocumented opcodes for 6502                       */
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

/*

 6502 opcode map:

    x0   x1   x2   x3   x4   x5   x6   x7   x8   x9   xA   xB   xC   xD   xE   xF
0x  BRK  ORA  ---  SLO  NOP  ORA  ASL  SLO  PHP  ORA  ASL  ANC  NOP  ORA  ASL  SLO
         inx       inx  zp   zp   zp   zp        imm  acc  imm  abs  abs  abs  abs

1x  BPL  ORA  ---  SLO  NOP  ORA  ASL  SLO  CLC  ORA  NOP  SLO  NOP  ORA  ASL  SLO
    rel  iny       iny  zpx  zpx  zpx  zpy       aby       aby  abx  abx  abx  abx

2x  JSR  AND  ---  RLA  BIT  AND  ROL  RLA  PLP  AND  ROL  ANC  BIT  AND  ROL  RLA
    abs  inx       inx  zp   zp   zp   zp        imm  acc  imm  abs  abs  abs  abs

3x  BMI  AND  ---  RLA  NOP  AND  ROL  RLA  SEC  AND  NOP  RLA  NOP  AND  ROL  RLA
    rel  iny       iny  zpx  zpx  zpx  zpy       aby       aby  abx  abx  abx  abx

4x  RTI  EOR  ---  SRE  NOP  EOR  LSR  SRE  PHA  EOR  LSR  ASR  JMP  EOR  LSR  SRE
         inx       inx  zp   zp   zp   zp        imm  acc  imm  abs  abs  abs  abs

5x  BVC  EOR  ---  SRE  NOP  EOR  LSR  SRE  CLI  EOR  NOP  SRE  NOP  EOR  LSR  SRE
    rel  iny       iny  zpx  zpx  zpx  zpx       aby       aby  abx  abx  abx  abx

6x  RTS  ADC  ---  RRA  NOP  ADC  ROR  RRA  PLA  ADC  ROR  ARR  JMP  ADC  ROR  RRA
         inx       inx  zp   zp   zp   zp        imm  acc  imm  ind  abs  abs  abs

7x  BVS  ADC  ---  RRA  NOP  ADC  ROR  RRA  SEI  ADC  NOP  RRA  NOP  ADC  ROR  RRA
    rel  iny       iny  zpx  zpx  zpx  zpx       aby       aby  abx  abx  abx  abx

8x  NOP  STA  NOP  SAX  STY  STA  STX  SAX  DEY  NOP  TXA  ANE  STY  STA  STX  SAX
    imm  inx  imm  inx  zp   zp   zp   zp        imm       imm  abs  abs  abs  abs

9x  BCC  STA  ---  SHA  STY  STA  STX  SAX  TYA  STA  TXS  TAS  SHY  STA  SHX  SHA
    rel  iny       iny  zpx  zpx  zpy  zpy       aby       aby  abx  abx  aby  aby

Ax  LDY  LDA  LDX  LAX  LDY  LDA  LDX  LAX  TAY  LDA  TAX  LXA  LDY  LDA  LDX  LAX
    imm  inx  imm  inx  zp   zp   zp   zp        imm       imm  abs  abs  abs  abs

Bx  BCS  LDA  ---  LAX  LDY  LDA  LDX  LAX  CLV  LDA  TSX  LAS  LDY  LDA  LDX  LAX
    rel  iny       iny  zpx  zpx  zpy  zpy       aby       aby  abx  abx  aby  aby

Cx  CPY  CMP  NOP  DCP  CPY  CMP  DEC  DCP  INY  CMP  DEX  SBX  CPY  CMP  DEC  DCP
    imm  inx  imm  inx  zp   zp   zp   zp        imm       imm  abs  abs  abs  abs

Dx  BNE  CMP  ---  DCP  NOP  CMP  DEC  DCP  CLD  CMP  NOP  DCP  NOP  CMP  DEC  DCP
    rel  iny       iny  zpx  zpx  zpx  zpx       aby  zpx  aby  abx  abx  abx  abx

Ex  CPX  SBC  NOP  ISC  CPX  SBC  INC  ISC  INX  SBC  NOP  SBC  CPX  SBC  INC  ISC
    imm  inx  imm  inx  zp   zp   zp   zp        imm       imm  abs  abs  abs  abs

Fx  BEQ  SBC  ---  ISC  NOP  SBC  INC  ISC  SED  SBC  NOP  ISC  NOP  SBC  INC  ISC
    rel  iny       iny  zpx  zpx  zpx  zpx       aby  zpx  aby  abx  abx  abx  abx

--- = CPU JAM/HALT

*/

/*

65xx ILLEGAL INSTRUCTIONS


* SLO: shift left the contents of a memory location and then OR the result with
       the accumulator.

Address mode | opcode | cycles |            N V B D I Z C
-------------+--------+--------+    FLAGS:  X . . . . X X
SLO abs      |   0Fh  |  6     |
SLO abs,X    |   1Fh  |  7     |
SLO abs,Y    |   1Bh  |  7     |
SLO zp       |   07h  |  5     |
SLO zp,X     |   17h  |  6     |
SLO (zp,X)   |   03h  |  8     |
SLO (zp),Y   |   13h  |  8     |
-------------+--------+--------+


* RLA: rotate left the contents of a memory location and then AND the result with
       the accumulator.

Address mode | opcode | cycles |            N V B D I Z C
-------------+--------+--------+    FLAGS:  X . . . . X X
RLA abs      |   2Fh  |  6     |
RLA abs,X    |   3Fh  |  7     |
RLA abs,Y    |   3Bh  |  7     |
RLA zp       |   27h  |  5     |
RLA zp,X     |   37h  |  6     |
RLA (zp,X)   |   23h  |  8     |
RLA (zp),Y   |   33h  |  8     |
-------------+--------+--------+


* SRE: shift right the contents of a memory location and then X-OR the result
       with the accumulator.

Address mode | opcode | cycles |            N V B D I Z C
-------------+--------+--------+    FLAGS:  X . . . . X X
SRE abs      |   4Fh  |  6     |
SRE abs,X    |   5Fh  |  7     |
SRE abs,Y    |   5Bh  |  7     |
SRE zp       |   47h  |  5     |
SRE zp,X     |   57h  |  6     |
SRE (zp,X)   |   43h  |  8     |
SRE (zp),Y   |   53h  |  8     |
-------------+--------+--------+


* RRA: rotate right the contents of a memory location and then adds with carry
       the result with the accumulator.

Address mode | opcode | cycles |            N V B D I Z C
-------------+--------+--------+    FLAGS:  X X . . . X X
RRA abs      |   6Fh  |  6     |
RRA abs,X    |   7Fh  |  7     |
RRA abs,Y    |   7Bh  |  7     |
RRA zp       |   67h  |  5     |
RRA zp,X     |   77h  |  6     |
RRA (zp,X)   |   63h  |  8     |
RRA (zp),Y   |   73h  |  8     |
-------------+--------+--------+


* SAX: calculate AND between the A and X registers (without changing the
       contents of the registers) and stores the result in memory.
       Flags into P register are not modified.

Address mode | opcode | cycles |            N V B D I Z C
-------------+--------+--------+    FLAGS:  . . . . . . .
SAX abs      |   8Fh  |  4     |
SAX zp       |   87h  |  3     |
SAX zp,Y     |   97h  |  4     |
SAX (zp,X)   |   83h  |  6     |
-------------+--------+--------+


* LAX: loads both the accumulator and the X register with the content of a memory
       location.

Address mode | opcode | cycles |            N V B D I Z C
-------------+--------+--------+    FLAGS:  X . . . . X .
LAX abs      |   AFh  |  4     |
LAX abs,Y    |   BFh  |  4*    |    * = adds +1 if page cross is detected.
LAX zp       |   A7h  |  3     |
LAX zp,Y     |   B7h  |  4     |
LAX (zp,X)   |   A3h  |  6     |
LAX (zp),Y   |   B3h  |  5*    |
-------------+--------+--------+


* DCP: decrements the contents of a memory location and then compares the result
       with the accumulator.

Address mode | opcode | cycles |            N V B D I Z C
-------------+--------+--------+    FLAGS:  X . . . . X X
DCP abs      |   CFh  |  6     |
DCP abs,X    |   DFh  |  7     |
DCP abs,Y    |   DBh  |  7     |
DCP zp       |   C7h  |  5     |
DCP zp,X     |   D7h  |  6     |
DCP (zp,X)   |   C3h  |  8     |
DCP (zp),Y   |   D3h  |  8     |
-------------+--------+--------+


* ISC: increments the contents of a memory location and then subtract with carry
       the result from the accumulator.

Address mode | opcode | cycles |            N V B D I Z C
-------------+--------+--------+    FLAGS:  X X . . . X X
ISC abs      |   EFh  |  6     |
ISC abs,X    |   FFh  |  7     |
ISC abs,Y    |   FBh  |  7     |
ISC zp       |   E7h  |  5     |
ISC zp,X     |   F7h  |  6     |
ISC (zp,X)   |   E3h  |  8     |
ISC (zp),Y   |   F3h  |  8     |
-------------+--------+--------+


* ASR: calculates the AND between the accumulator and an immediate value and then
       shift right the result.

Address mode | opcode | cycles |            N V B D I Z C
-------------+--------+--------+    FLAGS:  X . . . . X X
ASR #imm     |   4Bh  |  2     |
-------------+--------+--------+


* ARR: calculates the AND between the accumulator and an immediate value and then
       rotate right the result.

Address mode | opcode | cycles |            N V B D I Z C
-------------+--------+--------+    FLAGS:  X . . . . X X
ARR #imm     |   6Bh  |  2     |
-------------+--------+--------+


* ANE: calculates the OR of the accumulator with an unstable constant, then it does
       an AND with the X register and an immediate value.
       The unstable constant varies with temperature, the production batch and
       maybe other factors. Experimental measures assume its value to 0xEF.

Address mode | opcode | cycles |            N V B D I Z C
-------------+--------+--------+    FLAGS:  X . . . . X .
ANE #imm     |   8Bh  |  2     |
-------------+--------+--------+


* LXA: calculates the OR of the accumulator with an unstable constant, then it does
       an AND with an immediate value. The result is copied into the X register and
       the accumulator.
       The unstable constant varies with temperature, the production batch and
       maybe other factors. Experimental measures assume its value to 0xEE.

Address mode | opcode | cycles |            N V B D I Z C
-------------+--------+--------+    FLAGS:  X . . . . X .
LXA #imm     |   ABh  |  2     |
-------------+--------+--------+


* SBX: calculates the AND of the accumulator with the X register and the subtracts
       an immediate value.

Address mode | opcode | cycles |            N V B D I Z C
-------------+--------+--------+    FLAGS:  X . . . . X X
SBX #imm     |   CBh  |  2     |
-------------+--------+--------+


* NOP: No-Operation.

Address mode | opcode | cycles |            N V B D I Z C
-------------+--------+--------+    FLAGS:  . . . . . . .
NOP          |   1Ah  |  2     |
NOP          |   3Ah  |  2     |    * = adds +1 if page cross is detected.
NOP          |   5Ah  |  2     |
NOP          |   7Ah  |  2     |
NOP          |   DAh  |  2     |
NOP          |   FAh  |  2     |
NOP #imm     |   80h  |  2     |
NOP #imm     |   82h  |  2     |
NOP #imm     |   89h  |  2     |
NOP #imm     |   C2h  |  2     |
NOP #imm     |   E2h  |  2     |
NOP zp       |   04h  |  3     |
NOP zp,x     |   14h  |  4     |
NOP zp,x     |   34h  |  4     |
NOP zp       |   44h  |  3     |
NOP zp,x     |   54h  |  4     |
NOP zp       |   64h  |  3     |
NOP zp,x     |   74h  |  4     |
NOP zp,x     |   D4h  |  4     |
NOP zp,x     |   F4h  |  4     |
NOP abs      |   0Ch  |  4     |
NOP abs,x    |   1Ch  |  4*    |
NOP abs,x    |   3Ch  |  4*    |
NOP abs,x    |   5Ch  |  4*    |
NOP abs,x    |   7Ch  |  4*    |
NOP abs,x    |   DCh  |  4*    |
NOP abs,x    |   FCh  |  4*    |
-------------+--------+--------+


* TAS: calculates the AND of the accumulator with the X register and stores the result
       into the stack pointer. Then, it calculates the AND of the result with the
       high byte of the memory pointer plus 1 and it stores the final result in memory.

Address mode | opcode | cycles |            N V B D I Z C
-------------+--------+--------+    FLAGS:  . . . . . . .
TAS abs,y    |   9Bh  |  5     |
-------------+--------+--------+


* SHY: calculates the AND of the Y register with the high byte of the memory pointer
       plus 1 and it stores the final result in memory.

Address mode | opcode | cycles |            N V B D I Z C
-------------+--------+--------+    FLAGS:  . . . . . . .
SHY abs,x    |   9Ch  |  5     |
-------------+--------+--------+


* SHX: calculates the AND of the X register with the high byte of the memory pointer
       plus 1 and it stores the final result in memory.

Address mode | opcode | cycles |            N V B D I Z C
-------------+--------+--------+    FLAGS:  . . . . . . .
SHX abs,y    |   9Eh  |  5     |
-------------+--------+--------+


* SHA: calculates the AND of the accumulator with the X register with the high byte
       of the memory pointer plus 1 and it stores the final result in memory.

Address mode | opcode | cycles |            N V B D I Z C
-------------+--------+--------+    FLAGS:  . . . . . . .
SHX abs,y    |   9Fh  |  5     |
SHX (zp),y   |   93h  |  6     |
-------------+--------+--------+


* ANC: calculates the AND of the accumulator with an immediate value and then
       updates the status of N and Z bits of the status register.
       The N flag is also copied into the Carry flag.

Address mode | opcode | cycles |            N V B D I Z C
-------------+--------+--------+    FLAGS:  X . . . . X X
ANC #imm     |   0Bh  |  2     |
ANC #imm     |   2Bh  |  2     |
-------------+--------+--------+


* LAS: calculates the contents of a memory location with the contents of the
stack pointer register and it stores the result in the accumulator, the X
register, and the stack pointer.

Address mode | opcode | cycles |            N V B D I Z C
-------------+--------+--------+    FLAGS:  X . . . . X .
LAS abs,y    |   BBh  |  4*    |
-------------+--------+--------+    * = adds +1 if page cross is detected.


* SBC: alias of the official SBC opcode.

Address mode | opcode | cycles |            N V B D I Z C
-------------+--------+--------+    FLAGS:  X X . . . X X
SBC #imm     |   EBh  |  2     |
-------------+--------+--------+


*/

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

/* Address operators */

/* zp */
#define ADR_ZP(ad)                                              \
    ad = MemReadByte (Regs.PC+1);                               \
    Regs.PC += 2

/* zp,x */
#define ADR_ZPX(ad)                                             \
    ad = (MemReadByte (Regs.PC+1) + Regs.XR) & 0xFF;            \
    Regs.PC += 2

/* zp,y */
#define ADR_ZPY(ad)                                             \
    ad = (MemReadByte (Regs.PC+1) + Regs.YR) & 0xFF;            \
    Regs.PC += 2

/* abs */
#define ADR_ABS(ad)                                             \
    ad = MemReadWord (Regs.PC+1);                               \
    Regs.PC += 3

/* abs,x */
#define ADR_ABSX(ad)                                            \
    ad = MemReadWord (Regs.PC+1);                               \
    if (PAGE_CROSS (ad, Regs.XR)) {                             \
        ++Cycles;                                               \
    }                                                           \
    ad += Regs.XR;                                              \
    Regs.PC += 3

/* abs,y */
#define ADR_ABSY(ad)                                            \
    ad = MemReadWord (Regs.PC+1);                               \
    if (PAGE_CROSS (ad, Regs.YR)) {                             \
        ++Cycles;                                               \
    }                                                           \
    ad += Regs.YR;                                              \
    Regs.PC += 3

/* (zp,x) */
#define ADR_ZPXIND(ad)                                          \
    ad = (MemReadByte (Regs.PC+1) + Regs.XR) & 0xFF;            \
    ad = MemReadZPWord (ad);                                    \
    Regs.PC += 2

/* (zp),y */
#define ADR_ZPINDY(ad)                                          \
    ad = MemReadZPWord (MemReadByte (Regs.PC+1));               \
    if (PAGE_CROSS (ad, Regs.YR)) {                             \
        ++Cycles;                                               \
    }                                                           \
    ad += Regs.YR;                                              \
    Regs.PC += 2

/* (zp) */
#define ADR_ZPIND(ad)                                           \
    ad = MemReadZPWord (MemReadByte (Regs.PC+1));               \
    Regs.PC += 2

/* Address operators (no penalty on page cross) */

/* abs,x - no penalty */
#define ADR_ABSX_NP(ad)                                         \
    ad = MemReadWord (Regs.PC+1);                               \
    ad += Regs.XR;                                              \
    Regs.PC += 3

/* abs,y - no penalty */
#define ADR_ABSY_NP(ad)                                         \
    ad = MemReadWord (Regs.PC+1);                               \
    ad += Regs.YR;                                              \
    Regs.PC += 3

/* (zp),y - no penalty */
#define ADR_ZPINDY_NP(ad)                                       \
    ad = MemReadZPWord (MemReadByte (Regs.PC+1));               \
    ad += Regs.YR;                                              \
    Regs.PC += 2



/* Memory operators */

/* #imm */
#define MEM_AD_OP_IMM(op)                                       \
    op = MemReadByte (Regs.PC+1);                               \
    Regs.PC += 2

/* zp / zp,x / zp,y / abs / abs,x / abs,y / (zp,x) / (zp),y / (zp) */
#define MEM_AD_OP(mode, ad, op)                                 \
    ADR_##mode(ad);                                             \
    op = MemReadByte (ad)

/* ALU opcode helpers */

/* Execution cycles for ALU opcodes */
#define ALU_CY_ZP       3
#define ALU_CY_ZPX      4
#define ALU_CY_ZPY      4
#define ALU_CY_ABS      4
#define ALU_CY_ABSX     4
#define ALU_CY_ABSY     4
#define ALU_CY_ZPXIND   6
#define ALU_CY_ZPINDY   5
#define ALU_CY_ZPIND    5

/* #imm */
#define ALU_OP_IMM(op)                                          \
    unsigned char immediate;                                    \
    MEM_AD_OP_IMM(immediate);                                   \
    Cycles = 2;                                                 \
    op (immediate)

/* zp / zp,x / zp,y / abs / abs,x / abs,y / (zp,x) / (zp),y / (zp) */
#define ALU_OP(mode, op)                                        \
    unsigned address, operand;                                  \
    Cycles = ALU_CY_##mode;                                     \
    MEM_AD_OP (mode, address, operand);                         \
    op (operand)

/* Store opcode helpers */

/* Execution cycles for store opcodes */
#define STO_CY_ZP       3
#define STO_CY_ZPX      4
#define STO_CY_ZPY      4
#define STO_CY_ABS      4
#define STO_CY_ABSX     5
#define STO_CY_ABSY     5
#define STO_CY_ZPXIND   6
#define STO_CY_ZPINDY   6
#define STO_CY_ZPIND    5

/* zp / zp,x / zp,y / abs / abs,x / abs,y / (zp,x) / (zp),y / (zp) */
#define STO_OP(mode, op)                                        \
    unsigned address;                                           \
    Cycles = STO_CY_##mode;                                     \
    ADR_##mode (address);                                       \
    MemWriteByte(address, op)

/* zp / zp,x / zp,y / abs / abs,x / abs,y / (zp,x) / (zp),y / (zp) */
#define STO_CB(mode, cb)                                        \
    unsigned address, operand;                                  \
    Cycles = STO_CY_##mode;                                     \
    ADR_##mode (address);                                       \
    cb (operand);                                               \
    MemWriteByte(address, operand)

/* Read-Modify-Write opcode helpers */

/* Execution cycles for R-M-W opcodes */
#define RMW_CY_ZP       5
#define RMW_CY_ZPX      6
#define RMW_CY_ZPY      6
#define RMW_CY_ABS      6
#define RMW_CY_ABSX     7
#define RMW_CY_ABSY     7
#define RMW_CY_ZPXIND   6
#define RMW_CY_ZPINDY   5
#define RMW_CY_ZPIND    5

#define RMW_CY_ABSX_NP      RMW_CY_ABSX
#define RMW_CY_ABSY_NP      RMW_CY_ABSY
#define RMW_CY_ZPINDY_NP    RMW_CY_ZPINDY

/* zp / zp,x / zp,y / abs / abs,x / abs,y / (zp,x) / (zp),y / (zp) */
#define MEM_OP(mode, op)                                        \
    unsigned address, operand;                                  \
    Cycles = RMW_CY_##mode;                                     \
    MEM_AD_OP (mode, address, operand);                         \
    op (operand);                                               \
    MemWriteByte (address, (unsigned char)operand)

/* 2 x Read-Modify-Write opcode helpers (illegal opcodes) */

/* Execution cycles for 2 x R-M-W opcodes */
#define RMW2_CY_ZP       5
#define RMW2_CY_ZPX      6
#define RMW2_CY_ZPY      6
#define RMW2_CY_ABS      6
#define RMW2_CY_ABSX     7
#define RMW2_CY_ABSY     7
#define RMW2_CY_ZPXIND   8
#define RMW2_CY_ZPINDY   8

/* zp / zp,x / zp,y / abs / abs,x / abs,y / (zp,x) / (zp),y */
#define ILLx2_OP(mode, op)                                      \
    unsigned address;                                           \
    unsigned operand;                                           \
    Cycles = RMW2_CY_##mode;                                    \
    MEM_AD_OP (mode, address, operand);                         \
    op (operand);                                               \
    MemWriteByte (address, (unsigned char)operand)

/* AC opcode helpers */

/* #imm */
#define AC_OP_IMM(op)                                           \
    unsigned char immediate;                                    \
    MEM_AD_OP_IMM(immediate);                                   \
    Cycles = 2;                                                 \
    Regs.AC = Regs.AC op immediate;                             \
    TEST_ZF (Regs.AC);                                          \
    TEST_SF (Regs.AC)

/* zp / zp,x / zp,y / abs / abs,x / abs,y / (zp,x) / (zp),y / (zp) */
#define AC_OP(mode, op)                                         \
    unsigned address;                                           \
    unsigned operand;                                           \
    Cycles = ALU_CY_##mode;                                     \
    MEM_AD_OP(mode, address, operand);                          \
    Regs.AC = Regs.AC op operand;                               \
    TEST_ZF (Regs.AC);                                          \
    TEST_SF (Regs.AC)


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
            if (CPU == CPU_65C02) {                             \
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
        Regs.PC = (Regs.PC + 2 + (int) Offs) & 0xFFFF;          \
        if (PCH != OldPCH) {                                    \
            ++Cycles;                                           \
        }                                                       \
    } else {                                                    \
        Regs.PC += 2;                                           \
    }

/* compares */
#define COMPARE(v1, v2)                                         \
    do {                                                        \
        unsigned Result = v1 - v2;                              \
        TEST_ZF (Result);                                       \
        TEST_SF (Result);                                       \
        SET_CF (Result <= 0xFF);                                \
    } while (0)

#define CPX(operand)                                            \
    COMPARE (Regs.XR, operand)

#define CPY(operand)                                            \
    COMPARE (Regs.YR, operand)

#define CMP(operand)                                            \
    COMPARE (Regs.AC, operand)

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

/* ASL */
#define ASL(Val)                                                \
    SET_CF (Val & 0x80);                                        \
    Val = (Val << 1) & 0xFF;                                    \
    TEST_ZF (Val);                                              \
    TEST_SF (Val)

/* LSR */
#define LSR(Val)                                                \
    SET_CF (Val & 0x01);                                        \
    Val >>= 1;                                                  \
    TEST_ZF (Val);                                              \
    TEST_SF (Val)

/* INC */
#define INC(Val)                                                \
    Val = (Val + 1) & 0xFF;                                     \
    TEST_ZF (Val);                                              \
    TEST_SF (Val)

/* DEC */
#define DEC(Val)                                                \
    Val = (Val - 1) & 0xFF;                                     \
    TEST_ZF (Val);                                              \
    TEST_SF (Val)

/* SLO */
#define SLO(Val)                                                \
    Val <<= 1;                                                  \
    SET_CF (Val & 0x100);                                       \
    Regs.AC |= Val;                                             \
    Regs.AC &= 0xFF;                                            \
    TEST_ZF (Regs.AC);                                          \
    TEST_SF (Regs.AC)

/* RLA */
#define RLA(Val)                                                \
    Val <<= 1;                                                  \
    if (GET_CF ()) {                                            \
        Val |= 0x01;                                            \
    }                                                           \
    SET_CF (Val & 0x100);                                       \
    Regs.AC &= Val;                                             \
    TEST_ZF (Regs.AC);                                          \
    TEST_SF (Regs.AC)

/* SRE */
#define SRE(Val)                                                \
    SET_CF (Val & 0x01);                                        \
    Val >>= 1;                                                  \
    Regs.AC ^= Val;                                             \
    TEST_ZF (Regs.AC);                                          \
    TEST_SF (Regs.AC)

/* RRA */
#define RRA(Val)                                                \
    if (GET_CF ()) {                                            \
        Val |= 0x100;                                           \
    }                                                           \
    SET_CF (Val & 0x01);                                        \
    Val >>= 1;                                                  \
    ADC (Val)

/* BIT */
#define BIT(Val)                                                \
    SET_SF (Val & 0x80);                                        \
    SET_OF (Val & 0x40);                                        \
    SET_ZF ((Val & Regs.AC) == 0)

/* LDA */
#define LDA(Val)                                                \
    Regs.AC = Val;                                              \
    TEST_SF (Val);                                              \
    TEST_ZF (Val)

/* LDX */
#define LDX(Val)                                                \
    Regs.XR = Val;                                              \
    TEST_SF (Val);                                              \
    TEST_ZF (Val)

/* LDY */
#define LDY(Val)                                                \
    Regs.YR = Val;                                              \
    TEST_SF (Val);                                              \
    TEST_ZF (Val)

/* LAX */
#define LAX(Val)                                                \
    Regs.AC = Val;                                              \
    Regs.XR = Val;                                              \
    TEST_SF (Val);                                              \
    TEST_ZF (Val)

/* TSB */
#define TSB(Val)                                                \
    SET_ZF ((Val & Regs.AC) == 0);                              \
    Val |= Regs.AC

/* TRB */
#define TRB(Val)                                                \
    SET_ZF ((Val & Regs.AC) == 0);                              \
    Val &= ~Regs.AC

/* DCP */
#define DCP(Val)                                                \
    Val = (Val - 1) & 0xFF;                                     \
    COMPARE (Regs.AC, Val)

/* ISC */
#define ISC(Val)                                                \
    Val = (Val + 1) & 0xFF;                                     \
    SBC(Val)

/* ASR */
#define ASR(Val)                                                \
    Regs.AC &= Val;                                             \
    LSR(Regs.AC)

/* ARR */
#define ARR(Val)                                                \
    do {                                                        \
        unsigned tmp = Regs.AC & Val;                           \
        Val = tmp >> 1;                                         \
        if (GET_CF ()) {                                        \
            Val |= 0x80;                                        \
        }                                                       \
        if (GET_DF ()) {                                        \
            SET_SF (GET_CF ());                                 \
            TEST_ZF (Val);                                      \
            SET_OF ((Val ^ tmp) & 0x40);                        \
            if (((tmp & 0x0f) + (tmp & 0x01)) > 0x05) {         \
                Val = (Val & 0xf0) | ((Val + 0x06) & 0x0f);     \
            }                                                   \
            if (((tmp & 0xf0) + (tmp & 0x10)) > 0x50) {         \
                Val = (Val & 0x0f) | ((Val + 0x60) & 0xf0);     \
                SET_CF(1);                                      \
            } else {                                            \
                SET_CF(0);                                      \
            }                                                   \
            if (CPU == CPU_65C02) {                             \
                ++Cycles;                                       \
            }                                                   \
        } else {                                                \
            TEST_SF (Val);                                      \
            TEST_ZF (Val);                                      \
            SET_CF (Val & 0x40);                                \
            SET_OF ((Val & 0x40) ^ ((Val & 0x20) << 1));        \
        }                                                       \
        Regs.AC = Val;                                          \
    } while (0);

/* ANE */
#define ANE(Val)                                                \
    Val = (Regs.AC | 0xEF) & Regs.XR & Val;                     \
    Regs.AC = Val;                                              \
    TEST_SF (Val);                                              \
    TEST_ZF (Val)

/* LXA */
#define LXA(Val)                                                \
    Val = (Regs.AC | 0xEE) & Val;                               \
    Regs.AC = Val;                                              \
    Regs.XR = Val;                                              \
    TEST_SF (Val);                                              \
    TEST_ZF (Val)

/* SBX */
#define SBX(Val)                                                \
    do {                                                        \
        unsigned tmp = (Regs.AC & Regs.XR) - (Val);             \
        SET_CF (tmp < 0x100);                                   \
        tmp &= 0xFF;                                            \
        Regs.XR = tmp;                                          \
        TEST_SF (tmp);                                          \
        TEST_ZF (tmp);                                          \
    } while (0);

/* NOP */
#define NOP(Val)                                                \
    (void)Val

/* TAS */
#define TAS(Val)                                                \
    Val = Regs.AC & Regs.XR;                                    \
    Regs.SP = Val;                                              \
    Val &= (address >> 8) + 1

/* SHA */
#define SHA(Val)                                                \
    Val = Regs.AC & Regs.XR & ((address >> 8) + 1)

/* ANC */
#define ANC(Val)                                                \
    Val = Regs.AC & Val;                                        \
    Regs.AC = Val;                                              \
    SET_CF (Val & 0x80);                                        \
    TEST_SF (Val);                                              \
    TEST_ZF (Val)


/* LAS */
#define LAS(Val)                                                \
    Val = Regs.SP & Val;                                        \
    Regs.AC = Val;                                              \
    Regs.XR = Val;                                              \
    Regs.SP = Val;                                              \
    TEST_SF (Val);                                              \
    TEST_ZF (Val)


/* SBC */
#define SBC(v)                                                  \
    do {                                                        \
        unsigned r_a = Regs.AC;                                 \
        unsigned src = (v) & 0xFF;                              \
        unsigned ccc = (Regs.SR & CF) ^ CF;                     \
        unsigned tmp = r_a - src - ccc;                         \
                                                                \
        SET_CF(tmp < 0x100);                                    \
        TEST_SF(tmp);                                           \
        TEST_ZF(tmp);                                           \
        SET_OF((r_a ^ tmp) & (r_a ^ src) & 0x80);               \
                                                                \
        if (GET_DF ()) {                                        \
            unsigned low = (r_a & 0x0f) - (src & 0x0f) - ccc;   \
            tmp = (r_a & 0xf0) - (src & 0xf0);                  \
            if (low & 0x10) {                                   \
                low -= 6;                                       \
                tmp -= 0x10;                                    \
            }                                                   \
            tmp = (low & 0xf) | tmp;                            \
            if (tmp & 0x100) {                                  \
                tmp -= 0x60;                                    \
            }                                                   \
        }                                                       \
        Regs.AC = tmp & 0xFF;                                   \
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
    if (CPU == CPU_65C02)
    {
        SET_DF (0);
    }
    Regs.PC = MemReadWord (0xFFFE);
}



static void OPC_6502_01 (void)
/* Opcode $01: ORA (ind,x) */
{
    AC_OP (ZPXIND, |);
}



static void OPC_6502_03 (void)
/* Opcode $03: SLO (zp,x) */
{
    ILLx2_OP (ZPXIND, SLO);
}



/* Aliases of opcode $04 */
#define OPC_6502_44 OPC_6502_04
#define OPC_6502_64 OPC_6502_04

static void OPC_6502_04 (void)
/* Opcode $04: NOP zp */
{
    ALU_OP (ZP, NOP);
}



static void OPC_65SC02_04 (void)
/* Opcode $04: TSB zp */
{
    MEM_OP (ZP, TSB);
}



static void OPC_6502_05 (void)
/* Opcode $05: ORA zp */
{
    AC_OP (ZP, |);
}



static void OPC_6502_06 (void)
/* Opcode $06: ASL zp */
{
    MEM_OP (ZP, ASL);
}



static void OPC_6502_07 (void)
/* Opcode $07: SLO zp */
{
    ILLx2_OP (ZP, SLO);
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
    ASL(Regs.AC);
    Regs.PC += 1;
}



/* Aliases of opcode $0B */
#define OPC_6502_2B OPC_6502_0B

static void OPC_6502_0B (void)
/* Opcode $0B: ANC #imm */
{
    ALU_OP_IMM (ANC);
}



static void OPC_6502_0C (void)
/* Opcode $0C: NOP abs */
{
    ALU_OP (ABS, NOP);
}



static void OPC_65SC02_0C (void)
/* Opcode $0C: TSB abs */
{
    MEM_OP (ABS, TSB);
}



static void OPC_6502_0D (void)
/* Opcode $0D: ORA abs */
{
    AC_OP (ABS, |);
}



static void OPC_6502_0E (void)
/* Opcode $0E: ASL abs */
{
    MEM_OP (ABS, ASL);
}



static void OPC_6502_0F (void)
/* Opcode $0F: SLO abs */
{
    ILLx2_OP (ABS, SLO);
}



static void OPC_6502_10 (void)
/* Opcode $10: BPL */
{
    BRANCH (!GET_SF ());
}



static void OPC_6502_11 (void)
/* Opcode $11: ORA (zp),y */
{
    AC_OP (ZPINDY, |);
}



static void OPC_65SC02_12 (void)
/* Opcode $12: ORA (zp) */
{
    AC_OP (ZPIND, |);
}



static void OPC_6502_13 (void)
/* Opcode $03: SLO (zp),y */
{
    ILLx2_OP (ZPINDY, SLO);
}



/* Aliases of opcode $14 */
#define OPC_6502_34 OPC_6502_14
#define OPC_6502_54 OPC_6502_14
#define OPC_6502_74 OPC_6502_14
#define OPC_6502_D4 OPC_6502_14
#define OPC_6502_F4 OPC_6502_14

static void OPC_6502_14 (void)
/* Opcode $04: NOP zp,x */
{
    ALU_OP (ZPX, NOP);
}



static void OPC_65SC02_14 (void)
/* Opcode $14: TRB zp */
{
    MEM_OP (ZP, TRB);
}



static void OPC_6502_15 (void)
/* Opcode $15: ORA zp,x */
{
   AC_OP (ZPX, |);
}



static void OPC_6502_16 (void)
/* Opcode $16: ASL zp,x */
{
    MEM_OP (ZPX, ASL);
}



static void OPC_6502_17 (void)
/* Opcode $17: SLO zp,x */
{
    ILLx2_OP (ZPX, SLO);
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
    AC_OP (ABSY, |);
}



static void OPC_65SC02_1A (void)
/* Opcode $1A: INC a */
{
    Cycles = 2;
    INC(Regs.AC);
    Regs.PC += 1;
}



static void OPC_6502_1B (void)
/* Opcode $1B: SLO abs,y */
{
    ILLx2_OP (ABSY, SLO);
}



/* Aliases of opcode $1C */
#define OPC_6502_3C OPC_6502_1C
#define OPC_6502_5C OPC_6502_1C
#define OPC_6502_7C OPC_6502_1C
#define OPC_6502_DC OPC_6502_1C
#define OPC_6502_FC OPC_6502_1C

static void OPC_6502_1C (void)
/* Opcode $1C: NOP abs,x */
{
    ALU_OP (ABSX, NOP);
}



static void OPC_65SC02_1C (void)
/* Opcode $1C: TRB abs */
{
    MEM_OP (ABS, TRB);
}



static void OPC_6502_1D (void)
/* Opcode $1D: ORA abs,x */
{
    AC_OP (ABSX, |);
}



static void OPC_6502_1E (void)
/* Opcode $1E: ASL abs,x */
{
    MEM_OP (ABSX, ASL);
}



static void OPC_65C02_1E (void)
/* Opcode $1E: ASL abs,x */
{
    MEM_OP (ABSX_NP, ASL);
}



static void OPC_6502_1F (void)
/* Opcode $1F: SLO abs,x */
{
    ILLx2_OP (ABSX, SLO);
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
    AC_OP (ZPXIND, &);
}



static void OPC_6502_23 (void)
/* Opcode $23: RLA (zp,x) */
{
    ILLx2_OP (ZPXIND, RLA);
}



static void OPC_6502_24 (void)
{
/* Opcode $24: BIT zp */
    ALU_OP (ZP, BIT);
}



static void OPC_6502_25 (void)
/* Opcode $25: AND zp */
{
    AC_OP (ZP, &);
}



static void OPC_6502_26 (void)
/* Opcode $26: ROL zp */
{
    MEM_OP (ZP, ROL);
}



static void OPC_6502_27 (void)
/* Opcode $27: RLA zp */
{
    ILLx2_OP (ZP, RLA);
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
    ALU_OP (ABS, BIT);
}



static void OPC_6502_2D (void)
/* Opcode $2D: AND abs */
{
    AC_OP (ABS, &);
}



static void OPC_6502_2E (void)
/* Opcode $2E: ROL abs */
{
    MEM_OP (ABS, ROL);
}



static void OPC_6502_2F (void)
/* Opcode $2F: RLA abs */
{
    ILLx2_OP (ABS, RLA);
}



static void OPC_6502_30 (void)
/* Opcode $30: BMI */
{
    BRANCH (GET_SF ());
}



static void OPC_6502_31 (void)
/* Opcode $31: AND (zp),y */
{
    AC_OP (ZPINDY, &);
}



static void OPC_65SC02_32 (void)
/* Opcode $32: AND (zp) */
{
    AC_OP (ZPIND, &);
}



static void OPC_6502_33 (void)
/* Opcode $33: RLA (zp),y */
{
    ILLx2_OP (ZPINDY, RLA);
}



static void OPC_65SC02_34 (void)
/* Opcode $34: BIT zp,x */
{
    ALU_OP (ZPX, BIT);
}



static void OPC_6502_35 (void)
/* Opcode $35: AND zp,x */
{
    AC_OP (ZPX, &);
}



static void OPC_6502_36 (void)
/* Opcode $36: ROL zp,x */
{
    MEM_OP (ZPX, ROL);
}



static void OPC_6502_37 (void)
/* Opcode $37: RLA zp,x */
{
    ILLx2_OP (ZPX, RLA);
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
    AC_OP (ABSY, &);
}



static void OPC_65SC02_3A (void)
/* Opcode $3A: DEC a */
{
    Cycles = 2;
    DEC (Regs.AC);
    Regs.PC += 1;
}



static void OPC_6502_3B (void)
/* Opcode $3B: RLA abs,y */
{
    ILLx2_OP (ABSY, RLA);
}



static void OPC_65SC02_3C (void)
/* Opcode $3C: BIT abs,x */
{
    ALU_OP (ABSX, BIT);
}



static void OPC_6502_3D (void)
/* Opcode $3D: AND abs,x */
{
    AC_OP (ABSX, &);
}



static void OPC_6502_3E (void)
/* Opcode $3E: ROL abs,x */
{
    MEM_OP (ABSX, ROL);
}



static void OPC_65C02_3E (void)
/* Opcode $3E: ROL abs,x */
{
    MEM_OP (ABSX_NP, ROL);
}



static void OPC_6502_3F (void)
/* Opcode $3B: RLA abs,x */
{
    ILLx2_OP (ABSX, RLA);
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
    AC_OP (ZPXIND, ^);
}



static void OPC_6502_43 (void)
/* Opcode $43: SRE (zp,x) */
{
    ILLx2_OP (ZPXIND, SRE);
}



static void OPC_6502_45 (void)
/* Opcode $45: EOR zp */
{
    AC_OP (ZP, ^);
}



static void OPC_6502_46 (void)
/* Opcode $46: LSR zp */
{
    MEM_OP (ZP, LSR);
}



static void OPC_6502_47 (void)
/* Opcode $47: SRE zp */
{
    ILLx2_OP (ZP, SRE);
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
    LSR (Regs.AC);
    Regs.PC += 1;
}



static void OPC_6502_4B (void)
/* Opcode $4B: ASR imm */
{
    ALU_OP_IMM (ASR);
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
    AC_OP (ABS, ^);
}



static void OPC_6502_4E (void)
/* Opcode $4E: LSR abs */
{
    MEM_OP (ABS, LSR);
}



static void OPC_6502_4F (void)
/* Opcode $4F: SRE abs */
{
    ILLx2_OP (ABS, SRE);
}



static void OPC_6502_50 (void)
/* Opcode $50: BVC */
{
    BRANCH (!GET_OF ());
}



static void OPC_6502_51 (void)
/* Opcode $51: EOR (zp),y */
{
    AC_OP (ZPINDY, ^);
}



static void OPC_65SC02_52 (void)
/* Opcode $52: EOR (zp) */
{
    AC_OP (ZPIND, ^);
}



static void OPC_6502_53 (void)
/* Opcode $43: SRE (zp),y */
{
    ILLx2_OP (ZPINDY, SRE);
}



static void OPC_6502_55 (void)
/* Opcode $55: EOR zp,x */
{
    AC_OP (ZPX, ^);
}



static void OPC_6502_56 (void)
/* Opcode $56: LSR zp,x */
{
    MEM_OP (ZPX, LSR);
}



static void OPC_6502_57 (void)
/* Opcode $57: SRE zp,x */
{
    ILLx2_OP (ZPX, SRE);
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
    AC_OP (ABSY, ^);
}



static void OPC_65SC02_5A (void)
/* Opcode $5A: PHY */
{
    Cycles = 3;
    PUSH (Regs.YR);
    Regs.PC += 1;
}



static void OPC_6502_5B (void)
/* Opcode $5B: SRE abs,y */
{
    ILLx2_OP (ABSY, SRE);
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
    AC_OP (ABSX, ^);
}



static void OPC_6502_5E (void)
/* Opcode $5E: LSR abs,x */
{
    MEM_OP (ABSX, LSR);
}



static void OPC_65C02_5E (void)
/* Opcode $5E: LSR abs,x */
{
    MEM_OP (ABSX_NP, LSR);
}



static void OPC_6502_5F (void)
/* Opcode $5F: SRE abs,x */
{
    ILLx2_OP (ABSX, SRE);
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
    ALU_OP (ZPXIND, ADC);
}



static void OPC_6502_63 (void)
/* Opcode $63: RRA (zp,x) */
{
    ILLx2_OP (ZPXIND, RRA);
}



static void OPC_65SC02_64 (void)
/* Opcode $64: STZ zp */
{
    STO_OP (ZP, 0);
}



static void OPC_6502_65 (void)
/* Opcode $65: ADC zp */
{
    ALU_OP (ZP, ADC);
}



static void OPC_6502_66 (void)
/* Opcode $66: ROR zp */
{
    MEM_OP (ZP, ROR);
}



static void OPC_6502_67 (void)
/* Opcode $67: RRA zp */
{
    ILLx2_OP (ZP, RRA);
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
    ALU_OP_IMM (ADC);
}



static void OPC_6502_6A (void)
/* Opcode $6A: ROR a */
{
    Cycles = 2;
    ROR (Regs.AC);
    Regs.PC += 1;
}



static void OPC_6502_6B (void)
/* Opcode $6B: ARR imm */
{
    ALU_OP_IMM (ARR);
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
    ALU_OP (ABS, ADC);
}



static void OPC_6502_6E (void)
/* Opcode $6E: ROR abs */
{
    MEM_OP (ABS, ROR);
}



static void OPC_6502_6F (void)
/* Opcode $6F: RRA abs */
{
    ILLx2_OP (ABS, RRA);
}



static void OPC_6502_70 (void)
/* Opcode $70: BVS */
{
    BRANCH (GET_OF ());
}



static void OPC_6502_71 (void)
/* Opcode $71: ADC (zp),y */
{
    ALU_OP (ZPINDY, ADC);
}



static void OPC_65SC02_72 (void)
/* Opcode $72: ADC (zp) */
{
    ALU_OP (ZPIND, ADC);
}



static void OPC_6502_73 (void)
/* Opcode $73: RRA (zp),y */
{
    ILLx2_OP (ZPINDY, RRA);
}



static void OPC_65SC02_74 (void)
/* Opcode $74: STZ zp,x */
{
    STO_OP (ZPX, 0);
}



static void OPC_6502_75 (void)
/* Opcode $75: ADC zp,x */
{
    ALU_OP (ZPX, ADC);
}



static void OPC_6502_76 (void)
/* Opcode $76: ROR zp,x */
{
    MEM_OP (ZPX, ROR);
}



static void OPC_6502_77 (void)
/* Opcode $77: RRA zp,x */
{
    ILLx2_OP (ZPX, RRA);
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
    ALU_OP (ABSY, ADC);
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



static void OPC_6502_7B (void)
/* Opcode $7B: RRA abs,y */
{
    ILLx2_OP (ABSY, RRA);
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
    ALU_OP (ABSX, ADC);
}



static void OPC_6502_7E (void)
/* Opcode $7E: ROR abs,x */
{
    MEM_OP (ABSX, ROR);
}



static void OPC_65C02_7E (void)
/* Opcode $7E: ROR abs,x */
{
    MEM_OP (ABSX_NP, ROR);
}



static void OPC_6502_7F (void)
/* Opcode $7F: RRA abs,x */
{
    ILLx2_OP (ABSX, RRA);
}



/* Aliases of opcode $80 */
#define OPC_6502_82 OPC_6502_80
#define OPC_6502_C2 OPC_6502_80
#define OPC_6502_E2 OPC_6502_80
#define OPC_6502_89 OPC_6502_80

static void OPC_6502_80 (void)
/* Opcode $80: NOP imm */
{
    ALU_OP_IMM (NOP);
}



static void OPC_65SC02_80 (void)
/* Opcode $80: BRA */
{
    BRANCH (1);
}



static void OPC_6502_81 (void)
/* Opcode $81: STA (zp,x) */
{
    STO_OP (ZPXIND, Regs.AC);
}



static void OPC_6502_83 (void)
/* Opcode $83: SAX (zp,x) */
{
    STO_OP (ZPXIND, Regs.AC & Regs.XR);
}



static void OPC_6502_84 (void)
/* Opcode $84: STY zp */
{
    STO_OP (ZP, Regs.YR);
}



static void OPC_6502_85 (void)
/* Opcode $85: STA zp */
{
    STO_OP (ZP, Regs.AC);
}



static void OPC_6502_86 (void)
/* Opcode $86: STX zp */
{
    STO_OP (ZP, Regs.XR);
}



static void OPC_6502_87 (void)
/* Opcode $87: SAX zp */
{
    STO_OP (ZP, Regs.AC & Regs.XR);
}



static void OPC_6502_88 (void)
/* Opcode $88: DEY */
{
    Cycles = 2;
    DEC (Regs.YR);
    Regs.PC += 1;
}



static void OPC_65SC02_89 (void)
/* Opcode $89: BIT #imm */
{
    ALU_OP_IMM (BIT);
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



static void OPC_6502_8B (void)
/* Opcode $8B: ANE imm */
{
    ALU_OP_IMM (ANE);
}



static void OPC_6502_8C (void)
/* Opcode $8C: STY abs */
{
    STO_OP (ABS, Regs.YR);
}



static void OPC_6502_8D (void)
/* Opcode $8D: STA abs */
{
    STO_OP (ABS, Regs.AC);
}



static void OPC_6502_8E (void)
/* Opcode $8E: STX abs */
{
    STO_OP (ABS, Regs.XR);
}



static void OPC_6502_8F (void)
/* Opcode $8F: SAX abs */
{
    STO_OP (ABS, Regs.AC & Regs.XR);
}



static void OPC_6502_90 (void)
/* Opcode $90: BCC */
{
    BRANCH (!GET_CF ());
}



static void OPC_6502_91 (void)
/* Opcode $91: sta (zp),y */
{
    STO_OP (ZPINDY, Regs.AC);
}



static void OPC_65SC02_92 (void)
/* Opcode $92: sta (zp) */
{
    STO_OP (ZPIND, Regs.AC);
}



static void OPC_6502_93 (void)
/* Opcode $93: SHA (zp),y */
{
    STO_CB (ZPINDY, SHA);
}



static void OPC_6502_94 (void)
/* Opcode $94: STY zp,x */
{
    STO_OP (ZPX, Regs.YR);
}



static void OPC_6502_95 (void)
/* Opcode $95: STA zp,x */
{
    STO_OP (ZPX, Regs.AC);
}



static void OPC_6502_96 (void)
/* Opcode $96: stx zp,y */
{
    STO_OP (ZPY, Regs.XR);
}



static void OPC_6502_97 (void)
/* Opcode $97: SAX zp,y */
{
    STO_OP (ZPY, Regs.AC & Regs.XR);
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
    STO_OP (ABSY, Regs.AC);
}



static void OPC_6502_9A (void)
/* Opcode $9A: TXS */
{
    Cycles = 2;
    Regs.SP = Regs.XR;
    Regs.PC += 1;
}



static void OPC_6502_9B (void)
/* Opcode $9B: TAS abs,y */
{
    STO_CB (ABSY, TAS);
}



static void OPC_6502_9C (void)
/* Opcode $9D: SHY abs,x */
{
    STO_OP (ABSX, Regs.YR & ((address >> 8) + 1));
}



static void OPC_65SC02_9C (void)
/* Opcode $9C: STZ abs */
{
    STO_OP (ABS, 0);
}



static void OPC_6502_9D (void)
/* Opcode $9D: STA abs,x */
{
    STO_OP (ABSX, Regs.AC);
}



static void OPC_6502_9E (void)
/* Opcode $9E: SHX abs,x */
{
    STO_OP (ABSY, Regs.XR & ((address >> 8) + 1));
}



static void OPC_6502_9F (void)
/* Opcode $9F: SHA abs,y */
{
    STO_CB (ABSY, SHA);
}



static void OPC_65SC02_9E (void)
/* Opcode $9E: STZ abs,x */
{
    STO_OP (ABSX, 0);
}



static void OPC_6502_A0 (void)
/* Opcode $A0: LDY #imm */
{
    ALU_OP_IMM (LDY);
}



static void OPC_6502_A1 (void)
/* Opcode $A1: LDA (zp,x) */
{
    ALU_OP (ZPXIND, LDA);
}



static void OPC_6502_A2 (void)
/* Opcode $A2: LDX #imm */
{
    ALU_OP_IMM (LDX);
}



static void OPC_6502_A3 (void)
/* Opcode $A3: LAX (zp,x) */
{
    ALU_OP (ZPXIND, LAX);
}



static void OPC_6502_A4 (void)
/* Opcode $A4: LDY zp */
{
    ALU_OP (ZP, LDY);
}



static void OPC_6502_A5 (void)
/* Opcode $A5: LDA zp */
{
    ALU_OP (ZP, LDA);
}



static void OPC_6502_A6 (void)
/* Opcode $A6: LDX zp */
{
    ALU_OP (ZP, LDX);
}



static void OPC_6502_A7 (void)
/* Opcode $A7: LAX zp */
{
    ALU_OP (ZP, LAX);
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
    ALU_OP_IMM (LDA);
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



static void OPC_6502_AB (void)
/* Opcode $AB: LXA imm */
{
    ALU_OP_IMM (LXA);
}



static void OPC_6502_AC (void)
/* Opcode $Regs.AC: LDY abs */
{
    ALU_OP (ABS, LDY);
}



static void OPC_6502_AD (void)
/* Opcode $AD: LDA abs */
{
    ALU_OP (ABS, LDA);
}



static void OPC_6502_AE (void)
/* Opcode $AE: LDX abs */
{
    ALU_OP (ABS, LDX);
}



static void OPC_6502_AF (void)
/* Opcode $AF: LAX abs */
{
    ALU_OP (ABS, LAX);
}



static void OPC_6502_B0 (void)
/* Opcode $B0: BCS */
{
    BRANCH (GET_CF ());
}



static void OPC_6502_B1 (void)
/* Opcode $B1: LDA (zp),y */
{
    ALU_OP (ZPINDY, LDA);
}



static void OPC_65SC02_B2 (void)
/* Opcode $B2: LDA (zp) */
{
    ALU_OP (ZPIND, LDA);
}



static void OPC_6502_B3 (void)
/* Opcode $B3: LAX (zp),y */
{
    ALU_OP (ZPINDY, LAX);
}



static void OPC_6502_B4 (void)
/* Opcode $B4: LDY zp,x */
{
    ALU_OP (ZPX, LDY);
}



static void OPC_6502_B5 (void)
/* Opcode $B5: LDA zp,x */
{
    ALU_OP (ZPX, LDA);
}



static void OPC_6502_B6 (void)
/* Opcode $B6: LDX zp,y */
{
    ALU_OP (ZPY, LDX);
}



static void OPC_6502_B7 (void)
/* Opcode $B7: LAX zp,y */
{
    ALU_OP (ZPY, LAX);
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
    ALU_OP (ABSY, LDA);
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



static void OPC_6502_BB (void)
/* Opcode $BB: LAS abs,y */
{
    ALU_OP (ABSY, LAS);
}



static void OPC_6502_BC (void)
/* Opcode $BC: LDY abs,x */
{
    ALU_OP (ABSX, LDY);
}



static void OPC_6502_BD (void)
/* Opcode $BD: LDA abs,x */
{
    ALU_OP (ABSX, LDA);
}



static void OPC_6502_BE (void)
/* Opcode $BE: LDX abs,y */
{
    ALU_OP (ABSY, LDX);
}



static void OPC_6502_BF (void)
/* Opcode $BF: LAX abs,y */
{
    ALU_OP (ABSY, LAX);
}



static void OPC_6502_C0 (void)
/* Opcode $C0: CPY #imm */
{
    ALU_OP_IMM (CPY);
}



static void OPC_6502_C1 (void)
/* Opcode $C1: CMP (zp,x) */
{
    ALU_OP (ZPXIND, CMP);
}



static void OPC_6502_C3 (void)
/* Opcode $C3: DCP (zp,x) */
{
    MEM_OP (ZPXIND, DCP);
}



static void OPC_6502_C4 (void)
/* Opcode $C4: CPY zp */
{
    ALU_OP (ZP, CPY);
}



static void OPC_6502_C5 (void)
/* Opcode $C5: CMP zp */
{
    ALU_OP (ZP, CMP);
}



static void OPC_6502_C6 (void)
/* Opcode $C6: DEC zp */
{
    MEM_OP (ZP, DEC);
}



static void OPC_6502_C7 (void)
/* Opcode $C7: DCP zp */
{
    MEM_OP (ZP, DCP);
}



static void OPC_6502_C8 (void)
/* Opcode $C8: INY */
{
    Cycles = 2;
    INC(Regs.YR);
    Regs.PC += 1;
}



static void OPC_6502_C9 (void)
/* Opcode $C9: CMP #imm */
{
    ALU_OP_IMM (CMP);
}



static void OPC_6502_CA (void)
/* Opcode $CA: DEX */
{
    Cycles = 2;
    DEC (Regs.XR);
    Regs.PC += 1;
}



static void OPC_6502_CB (void)
/* Opcode $CB: SBX imm */
{
    ALU_OP_IMM (SBX);
}



static void OPC_6502_CC (void)
/* Opcode $CC: CPY abs */
{
    ALU_OP (ABS, CPY);
}



static void OPC_6502_CD (void)
/* Opcode $CD: CMP abs */
{
    ALU_OP (ABS, CMP);
}



static void OPC_6502_CE (void)
/* Opcode $CE: DEC abs */
{
    MEM_OP (ABS, DEC);
}



static void OPC_6502_CF (void)
/* Opcode $CF: DCP abs */
{
    MEM_OP (ABS, DCP);
}



static void OPC_6502_D0 (void)
/* Opcode $D0: BNE */
{
    BRANCH (!GET_ZF ());
}



static void OPC_6502_D1 (void)
/* Opcode $D1: CMP (zp),y */
{
    ALU_OP (ZPINDY, CMP);
}



static void OPC_65SC02_D2 (void)
/* Opcode $D2: CMP (zp) */
{
    ALU_OP (ZPIND, CMP);
}



static void OPC_6502_D3 (void)
/* Opcode $D3: DCP (zp),y */
{
    MEM_OP (ZPINDY, DCP);
}



static void OPC_6502_D5 (void)
/* Opcode $D5: CMP zp,x */
{
    ALU_OP (ZPX, CMP);
}



static void OPC_6502_D6 (void)
/* Opcode $D6: DEC zp,x */
{
    MEM_OP (ZPX, DEC);
}



static void OPC_6502_D7 (void)
/* Opcode $D7: DCP zp,x */
{
    MEM_OP (ZPX, DCP);
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
    ALU_OP (ABSY, CMP);
}



static void OPC_65SC02_DA (void)
/* Opcode $DA: PHX */
{
    Cycles = 3;
    PUSH (Regs.XR);
    Regs.PC += 1;
}



static void OPC_6502_DB (void)
/* Opcode $DB: DCP abs,y */
{
    MEM_OP (ABSY, DCP);
}



static void OPC_6502_DD (void)
/* Opcode $DD: CMP abs,x */
{
    ALU_OP (ABSX, CMP);
}



static void OPC_6502_DE (void)
/* Opcode $DE: DEC abs,x */
{
    MEM_OP (ABSX, DEC);
}



static void OPC_6502_DF (void)
/* Opcode $DF: DCP abs,x */
{
    MEM_OP (ABSX, DCP);
}



static void OPC_6502_E0 (void)
/* Opcode $E0: CPX #imm */
{
    ALU_OP_IMM (CPX);
}



static void OPC_6502_E1 (void)
/* Opcode $E1: SBC (zp,x) */
{
    ALU_OP (ZPXIND, SBC);
}



static void OPC_6502_E3 (void)
/* Opcode $E3: ISC (zp,x) */
{
    MEM_OP (ZPXIND, ISC);
}



static void OPC_6502_E4 (void)
/* Opcode $E4: CPX zp */
{
    ALU_OP (ZP, CPX);
}



static void OPC_6502_E5 (void)
/* Opcode $E5: SBC zp */
{
    ALU_OP (ZP, SBC);
}



static void OPC_6502_E6 (void)
/* Opcode $E6: INC zp */
{
    MEM_OP (ZP, INC);
}



static void OPC_6502_E7 (void)
/* Opcode $E7: ISC zp */
{
    MEM_OP (ZP, ISC);
}



static void OPC_6502_E8 (void)
/* Opcode $E8: INX */
{
    Cycles = 2;
    INC (Regs.XR);
    Regs.PC += 1;
}



/* Aliases of opcode $EA */
#define OPC_6502_EB OPC_6502_E9

static void OPC_6502_E9 (void)
/* Opcode $E9: SBC #imm */
{
    ALU_OP_IMM (SBC);
}



/* Aliases of opcode $EA */
#define OPC_6502_1A OPC_6502_EA
#define OPC_6502_3A OPC_6502_EA
#define OPC_6502_5A OPC_6502_EA
#define OPC_6502_7A OPC_6502_EA
#define OPC_6502_DA OPC_6502_EA
#define OPC_6502_FA OPC_6502_EA

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
    ALU_OP (ABS, CPX);
}



static void OPC_6502_ED (void)
/* Opcode $ED: SBC abs */
{
    ALU_OP (ABS, SBC);
}


static void OPC_6502_EE (void)
/* Opcode $EE: INC abs */
{
    MEM_OP (ABS, INC);
}



static void OPC_6502_EF (void)
/* Opcode $EF: ISC abs */
{
    MEM_OP (ABS, ISC);
}



static void OPC_6502_F0 (void)
/* Opcode $F0: BEQ */
{
    BRANCH (GET_ZF ());
}



static void OPC_6502_F1 (void)
/* Opcode $F1: SBC (zp),y */
{
    ALU_OP (ZPINDY, SBC);
}



static void OPC_65SC02_F2 (void)
/* Opcode $F2: SBC (zp) */
{
    ALU_OP (ZPIND, SBC);
}



static void OPC_6502_F3 (void)
/* Opcode $F3: ISC (zp),y */
{
    MEM_OP (ZPINDY, ISC);
}



static void OPC_6502_F5 (void)
/* Opcode $F5: SBC zp,x */
{
    ALU_OP (ZPX, SBC);
}



static void OPC_6502_F6 (void)
/* Opcode $F6: INC zp,x */
{
    MEM_OP (ZPX, INC);
}



static void OPC_6502_F7 (void)
/* Opcode $F7: ISC zp,x */
{
    MEM_OP (ZPX, ISC);
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
    ALU_OP (ABSY, SBC);
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



static void OPC_6502_FB (void)
/* Opcode $FB: ISC abs,y */
{
    MEM_OP (ABSY, ISC);
}



static void OPC_6502_FD (void)
/* Opcode $FD: SBC abs,x */
{
    ALU_OP (ABSX, SBC);
}



static void OPC_6502_FE (void)
/* Opcode $FE: INC abs,x */
{
    MEM_OP (ABSX, INC);
}



static void OPC_6502_FF (void)
/* Opcode $FF: ISC abs,x */
{
    MEM_OP (ABSX, ISC);
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



/* Opcode handler table for the 6502X */
static const OPFunc OP6502XTable[256] = {
    OPC_6502_00,
    OPC_6502_01,
    OPC_Illegal,
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
    OPC_Illegal,
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
    OPC_Illegal,
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
    OPC_Illegal,
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
    OPC_Illegal,
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
    OPC_Illegal,
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
    OPC_Illegal,
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
    OPC_Illegal,
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
    OPC_Illegal,
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
    OPC_Illegal,
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
    OPC_Illegal,
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
    OPC_Illegal,
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
    OPC_6502_FF
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
    OPC_65C02_1E,
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
    OPC_65C02_3E,
    OPC_Illegal,        // $3F: BBR3 currently unsupported
    OPC_6502_40,
    OPC_6502_41,
    OPC_65C02_NOP22,    // $42
    OPC_65C02_NOP11,    // $43
    OPC_6502_44,        // $44
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
    OPC_65C02_5E,
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
    OPC_65C02_7E,
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
static const OPFunc* Handlers[3] = {
    OP6502Table,
    OP65C02Table,
    OP6502XTable
};



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
