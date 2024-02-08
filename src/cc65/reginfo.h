/*****************************************************************************/
/*                                                                           */
/*                                 reginfo.h                                 */
/*                                                                           */
/*                        6502 register tracking info                        */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001-2003 Ullrich von Bassewitz                                       */
/*               Roemerstrasse 52                                            */
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



#ifndef REGINFO_H
#define REGINFO_H



#include <stdio.h>      /* ### */

/* common */
#include "inline.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Encoding for an unknown register value */
#define UNKNOWN_REGVAL  -1

/* Encoding for an unknown processor status:
** For Bit N in the flags, ((flags >> N) & 0x0101) == 0x0101 means 'unknown'
*/
#define UNKNOWN_PFVAL_C     0x0101U     /* Carray */
#define UNKNOWN_PFVAL_Z     0x0202U     /* Zero */
#define UNKNOWN_PFVAL_I     0x0404U     /* Interrupt */
#define UNKNOWN_PFVAL_D     0x0808U     /* Decimal */
#define UNKNOWN_PFVAL_U     0x1010U     /* Unused */
#define UNKNOWN_PFVAL_B     0x2020U     /* Break */
#define UNKNOWN_PFVAL_V     0x4040U     /* Overflow */
#define UNKNOWN_PFVAL_N     0x8080U     /* Negative */
#define UNKNOWN_PFVAL_CZ    (UNKNOWN_PFVAL_C | UNKNOWN_PFVAL_Z)
#define UNKNOWN_PFVAL_CZN   (UNKNOWN_PFVAL_N | UNKNOWN_PFVAL_CZ)
#define UNKNOWN_PFVAL_CZVN  (UNKNOWN_PFVAL_V | UNKNOWN_PFVAL_CZN)
#define UNKNOWN_PFVAL_ZN    (UNKNOWN_PFVAL_Z | UNKNOWN_PFVAL_N)
#define UNKNOWN_PFVAL_ZVN   (UNKNOWN_PFVAL_V | UNKNOWN_PFVAL_ZN)
#define UNKNOWN_PFVAL_6502  0xE7E7U
#define UNKNOWN_PFVAL_ALL   0xFFFFU

/* Encoding for a known processor status */
#define PFVAL_C     0x0001U     /* Carray set */
#define PFVAL_Z     0x0002U     /* Zero set */
#define PFVAL_I     0x0004U     /* Interrupt set */
#define PFVAL_D     0x0008U     /* Decimal set */
#define PFVAL_U     0x0010U     /* Unused set */
#define PFVAL_B     0x0020U     /* Break set */
#define PFVAL_V     0x0040U     /* Overflow set */
#define PFVAL_N     0x0080U     /* Negative set */
#define PFVAL_CZ    (PFVAL_C | PFVAL_Z)
#define PFVAL_CZN   (PFVAL_N | PFVAL_CZ)
#define PFVAL_CZVN  (PFVAL_V | PFVAL_CZN)
#define PFVAL_ZN    (PFVAL_Z | PFVAL_N)
#define PFVAL_ZVN   (PFVAL_V | PFVAL_ZN)
#define PFVAL_6502  0x00E7U
#define PFVAL_ALL   0x00FFU

/* Used for functions to convert the processor states to processor flags */
#define PSTATE_BITS_SHIFT  24
#define PSTATE_BITS_MASK    (0xFFU << PSTATE_BITS_SHIFT)

/* Encoding for unknown Z/N status origin */
#define UNKNOWN_ZNREG  0x0000U

/* Encoding for known register Z/N status origins */
#define ZNREG_NONE      0x0000U     /* None */
#define ZNREG_A         REG_A
#define ZNREG_X         REG_X
#define ZNREG_Y         REG_Y
#define ZNREG_TMP1      REG_TMP1
#define ZNREG_PTR1_LO   REG_PTR1_LO
#define ZNREG_PTR1_HI   REG_PTR1_HI
#define ZNREG_PTR2_LO   REG_PTR2_LO
#define ZNREG_PTR2_HI   REG_PTR2_HI
#define ZNREG_SREG_LO   REG_SREG_LO
#define ZNREG_SREG_HI   REG_SREG_HI
#define ZNREG_SAVE_LO   REG_SAVE_LO
#define ZNREG_SAVE_HI   REG_SAVE_HI
#define ZNREG_AX        (REG_A | REG_X)
#define ZNREG_AY        (REG_A | REG_Y)
#define ZNREG_AXY       (REG_A | REG_X | REG_Y)



/* Register contents */
typedef struct RegContents RegContents;
struct RegContents {
    short       RegA;
    short       RegX;
    short       RegY;
    short       SRegLo;
    short       SRegHi;
    short       Ptr1Lo;
    short       Ptr1Hi;
    short       Tmp1;
    unsigned short PFlags;  /* Processor flags */
    unsigned short ZNRegs;  /* Which register(s) the Z/N flags reflect */
};

/* Register change info */
typedef struct RegInfo RegInfo;
struct RegInfo {
    RegContents In;             /* Incoming register values */
    RegContents Out;            /* Outgoing register values */
    RegContents Out2;           /* Alternative outgoing reg values for branches */
};



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void RC_Invalidate (RegContents* C);
/* Invalidate all registers */

void RC_InvalidateZP (RegContents* C);
/* Invalidate all ZP registers */

void RC_InvalidatePS (RegContents* C);
/* Invalidate processor status */

void RC_Dump (FILE* F, const RegContents* RC);
/* Dump the contents of the given RegContents struct */

#if defined(HAVE_INLINE)
INLINE int RegValIsKnown (short Val)
/* Return true if the register value is known */
{
    return (Val >= 0);
}
#else
#  define RegValIsKnown(S)      ((S) >= 0)
#endif

#if defined(HAVE_INLINE)
INLINE int RegValIsUnknown (short Val)
/* Return true if the register value is not known */
{
    return (Val < 0);
}
#else
#  define RegValIsUnknown(S)      ((S) < 0)
#endif

#if defined(HAVE_INLINE)
INLINE int PStatesAreKnown (unsigned short PFlags, unsigned WhatStates)
/* Return true if all queried processor states are known.
** Note: WhatStates takes PSTATE_* rather than PFVAL_*.
*/
{
    return ((PFlags << (PSTATE_BITS_SHIFT - 8)) & WhatStates & PSTATE_BITS_MASK) == 0;
}
#else
int PStatesAreKnown (unsigned short PFlags, unsigned WhatStates);
#endif

#if defined(HAVE_INLINE)
INLINE int PStatesAreUnknown (unsigned short PFlags, unsigned WhatStates)
/* Return true if any queried processor states are unknown.
** Note: WhatStates takes PSTATE_* rather than PFVAL_*.
*/
{
    return !PStatesAreKnown (PFlags, WhatStates);
}
#else
#  define PStatesAreUnknown(V, B)      (!PStatesAreKnown (V, B))
#endif

#if defined(HAVE_INLINE)
INLINE int PStatesAreSet (unsigned short PFlags, unsigned WhatStates)
/* Return true if all queried processor states are known to be set.
** Note: WhatStates takes PSTATE_* rather than PFVAL_*.
*/
{
    return (PFlags & (WhatStates >> (PSTATE_BITS_SHIFT - 8))) == 0 &&
           (PFlags & (WhatStates >> PSTATE_BITS_SHIFT)) == WhatStates >> PSTATE_BITS_SHIFT;
}
#else
int PStatesAreSet (unsigned short PFlags, unsigned WhatStates);
#endif

#if defined(HAVE_INLINE)
INLINE int PStatesAreClear (unsigned short PFlags, unsigned WhatStates)
/* Return true if the queried processor states are known to be cleared.
** Note: WhatStates takes PSTATE_* rather than PFVAL_*.
*/
{
    return (PFlags & (WhatStates >> (PSTATE_BITS_SHIFT - 8))) == 0 &&
           (PFlags & (WhatStates >> PSTATE_BITS_SHIFT)) == 0;
}
#else
int PStatesAreClear (unsigned short PFlags, unsigned WhatStates);
#endif

RegInfo* NewRegInfo (const RegContents* RC);
/* Allocate a new register info, initialize and return it. If RC is not
** a NULL pointer, it is used to initialize both, the input and output
** registers. If the pointer is NULL, all registers are set to unknown.
*/

void FreeRegInfo (RegInfo* RI);
/* Free a RegInfo struct */

void DumpRegInfo (const char* Desc, const RegInfo* RI);
/* Dump the register info for debugging */



/* End of reginfo.h */

#endif
