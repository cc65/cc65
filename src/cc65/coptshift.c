/*****************************************************************************/
/*                                                                           */
/*                                coptshift.c                                */
/*                                                                           */
/*                              Optimize shifts                              */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001-2012, Ullrich von Bassewitz                                      */
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



/* common */
#include "chartype.h"

/* cc65 */           
#include "codeent.h"
#include "codeinfo.h"
#include "coptshift.h"



/*****************************************************************************/
/*                                     Data                                  */
/*****************************************************************************/



/* Shift types */
enum {
    SHIFT_NONE,
    SHIFT_ASR_1,
    SHIFT_ASL_1,
    SHIFT_LSR_1,
    SHIFT_LSL_1
};



/*****************************************************************************/
/*				Optimize shifts                              */
/*****************************************************************************/



unsigned OptShift1 (CodeSeg* S)
/* A call to the shlaxN routine may get replaced by one or more asl insns
 * if the value of X is not used later. If X is used later, but it is zero
 * on entry and it's a shift by one, it may get replaced by:
 *
 *      asl     a
 *      bcc     L1
 *      inx
 *  L1:
 *
 */
{
    unsigned Changes = 0;
    unsigned I;

    /* Walk over the entries */
    I = 0;
    while (I < CS_GetEntryCount (S)) {

        CodeEntry* N;
        CodeEntry* X;
        CodeLabel* L;

      	/* Get next entry */
       	CodeEntry* E = CS_GetEntry (S, I);

     	/* Check for the sequence */
	if (E->OPC == OP65_JSR                       &&
       	    (strncmp (E->Arg, "shlax", 5) == 0 ||
	     strncmp (E->Arg, "aslax", 5) == 0)	     &&
	    strlen (E->Arg) == 6                     &&
	    IsDigit (E->Arg[5])) {

            if (!RegXUsed (S, I+1)) {

                /* Insert shift insns */
                unsigned Count = E->Arg[5] - '0';
                while (Count--) {
                    X = NewCodeEntry (OP65_ASL, AM65_ACC, "a", 0, E->LI);
                    CS_InsertEntry (S, X, I+1);
                }

                /* Delete the call to shlax */
                CS_DelEntry (S, I);

                /* Remember, we had changes */
                ++Changes;

            } else if (E->RI->In.RegX == 0              &&
                       E->Arg[5] == '1'                 &&
                       (N = CS_GetNextEntry (S, I)) != 0) {

                /* asl a */
                X = NewCodeEntry (OP65_ASL, AM65_ACC, "a", 0, E->LI);
                CS_InsertEntry (S, X, I+1);

                /* bcc L1 */
                L = CS_GenLabel (S, N);
                X = NewCodeEntry (OP65_BCC, AM65_BRA, L->Name, L, E->LI);
                CS_InsertEntry (S, X, I+2);

                /* inx */
                X = NewCodeEntry (OP65_INX, AM65_IMP, 0, 0, E->LI);
                CS_InsertEntry (S, X, I+3);

                /* Delete the call to shlax */
                CS_DelEntry (S, I);

                /* Remember, we had changes */
                ++Changes;
            }

	}

	/* Next entry */
	++I;

    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptShift2(CodeSeg* S)
/* A call to the asrax1 routines may get replaced by something simpler, if
 * X is not used later:
 *
 *      cmp     #$80
 *      ror     a
 *
 */
{
    unsigned Changes = 0;
    unsigned I;

    /* Walk over the entries */
    I = 0;
    while (I < CS_GetEntryCount (S)) {

        unsigned Count;

      	/* Get next entry */
       	CodeEntry* E = CS_GetEntry (S, I);

     	/* Check for the sequence */
	if (E->OPC == OP65_JSR                  &&
       	    strncmp (E->Arg, "asrax", 5) == 0   &&
	    strlen (E->Arg) == 6                &&
	    IsDigit (E->Arg[5])                 &&
            (Count = (E->Arg[5] - '0')) >= 1    &&
            Count * 100 <= S->CodeSizeFactor    &&
            !RegXUsed (S, I+1)) {

            CodeEntry* X;
            unsigned J = I+1;

            /* Generate the replacement sequence */
            while (Count--) {
                /* cmp #$80 */
                X = NewCodeEntry (OP65_CMP, AM65_IMM, "$80", 0, E->LI);
                CS_InsertEntry (S, X, J++);

                /* ror a */
                X = NewCodeEntry (OP65_ROR, AM65_ACC, "a", 0, E->LI);
                CS_InsertEntry (S, X, J++);
            }

            /* Delete the call to asrax */
            CS_DelEntry (S, I);

            /* Remember, we had changes */
            ++Changes;
    	}

    	/* Next entry */
    	++I;

    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptShift3 (CodeSeg* S)
/* The sequence
 *
 *      bcc     L
 *  	inx
 * L:   jsr     shrax1
 *
 * may get replaced by
 *
 *      ror     a
 *
 * if X is zero on entry and unused later. For shift counts > 1, more
 *
 *      shr     a
 *
 * must be added.
 */
{
    unsigned Changes = 0;
    unsigned I;

    /* Walk over the entries */
    I = 0;
    while (I < CS_GetEntryCount (S)) {

	CodeEntry* L[3];

      	/* Get next entry */
       	L[0] = CS_GetEntry (S, I);

     	/* Check for the sequence */
       	if ((L[0]->OPC == OP65_BCC || L[0]->OPC == OP65_JCC)    &&
	    L[0]->JumpTo != 0                                   &&
            L[0]->RI->In.RegX == 0                              &&
       	    CS_GetEntries (S, L+1, I+1, 2)                      &&
	    L[1]->OPC == OP65_INX            	       	        &&
	    L[0]->JumpTo->Owner == L[2]                         &&
	    !CS_RangeHasLabel (S, I, 2)                         &&
            L[2]->OPC == OP65_JSR                               &&
            strlen (L[2]->Arg) == 6                             &&
            memcmp (L[2]->Arg, "shrax", 5) == 0                 &&
            IsDigit (L[2]->Arg[5])                              &&
	    !RegXUsed (S, I+3)) {

            unsigned ShiftCount = (L[2]->Arg[5] - '0');

            /* Add the replacement insn instead */
            CodeEntry* X = NewCodeEntry (OP65_ROR, AM65_ACC, "a", 0, L[2]->LI);
            CS_InsertEntry (S, X, I+3);
            while (--ShiftCount) {
                X = NewCodeEntry (OP65_LSR, AM65_ACC, "a", 0, L[2]->LI);
                CS_InsertEntry (S, X, I+4);
            }

	    /* Remove the bcs/dex/jsr */
	    CS_DelEntries (S, I, 3);

	    /* Remember, we had changes */
	    ++Changes;

	}

	/* Next entry */
	++I;

    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptShift4 (CodeSeg* S)
/* Calls to the asraxN or shraxN routines may get replaced by one or more lsr
 * insns if the value of X is zero.
 */
{
    unsigned Changes = 0;
    unsigned I;

    /* Walk over the entries */
    I = 0;
    while (I < CS_GetEntryCount (S)) {

      	/* Get next entry */
       	CodeEntry* E = CS_GetEntry (S, I);

     	/* Check for the sequence */
	if (E->OPC == OP65_JSR                          &&
       	    (strncmp (E->Arg, "shrax", 5) == 0  ||
             strncmp (E->Arg, "asrax", 5) == 0)         &&
	    strlen (E->Arg) == 6                        &&
	    IsDigit (E->Arg[5])                         &&
       	    E->RI->In.RegX == 0) {

	    /* Insert shift insns */
	    unsigned Count = E->Arg[5] - '0';
	    while (Count--) {
	    	CodeEntry* X = NewCodeEntry (OP65_LSR, AM65_ACC, "a", 0, E->LI);
    	    	CS_InsertEntry (S, X, I+1);
	    }

	    /* Delete the call to shrax */
	    CS_DelEntry (S, I);

	    /* Remember, we had changes */
	    ++Changes;

	}

	/* Next entry */
	++I;

    }

    /* Return the number of changes made */
    return Changes;
}



static unsigned GetShiftType (const char* Sub)
/* Helper function for OptShift5 */
{
    if (*Sub == 'a') {
        if (strcmp (Sub+1, "slax1") == 0) {
            return SHIFT_ASL_1;
        } else if (strcmp (Sub+1, "srax1") == 0) {
            return SHIFT_ASR_1;
        }
    } else if (*Sub == 's') {
        if (strcmp (Sub+1, "hlax1") == 0) {
            return SHIFT_LSL_1;
        } else if (strcmp (Sub+1, "hrax1") == 0) {
            return SHIFT_LSR_1;
        }
    }
    return SHIFT_NONE;
}



unsigned OptShift5 (CodeSeg* S)
/* Search for the sequence
 *
 *      lda     xxx
 *      ldx     yyy
 *      jsr     aslax1/asrax1/shlax1/shrax1
 *      sta     aaa
 *      stx     bbb
 *
 * and replace it by
 *
 *      lda     xxx
 *      asl     a
 *      sta     aaa
 *      lda     yyy
 *      rol     a
 *      sta     bbb
 *
 * or similar, provided that a/x is not used later
 */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

        unsigned ShiftType;
     	CodeEntry* L[5];

      	/* Get next entry */
       	L[0] = CS_GetEntry (S, I);

     	/* Check for the sequence */
       	if (L[0]->OPC == OP65_LDA                               &&
            (L[0]->AM == AM65_ABS || L[0]->AM == AM65_ZP)       &&
       	    CS_GetEntries (S, L+1, I+1, 4)     	                &&
            !CS_RangeHasLabel (S, I+1, 4)                       &&
            L[1]->OPC == OP65_LDX                               &&
            (L[1]->AM == AM65_ABS || L[1]->AM == AM65_ZP)       &&
            L[2]->OPC == OP65_JSR                               &&
            (ShiftType = GetShiftType (L[2]->Arg)) != SHIFT_NONE&&
       	    L[3]->OPC == OP65_STA                               &&
            (L[3]->AM == AM65_ABS || L[3]->AM == AM65_ZP)       &&
            L[4]->OPC == OP65_STX                               &&
            (L[4]->AM == AM65_ABS || L[4]->AM == AM65_ZP)       &&
            !RegAXUsed (S, I+5)) {

            CodeEntry* X;

            /* Handle the four shift types differently */
            switch (ShiftType) {

                case SHIFT_ASR_1:
                    X = NewCodeEntry (OP65_LDA, L[1]->AM, L[1]->Arg, 0, L[1]->LI);
                    CS_InsertEntry (S, X, I+5);
                    X = NewCodeEntry (OP65_CMP, AM65_IMM, "$80", 0, L[2]->LI);
                    CS_InsertEntry (S, X, I+6);
                    X = NewCodeEntry (OP65_ROR, AM65_ACC, "a", 0, L[2]->LI);
                    CS_InsertEntry (S, X, I+7);
                    X = NewCodeEntry (OP65_STA, L[4]->AM, L[4]->Arg, 0, L[4]->LI);
                    CS_InsertEntry (S, X, I+8);
                    X = NewCodeEntry (OP65_LDA, L[0]->AM, L[0]->Arg, 0, L[0]->LI);
                    CS_InsertEntry (S, X, I+9);
                    X = NewCodeEntry (OP65_ROR, AM65_ACC, "a", 0, L[2]->LI);
                    CS_InsertEntry (S, X, I+10);
                    X = NewCodeEntry (OP65_STA, L[3]->AM, L[3]->Arg, 0, L[3]->LI);
                    CS_InsertEntry (S, X, I+11);
                    CS_DelEntries (S, I, 5);
                    break;

                case SHIFT_LSR_1:
                    X = NewCodeEntry (OP65_LDA, L[1]->AM, L[1]->Arg, 0, L[1]->LI);
                    CS_InsertEntry (S, X, I+5);
                    X = NewCodeEntry (OP65_LSR, AM65_ACC, "a", 0, L[2]->LI);
                    CS_InsertEntry (S, X, I+6);
                    X = NewCodeEntry (OP65_STA, L[4]->AM, L[4]->Arg, 0, L[4]->LI);
                    CS_InsertEntry (S, X, I+7);
                    X = NewCodeEntry (OP65_LDA, L[0]->AM, L[0]->Arg, 0, L[0]->LI);
                    CS_InsertEntry (S, X, I+8);
                    X = NewCodeEntry (OP65_ROR, AM65_ACC, "a", 0, L[2]->LI);
                    CS_InsertEntry (S, X, I+9);
                    X = NewCodeEntry (OP65_STA, L[3]->AM, L[3]->Arg, 0, L[3]->LI);
                    CS_InsertEntry (S, X, I+10);
                    CS_DelEntries (S, I, 5);
                    break;

                case SHIFT_LSL_1:
                case SHIFT_ASL_1:
                    /* These two are identical */
                    X = NewCodeEntry (OP65_ASL, AM65_ACC, "a", 0, L[2]->LI);
                    CS_InsertEntry (S, X, I+1);
                    X = NewCodeEntry (OP65_STA, L[3]->AM, L[3]->Arg, 0, L[3]->LI);
                    CS_InsertEntry (S, X, I+2);
                    X = NewCodeEntry (OP65_LDA, L[1]->AM, L[1]->Arg, 0, L[1]->LI);
                    CS_InsertEntry (S, X, I+3);
                    X = NewCodeEntry (OP65_ROL, AM65_ACC, "a", 0, L[2]->LI);
                    CS_InsertEntry (S, X, I+4);
                    X = NewCodeEntry (OP65_STA, L[4]->AM, L[4]->Arg, 0, L[4]->LI);
                    CS_InsertEntry (S, X, I+5);
                    CS_DelEntries (S, I+6, 4);
                    break;

            }

	    /* Remember, we had changes */
            ++Changes;

	}

	/* Next entry */
	++I;

    }

    /* Return the number of changes made */
    return Changes;
}



unsigned OptShift6 (CodeSeg* S)
/* Inline the shift subroutines. */
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

        CodeEntry* X;
        unsigned   IP;

      	/* Get next entry */
     	CodeEntry* E = CS_GetEntry (S, I);

     	/* Check for a call to one of the shift routine */
	if (E->OPC == OP65_JSR                          &&
       	    (strncmp (E->Arg, "shlax", 5) == 0  ||
             strncmp (E->Arg, "aslax", 5) == 0)         &&
	    strlen (E->Arg) == 6                        &&
	    IsDigit (E->Arg[5])) {

            /* Get number of shifts */
            unsigned ShiftCount = (E->Arg[5] - '0');

            /* Code is:
             *
             *      stx     tmp1
             *      asl     a
             *      rol     tmp1
             *      (repeat ShiftCount-1 times)
             *      ldx     tmp1
             *
             * which makes 4 + 3 * ShiftCount bytes, compared to the original
             * 3 bytes for the subroutine call. However, in most cases, the
             * final load of the X register gets merged with some other insn
             * and replaces a txa, so for a shift count of 1, we get a factor
             * of 200, which matches nicely the CodeSizeFactor enabled with -Oi
             */
            if (ShiftCount > 1 || S->CodeSizeFactor > 200) {
                unsigned Size = 4 + 3 * ShiftCount;
                if ((Size * 100 / 3) > S->CodeSizeFactor) {
                    /* Not acceptable */
                    goto NextEntry;
                }
            }

            /* Inline the code. Insertion point is behind the subroutine call */
            IP = (I + 1);

            /* stx tmp1 */
            X = NewCodeEntry (OP65_STX, AM65_ZP, "tmp1", 0, E->LI);
            CS_InsertEntry (S, X, IP++);

            while (ShiftCount--) {
                /* asl a */
                X = NewCodeEntry (OP65_ASL, AM65_ACC, "a", 0, E->LI);
                CS_InsertEntry (S, X, IP++);

                /* rol tmp1 */
                X = NewCodeEntry (OP65_ROL, AM65_ZP, "tmp1", 0, E->LI);
                CS_InsertEntry (S, X, IP++);
            }

            /* ldx tmp1 */
            X = NewCodeEntry (OP65_LDX, AM65_ZP, "tmp1", 0, E->LI);
            CS_InsertEntry (S, X, IP++);

            /* Remove the subroutine call */
            CS_DelEntry (S, I);

	    /* Remember, we had changes */
            ++Changes;
	}

NextEntry:
	/* Next entry */
	++I;

    }

    /* Return the number of changes made */
    return Changes;
}



