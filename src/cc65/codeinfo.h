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

/* Defines for registers. */
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



/* Zero page register info */
typedef struct ZPInfo ZPInfo;
struct ZPInfo {
    unsigned char  Len;         /* Length of the following string */
    char           Name[11];    /* Name of zero page symbol */
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
    CMP_ULE
} cmp_t;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void GetFuncInfo (const char* Name, unsigned short* Use, unsigned short* Chg);
/* For the given function, lookup register information and store it into
** the given variables. If the function is unknown, assume it will use and
** load all registers.
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



/* End of codeinfo.h */

#endif
