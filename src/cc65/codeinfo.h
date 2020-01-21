/*****************************************************************************/
/*                                                                           */
/*                                codeinfo.h                                 */
/*                                                                           */
/*                  Additional information about 6502 code                   */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001-2002 Ullrich von Bassewitz                                       */
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



#ifndef CODEINFO_H
#define CODEINFO_H



/*****************************************************************************/
/*                                 Forwards                                  */
/*****************************************************************************/



struct CodeSeg;



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Forward to struct RegContents */
struct RegContents;

/* Defines for registers */
#define REG_NONE        0x0000U
#define REG_A           0x0001U
#define REG_X           0x0002U
#define REG_Y           0x0004U
#define REG_TMP1        0x0008U
#define REG_PTR1_LO     0x0010U
#define REG_PTR1_HI     0x0020U
#define REG_PTR2_LO     0x0040U
#define REG_PTR2_HI     0x0080U
#define REG_SREG_LO     0x0100U
#define REG_SREG_HI     0x0200U
#define REG_SAVE_LO     0x0400U
#define REG_SAVE_HI     0x0800U
#define REG_SP_LO       0x1000U
#define REG_SP_HI       0x2000U

/* Defines for some special register usage */
#define SLV_SP65        0x00200000U     /* Accesses 6502 stack pointer */
#define SLV_PH65        0x00400000U     /* Pushes onto 6502 stack */
#define SLV_PL65        0x00800000U     /* Pops from 6502 stack */

/* Defines for processor states */
#define PSTATE_NONE     0x00000000U
#define PSTATE_C        0x01000000U     /* Carry */
#define PSTATE_Z        0x02000000U     /* Zero */
#define PSTATE_I        0x04000000U     /* Interrupt */
#define PSTATE_D        0x08000000U     /* Decimal */
#define PSTATE_U        0x10000000U     /* Unused */
#define PSTATE_B        0x20000000U     /* Break */
#define PSTATE_V        0x40000000U     /* Overflow */
#define PSTATE_N        0x80000000U     /* Negative */

/* Combined register defines */
#define REG_PTR1        (REG_PTR1_LO | REG_PTR1_HI)
#define REG_PTR2        (REG_PTR2_LO | REG_PTR2_HI)
#define REG_SREG        (REG_SREG_LO | REG_SREG_HI)
#define REG_SAVE        (REG_SAVE_LO | REG_SAVE_HI)
#define REG_SP          (REG_SP_LO | REG_SP_HI)
#define REG_AX          (REG_A | REG_X)
#define REG_AY          (REG_A | REG_Y)
#define REG_XY          (REG_X | REG_Y)
#define REG_AXY         (REG_AX | REG_Y)
#define REG_EAX         (REG_AX | REG_SREG)
#define REG_EAXY        (REG_EAX | REG_Y)
#define REG_ZP          0xFFF8U
#define REG_ALL         0xFFFFU
#define PSTATE_CZ       (PSTATE_C | PSTATE_Z)
#define PSTATE_CZN      (PSTATE_C | PSTATE_Z | PSTATE_N)
#define PSTATE_CZVN     (PSTATE_C | PSTATE_Z | PSTATE_V | PSTATE_N)
#define PSTATE_ZN       (PSTATE_Z | PSTATE_N)
#define PSTATE_ZVN      (PSTATE_Z | PSTATE_V | PSTATE_N)
#define PSTATE_6502     0xE7000000U
#define PSTATE_ALL      0xFF000000U
#define REG_EVERYTHING  0xFFFFFFFFU



/* Zero page register info */
typedef struct ZPInfo ZPInfo;
struct ZPInfo {
    unsigned char  Len;         /* Length of the following string */
    char           Name[10];    /* Name of zero page symbol */
    unsigned char  Size;        /* Maximum buffer size of this register */
    unsigned short ByteUse;     /* Register info for this symbol */
    unsigned short WordUse;     /* Register info for 16 bit access */
};



/* Defines for the conditions in a compare */
typedef enum {
    CMP_INV = -1,
    CMP_EQ,
    CMP_NE,
    CMP_GT,
    CMP_GE,
    CMP_LT,
    CMP_LE,
    CMP_UGT,
    CMP_UGE,
    CMP_ULT,
    CMP_ULE,

    /* End of the enumeration */
    CMP_END
} cmp_t;



/* Defines for the conditions in a compare */
typedef enum {
    FNCLS_UNKNOWN = -1, /* Unknown */
    FNCLS_BUILTIN,      /* Builtin */
    FNCLS_GLOBAL,       /* Found in global sym table minus the leading underscore */
    FNCLS_NUMERIC       /* A call to a numeric address */
} fncls_t;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



int IsZPArg (const char* Arg);
/* Exam if the main part of the arg string indicates a ZP loc */

fncls_t GetFuncInfo (const char* Name, unsigned int* Use, unsigned int* Chg);
/* For the given function, lookup register information and store it into
** the given variables. If the function is unknown, assume it will use and
** load all registers.
** Return the whatever category the function is in.
*/

const ZPInfo* GetZPInfo (const char* Name);
/* If the given name is a zero page symbol, return a pointer to the info
** struct for this symbol, otherwise return NULL.
*/

unsigned GetRegInfo (struct CodeSeg* S, unsigned Index, unsigned Wanted);
/* Determine register usage information for the instructions starting at the
** given index.
*/

int RegAUsed (struct CodeSeg* S, unsigned Index);
/* Check if the value in A is used. */

int RegXUsed (struct CodeSeg* S, unsigned Index);
/* Check if the value in X is used. */

int RegYUsed (struct CodeSeg* S, unsigned Index);
/* Check if the value in Y is used. */

int RegAXUsed (struct CodeSeg* S, unsigned Index);
/* Check if the value in A or(!) the value in X are used. */

int RegEAXUsed (struct CodeSeg* S, unsigned Index);
/* Check if any of the four bytes in EAX are used. */

unsigned GetKnownReg (unsigned Use, const struct RegContents* RC);
/* Return the register or zero page location from the set in Use, thats
** contents are known. If Use does not contain any register, or if the
** register in question does not have a known value, return REG_NONE.
*/

cmp_t FindBoolCmpCond (const char* Name);
/* Check if the given string is the name of one of the boolean transformer
** subroutine, and if so, return the condition that is evaluated by this
** routine. Return CMP_INV if the condition is not recognised.
*/

cmp_t FindTosCmpCond (const char* Name);
/* Check if this is a call to one of the TOS compare functions (tosgtax).
** Return the condition code or CMP_INV on failure.
*/

const char* GetBoolTransformer (cmp_t Cond);
/* Get the bool transformer corresponding to the given compare condition */

cmp_t GetNegatedCond (cmp_t Cond);
/* Get the logically opposite compare condition */

cmp_t GetRevertedCond (cmp_t Cond);
/* Get the compare condition in reverted order of operands */

const char* GetCmpSuffix (cmp_t Cond);
/* Return the compare suffix by the given a compare condition or 0 on failure */

char* GetBoolCmpSuffix (char* Buf, cmp_t Cond);
/* Search for a boolean transformer subroutine (eg. booleq) by the given compare
** condition.
** Return the output buffer filled with the name of the correct subroutine or 0
** on failure.
*/

char* GetTosCmpSuffix (char* Buf, cmp_t Cond);
/* Search for a TOS compare function (eg. tosgtax) by the given compare condition.
** Return the output buffer filled with the name of the correct function or 0 on
** failure.
*/

/* End of codeinfo.h */

#endif
