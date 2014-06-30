/*****************************************************************************/
/*                                                                           */
/*                               coptptrstore.c                              */
/*                                                                           */
/*                      Optimize stores through pointers                     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2012,      Ullrich von Bassewitz                                      */
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



#include <string.h>

/* common */
#include "chartype.h"
#include "strbuf.h"
#include "xmalloc.h"
#include "xsprintf.h"

/* cc65 */
#include "codeent.h"
#include "codeinfo.h"
#include "coptptrstore.h"



/*****************************************************************************/
/*                             Helper functions                              */
/*****************************************************************************/



static unsigned OptPtrStore1Sub (CodeSeg* S, unsigned I, CodeEntry** const L)
/* Check if this is one of the allowed suboperation for OptPtrStore1 */
{
    /* Check for a label attached to the entry */
    if (CE_HasLabel (L[0])) {
        return 0;
    }

    /* Check for single insn sub ops */
    if (L[0]->OPC == OP65_AND                                           ||
        L[0]->OPC == OP65_EOR                                           ||
        L[0]->OPC == OP65_ORA                                           ||
        (L[0]->OPC == OP65_JSR                          &&
         (strncmp (L[0]->Arg, "shlax", 5) == 0  ||
          strncmp (L[0]->Arg, "shrax", 5) == 0)         &&
         strlen (L[0]->Arg) == 6                        &&
         IsDigit (L[0]->Arg[5]))) {

        /* One insn */
        return 1;

    } else if (L[0]->OPC == OP65_CLC                      &&
               (L[1] = CS_GetNextEntry (S, I)) != 0       &&
               L[1]->OPC == OP65_ADC                      &&
               !CE_HasLabel (L[1])) {
        return 2;
    } else if (L[0]->OPC == OP65_SEC                      &&
               (L[1] = CS_GetNextEntry (S, I)) != 0       &&
               L[1]->OPC == OP65_SBC                      &&
               !CE_HasLabel (L[1])) {
        return 2;
    }



    /* Not found */
    return 0;
}



static const char* LoadAXZP (CodeSeg* S, unsigned I)
/* If the two instructions preceeding S/I are a load of A/X from a two byte
** zero byte location, return the name of the zero page location. Otherwise
** return NULL.
*/
{
    CodeEntry* L[2];
    unsigned Len;

    if (I >= 2                                                  &&
        CS_GetEntries (S, L, I-2, 2)                            &&
        L[0]->OPC == OP65_LDA                                   &&
        L[0]->AM == AM65_ZP                                     &&
        L[1]->OPC == OP65_LDX                                   &&
        L[1]->AM == AM65_ZP                                     &&
        !CE_HasLabel (L[1])                                     &&
        (Len = strlen (L[0]->Arg)) == strlen (L[1]->Arg) - 2    &&
        memcmp (L[0]->Arg, L[1]->Arg, Len) == 0                 &&
        L[1]->Arg[Len] == '+'                                   &&
        L[1]->Arg[Len+1] == '1') {

        /* Return the label */
        return L[0]->Arg;

    } else {

        /* Not found */
        return 0;

    }
}



static const char* LoadAXImm (CodeSeg* S, unsigned I)
/* If the instructions preceeding S/I are a load of A/X of a constant value
** or a word sized address label, return the address of the location as a
** string.
** Beware: In case of a numeric value, the result is returned in static
** storage which is overwritten with each call.
*/
{
    static StrBuf Buf = STATIC_STRBUF_INITIALIZER;
    CodeEntry* L[2];
    CodeEntry* ALoad;
    CodeEntry* XLoad;
    unsigned Len;

    /* Fetch entry at I and check if A/X is known */
    L[0] = CS_GetEntry (S, I);
    if (L[0] != 0                                               &&
        RegValIsKnown (L[0]->RI->In.RegA)                       &&
        RegValIsKnown (L[0]->RI->In.RegX)) {

        /* Numeric argument - get low and high byte */
        unsigned Lo = (L[0]->RI->In.RegA & 0xFF);
        unsigned Hi = (L[0]->RI->In.RegX & 0xFF);

        /* Format into buffer */
        SB_Printf (&Buf, "$%04X", Lo | (Hi << 8));

        /* Return the address as a string */
        return SB_GetConstBuf (&Buf);

    }

    /* Search back for the two instructions loading A and X. Abort
    ** the search if the registers are changed in any other way or
    ** if a label is reached while we don't have both loads.
    */
    ALoad = 0;
    XLoad = 0;
    while (I-- > 0) {
        /* Get next entry */
        CodeEntry* E = CS_GetEntry (S, I);

        /* Check for the loads of A and X */
        if (ALoad == 0 && E->OPC == OP65_LDA && E->AM == AM65_IMM) {
            ALoad = E;
        } else if (E->Chg & REG_A) {
            /* A is changed before we get the load */
            return 0;
        } else if (XLoad == 0 && E->OPC == OP65_LDX && E->AM == AM65_IMM) {
            XLoad = E;
        } else if (E->Chg & REG_X) {
            /* X is changed before we get the load */
            return 0;
        }

        if (ALoad != 0 && XLoad != 0) {
            /* We have both */
            break;
        }

        /* If we have a label, before both are found, bail out */
        if (CE_HasLabel (E)) {
            return 0;
        }
    }

    /* Check for a load of a label address */
    if ((Len = strlen (ALoad->Arg)) > 3                         &&
        ALoad->Arg[0] == '<'                                    &&
        ALoad->Arg[1] == '('                                    &&
        strlen (XLoad->Arg) == Len                              &&
        XLoad->Arg[0] == '>'                                    &&
        memcmp (ALoad->Arg+1, XLoad->Arg+1, Len-1) == 0) {

        /* Load of an address label */
        SB_CopyBuf (&Buf, ALoad->Arg + 2, Len - 3);
        SB_Terminate (&Buf);
        return SB_GetConstBuf (&Buf);
    }

    /* Not found */
    return 0;
}



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



unsigned OptPtrStore1 (CodeSeg* S)
/* Search for the sequence:
**
**      clc
**      adc     xxx
**      bcc     L
**      inx
** L:   jsr     pushax
**      ldx     #$00
**      lda     yyy
**      ldy     #$00
**      jsr     staspidx
**
** and replace it by:
**
**      sta     ptr1
**      stx     ptr1+1
**      ldy     xxx
**      ldx     #$00
**      lda     yyy
**      sta     (ptr1),y
**
** or by
**
**      ldy     xxx
**      ldx     #$00
**      lda     yyy
**      sta     (zp),y
**
** or by
**
**      ldy     xxx
**      ldx     #$00
**      lda     yyy
**      sta     label,y
**
** or by
**
**      ldy     xxx
**      ldx     #$00
**      lda     yyy
**      sta     $xxxx,y
**
** depending on the code preceeding the sequence above.
*/
{
    unsigned Changes = 0;
    unsigned I;

    /* Walk over the entries */
    I = 0;
    while (I < CS_GetEntryCount (S)) {

        CodeEntry* L[9];

        /* Get next entry */
        L[0] = CS_GetEntry (S, I);

        /* Check for the sequence */
        if (L[0]->OPC == OP65_CLC                               &&
            CS_GetEntries (S, L+1, I+1, 8)                      &&
            L[1]->OPC == OP65_ADC                               &&
            (L[1]->AM == AM65_ABS                       ||
             L[1]->AM == AM65_ZP                        ||
             L[1]->AM == AM65_IMM                       ||
             (L[1]->AM == AM65_ZP_INDY          &&
              RegValIsKnown (L[1]->RI->In.RegY)))               &&
            (L[2]->OPC == OP65_BCC || L[2]->OPC == OP65_JCC)    &&
            L[2]->JumpTo != 0                                   &&
            L[2]->JumpTo->Owner == L[4]                         &&
            L[3]->OPC == OP65_INX                               &&
            CE_IsCallTo (L[4], "pushax")                        &&
            L[5]->OPC == OP65_LDX                               &&
            L[6]->OPC == OP65_LDA                               &&
            L[7]->OPC == OP65_LDY                               &&
            CE_IsKnownImm (L[7], 0)                             &&
            CE_IsCallTo (L[8], "staspidx")                      &&
            !CS_RangeHasLabel (S, I+1, 3)                       &&
            !CS_RangeHasLabel (S, I+5, 4)) {

            CodeEntry* X;
            const char* Loc;
            am_t AM;

            /* Track the insertion point */
            unsigned IP = I + 9;
            if ((Loc = LoadAXZP (S, I)) != 0) {
                /* If the sequence is preceeded by a load of a ZP value,
                ** we can use this ZP value as a pointer using ZP
                ** indirect Y addressing.
                */
                AM = AM65_ZP_INDY;
            } else if ((Loc = LoadAXImm (S, I)) != 0) {
                /* If the sequence is preceeded by a load of an immediate
                ** value, we can use this absolute value as an address
                ** using absolute indexed Y addressing.
                */
                AM = AM65_ABSY;
            }

            /* If we don't have a store location, we use ptr1 with zp
            ** indirect Y addressing. We must store the value in A/X into
            ** ptr1 in this case.
            */
            if (Loc == 0) {

                /* Must use ptr1 */
                Loc = "ptr1";
                AM  = AM65_ZP_INDY;

                X = NewCodeEntry (OP65_STA, AM65_ZP, "ptr1", 0, L[8]->LI);
                CS_InsertEntry (S, X, IP++);

                X = NewCodeEntry (OP65_STX, AM65_ZP, "ptr1+1", 0, L[8]->LI);
                CS_InsertEntry (S, X, IP++);

            }

            /* If the index is loaded from (zp),y, we cannot do that directly.
            ** Note: In this case, the Y register will contain the correct
            ** value after removing the old code, so we don't need to load
            ** it here.
            */
            if (L[1]->AM == AM65_ZP_INDY) {
                X = NewCodeEntry (OP65_LDA, L[1]->AM, L[1]->Arg, 0, L[1]->LI);
                CS_InsertEntry (S, X, IP++);

                X = NewCodeEntry (OP65_TAY, AM65_IMP, 0, 0, L[1]->LI);
                CS_InsertEntry (S, X, IP++);
            } else {
                X = NewCodeEntry (OP65_LDY, L[1]->AM, L[1]->Arg, 0, L[1]->LI);
                CS_InsertEntry (S, X, IP++);
            }

            X = NewCodeEntry (OP65_LDX, L[5]->AM, L[5]->Arg, 0, L[5]->LI);
            CS_InsertEntry (S, X, IP++);

            X = NewCodeEntry (OP65_LDA, L[6]->AM, L[6]->Arg, 0, L[6]->LI);
            CS_InsertEntry (S, X, IP++);

            X = NewCodeEntry (OP65_STA, AM, Loc, 0, L[8]->LI);
            CS_InsertEntry (S, X, IP++);

            /* Remove the old code */
            CS_DelEntries (S, I, 9);

            /* Skip most of the generated replacement code */
            I += 3;

            /* Remember, we had changes */
            ++Changes;

        }

        /* Next entry */
        ++I;

    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptPtrStore2 (CodeSeg* S)
/* Search for the sequence:
**
**      clc
**      adc     xxx
**      bcc     L
**      inx
** L:   jsr     pushax
**      ldy     yyy
**      ldx     #$00
**      lda     (sp),y
**      ldy     #$00
**      jsr     staspidx
**
** and replace it by:
**
**      sta     ptr1
**      stx     ptr1+1
**      ldy     yyy-2
**      ldx     #$00
**      lda     (sp),y
**      ldy     xxx
**      sta     (ptr1),y
**
** or by
**
**      ldy     yyy-2
**      ldx     #$00
**      lda     (sp),y
**      ldy     xxx
**      sta     (zp),y
**
** or by
**
**      ldy     yyy-2
**      ldx     #$00
**      lda     (sp),y
**      ldy     xxx
**      sta     label,y
**
** or by
**
**      ldy     yyy-2
**      ldx     #$00
**      lda     (sp),y
**      ldy     xxx
**      sta     $xxxx,y
**
** depending on the code preceeding the sequence above.
*/
{
    unsigned Changes = 0;
    unsigned I;

    /* Walk over the entries */
    I = 0;
    while (I < CS_GetEntryCount (S)) {

        CodeEntry* L[10];

        /* Get next entry */
        L[0] = CS_GetEntry (S, I);

        /* Check for the sequence */
        if (L[0]->OPC == OP65_CLC                               &&
            CS_GetEntries (S, L+1, I+1, 9)                      &&
            L[1]->OPC == OP65_ADC                               &&
            (L[1]->AM == AM65_ABS                       ||
             L[1]->AM == AM65_ZP                        ||
             L[1]->AM == AM65_IMM                       ||
             (L[1]->AM == AM65_ZP_INDY          &&
              RegValIsKnown (L[1]->RI->In.RegY)))               &&
            (L[2]->OPC == OP65_BCC || L[2]->OPC == OP65_JCC)    &&
            L[2]->JumpTo != 0                                   &&
            L[2]->JumpTo->Owner == L[4]                         &&
            L[3]->OPC == OP65_INX                               &&
            CE_IsCallTo (L[4], "pushax")                        &&
            L[5]->OPC == OP65_LDY                               &&
            CE_IsConstImm (L[5])                                &&
            L[6]->OPC == OP65_LDX                               &&
            L[7]->OPC == OP65_LDA                               &&
            L[7]->AM == AM65_ZP_INDY                            &&
            strcmp (L[7]->Arg, "sp") == 0                       &&
            L[8]->OPC == OP65_LDY                               &&
            (L[8]->AM == AM65_ABS                       ||
             L[8]->AM == AM65_ZP                        ||
             L[8]->AM == AM65_IMM)                              &&
            CE_IsCallTo (L[9], "staspidx")                      &&
            !CS_RangeHasLabel (S, I+1, 3)                       &&
            !CS_RangeHasLabel (S, I+5, 5)) {

            CodeEntry* X;
            const char* Arg;
            const char* Loc;
            am_t AM;

            /* Track the insertion point */
            unsigned IP = I + 10;
            if ((Loc = LoadAXZP (S, I)) != 0) {
                /* If the sequence is preceeded by a load of a ZP value,
                ** we can use this ZP value as a pointer using ZP
                ** indirect Y addressing.
                */
                AM = AM65_ZP_INDY;
            } else if ((Loc = LoadAXImm (S, I)) != 0) {
                /* If the sequence is preceeded by a load of an immediate
                ** value, we can use this absolute value as an address
                ** using absolute indexed Y addressing.
                */
                AM = AM65_ABSY;
            }

            /* If we don't have a store location, we use ptr1 with zp
            ** indirect Y addressing. We must store the value in A/X into
            ** ptr1 in this case.
            */
            if (Loc == 0) {

                /* Must use ptr1 */
                Loc = "ptr1";
                AM  = AM65_ZP_INDY;

                X = NewCodeEntry (OP65_STA, AM65_ZP, "ptr1", 0, L[8]->LI);
                CS_InsertEntry (S, X, IP++);

                X = NewCodeEntry (OP65_STX, AM65_ZP, "ptr1+1", 0, L[8]->LI);
                CS_InsertEntry (S, X, IP++);

            }

            /* Generate four different replacements depending on the addressing
            ** mode of the store and from where the index is loaded:
            **
            ** 1. If the index is not loaded ZP indirect Y, we can use Y for
            **    the store index.
            **
            ** 2. If the index is loaded ZP indirect Y and we store absolute
            **    indexed, we need Y to load the index and will therefore
            **    use X as index for the store. The disadvantage is that we
            **    need to reload X later.
            **
            ** 3. If the index is loaded ZP indirect Y and we store ZP indirect
            **    Y, we must use Y for load and store and must therefore save
            **    the A register when loading Y the second time.
            */
            if (L[1]->AM != AM65_ZP_INDY) {

                /* Case 1 */
                Arg = MakeHexArg (L[5]->Num - 2);
                X = NewCodeEntry (OP65_LDY, AM65_IMM, Arg, 0, L[5]->LI);
                CS_InsertEntry (S, X, IP++);

                X = NewCodeEntry (OP65_LDX, L[6]->AM, L[6]->Arg, 0, L[6]->LI);
                CS_InsertEntry (S, X, IP++);

                X = NewCodeEntry (OP65_LDA, L[7]->AM, L[7]->Arg, 0, L[7]->LI);
                CS_InsertEntry (S, X, IP++);

                X = NewCodeEntry (OP65_LDY, L[1]->AM, L[1]->Arg, 0, L[1]->LI);
                CS_InsertEntry (S, X, IP++);

                X = NewCodeEntry (OP65_STA, AM, Loc, 0, L[9]->LI);
                CS_InsertEntry (S, X, IP++);

            } else if (AM == AM65_ABSY) {

                /* Case 2 */
                X = NewCodeEntry (OP65_LDA, L[1]->AM, L[1]->Arg, 0, L[1]->LI);
                CS_InsertEntry (S, X, IP++);

                X = NewCodeEntry (OP65_TAX, AM65_IMP, 0, 0, L[1]->LI);
                CS_InsertEntry (S, X, IP++);

                Arg = MakeHexArg (L[5]->Num - 2);
                X = NewCodeEntry (OP65_LDY, AM65_IMM, Arg, 0, L[5]->LI);
                CS_InsertEntry (S, X, IP++);

                X = NewCodeEntry (OP65_LDA, L[7]->AM, L[7]->Arg, 0, L[7]->LI);
                CS_InsertEntry (S, X, IP++);

                X = NewCodeEntry (OP65_STA, AM65_ABSX, Loc, 0, L[9]->LI);
                CS_InsertEntry (S, X, IP++);

                X = NewCodeEntry (OP65_LDX, L[6]->AM, L[6]->Arg, 0, L[6]->LI);
                CS_InsertEntry (S, X, IP++);

            } else {

                /* Case 3 */
                Arg = MakeHexArg (L[5]->Num - 2);
                X = NewCodeEntry (OP65_LDY, AM65_IMM, Arg, 0, L[5]->LI);
                CS_InsertEntry (S, X, IP++);

                X = NewCodeEntry (OP65_LDX, L[6]->AM, L[6]->Arg, 0, L[6]->LI);
                CS_InsertEntry (S, X, IP++);

                X = NewCodeEntry (OP65_LDA, L[7]->AM, L[7]->Arg, 0, L[7]->LI);
                CS_InsertEntry (S, X, IP++);

                X = NewCodeEntry (OP65_PHA, AM65_IMP, 0, 0, L[6]->LI);
                CS_InsertEntry (S, X, IP++);

                Arg = MakeHexArg (L[1]->RI->In.RegY);
                X = NewCodeEntry (OP65_LDY, AM65_IMM, Arg, 0, L[1]->LI);
                CS_InsertEntry (S, X, IP++);

                X = NewCodeEntry (OP65_LDA, L[1]->AM, L[1]->Arg, 0, L[1]->LI);
                CS_InsertEntry (S, X, IP++);

                X = NewCodeEntry (OP65_TAY, AM65_IMP, 0, 0, L[1]->LI);
                CS_InsertEntry (S, X, IP++);

                X = NewCodeEntry (OP65_PLA, AM65_IMP, 0, 0, L[6]->LI);
                CS_InsertEntry (S, X, IP++);

                X = NewCodeEntry (OP65_STA, AM, Loc, 0, L[9]->LI);
                CS_InsertEntry (S, X, IP++);

            }

            /* Remove the old code */
            CS_DelEntries (S, I, 10);

            /* Skip most of the generated replacement code */
            I += 4;

            /* Remember, we had changes */
            ++Changes;

        }

        /* Next entry */
        ++I;

    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptPtrStore3 (CodeSeg* S)
/* Search for the sequence:
**
**      jsr     pushax
**      ldy     xxx
**      jsr     ldauidx
**      subop
**      ldy     yyy
**      jsr     staspidx
**
** and replace it by:
**
**      sta     ptr1
**      stx     ptr1+1
**      ldy     xxx
**      ldx     #$00
**      lda     (ptr1),y
**      subop
**      ldy     yyy
**      sta     (ptr1),y
**
** In case a/x is loaded from the register bank before the pushax, we can even
** use the register bank instead of ptr1.
*/
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

        unsigned K;
        CodeEntry* L[10];

        /* Get next entry */
        L[0] = CS_GetEntry (S, I);

        /* Check for the sequence */
        if (CE_IsCallTo (L[0], "pushax")            &&
            CS_GetEntries (S, L+1, I+1, 3)          &&
            L[1]->OPC == OP65_LDY                   &&
            CE_IsConstImm (L[1])                    &&
            !CE_HasLabel (L[1])                     &&
            CE_IsCallTo (L[2], "ldauidx")           &&
            !CE_HasLabel (L[2])                     &&
            (K = OptPtrStore1Sub (S, I+3, L+3)) > 0 &&
            CS_GetEntries (S, L+3+K, I+3+K, 2)      &&
            L[3+K]->OPC == OP65_LDY                 &&
            CE_IsConstImm (L[3+K])                  &&
            !CE_HasLabel (L[3+K])                   &&
            CE_IsCallTo (L[4+K], "staspidx")        &&
            !CE_HasLabel (L[4+K])) {


            const char* RegBank = 0;
            const char* ZPLoc   = "ptr1";
            CodeEntry* X;


            /* Get the preceeding two instructions and check them. We check
            ** for:
            **          lda     regbank+n
            **          ldx     regbank+n+1
            */
            if (I > 1) {
                CodeEntry* P[2];
                P[0] = CS_GetEntry (S, I-2);
                P[1] = CS_GetEntry (S, I-1);
                if (P[0]->OPC == OP65_LDA &&
                    P[0]->AM  == AM65_ZP  &&
                    P[1]->OPC == OP65_LDX &&
                    P[1]->AM  == AM65_ZP  &&
                    !CE_HasLabel (P[1])   &&
                    strncmp (P[0]->Arg, "regbank+", 8) == 0) {

                    unsigned Len = strlen (P[0]->Arg);

                    if (strncmp (P[0]->Arg, P[1]->Arg, Len) == 0 &&
                        P[1]->Arg[Len+0] == '+'                  &&
                        P[1]->Arg[Len+1] == '1'                  &&
                        P[1]->Arg[Len+2] == '\0') {

                        /* Ok, found. Use the name of the register bank */
                        RegBank = ZPLoc = P[0]->Arg;
                    }
                }
            }

            /* Insert the load via the zp pointer */
            X = NewCodeEntry (OP65_LDX, AM65_IMM, "$00", 0, L[3]->LI);
            CS_InsertEntry (S, X, I+3);
            X = NewCodeEntry (OP65_LDA, AM65_ZP_INDY, ZPLoc, 0, L[2]->LI);
            CS_InsertEntry (S, X, I+4);

            /* Insert the store through the zp pointer */
            X = NewCodeEntry (OP65_STA, AM65_ZP_INDY, ZPLoc, 0, L[3]->LI);
            CS_InsertEntry (S, X, I+6+K);

            /* Delete the old code */
            CS_DelEntry (S, I+7+K);     /* jsr spaspidx */
            CS_DelEntry (S, I+2);       /* jsr ldauidx */

            /* Create and insert the stores into the zp pointer if needed */
            if (RegBank == 0) {
                X = NewCodeEntry (OP65_STA, AM65_ZP, "ptr1", 0, L[0]->LI);
                CS_InsertEntry (S, X, I+1);
                X = NewCodeEntry (OP65_STX, AM65_ZP, "ptr1+1", 0, L[0]->LI);
                CS_InsertEntry (S, X, I+2);
            }

            /* Delete more old code. Do it here to keep a label attached to
            ** entry I in place.
            */
            CS_DelEntry (S, I);         /* jsr pushax */

            /* Remember, we had changes */
            ++Changes;

        }

        /* Next entry */
        ++I;

    }

    /* Return the number of changes made */
    return Changes;
}
