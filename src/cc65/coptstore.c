/*****************************************************************************/
/*                                                                           */
/*                                  coptstore.c                              */
/*                                                                           */
/*                                Optimize stores                            */
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



/* cc65 */
#include "codeent.h"
#include "codeinfo.h"
#include "coptstore.h"



/*****************************************************************************/
/*				     Code                                    */
/*****************************************************************************/



static void InsertStore (CodeSeg* S, unsigned* IP, LineInfo* LI)
{
    CodeEntry* X = NewCodeEntry (OP65_STA, AM65_ZP_INDY, "sp", 0, LI);
    CS_InsertEntry (S, X, (*IP)++);
}



unsigned OptStore1 (CodeSeg* S)
/* Search for a call to staxysp. If the ax register is not used later, and
 * the value is constant, just use the A register and store directly into the
 * stack.
 */
{
    unsigned I;
    unsigned Changes = 0;

    /* Generate register info */
    CS_GenRegInfo (S);

    /* Walk over the entries */
    I = 0;
    while (I < CS_GetEntryCount (S)) {

      	/* Get next entry */
       	CodeEntry* E = CS_GetEntry (S, I);

        /* Get the input registers */
        const RegInfo* RI = E->RI;

     	/* Check for the call */
       	if (CE_IsCallTo (E, "staxysp")          &&
            RegValIsKnown (RI->In.RegA)         &&
            RegValIsKnown (RI->In.RegX)         &&
            RegValIsKnown (RI->In.RegY)         &&
            !RegAXUsed (S, I+1)) {

            /* Get the register values */
            unsigned char A = RI->In.RegA;
            unsigned char X = RI->In.RegX;
            unsigned char Y = RI->In.RegY;

            /* Setup other variables */
            CodeEntry*  N;
            unsigned    IP = I + 1;     /* Insertion point */

            /* Replace the store. We will not remove the loads, since this is
             * too complex and will be done by other optimizer steps.
             */
            N = NewCodeEntry (OP65_LDA, AM65_IMM, MakeHexArg (A), 0, E->LI);
            CS_InsertEntry (S, N, IP++);
            InsertStore (S, &IP, E->LI);

            /* Check if we can store one of the other bytes */
            if (A != X) {
                N = NewCodeEntry (OP65_LDA, AM65_IMM, MakeHexArg (X), 0, E->LI);
                CS_InsertEntry (S, N, IP++);
            }
            N = NewCodeEntry (OP65_LDY, AM65_IMM, MakeHexArg (Y+1), 0, E->LI);
            CS_InsertEntry (S, N, IP++);
            InsertStore (S, &IP, E->LI);

            /* Remove the call */
            CS_DelEntry (S, I);

	    /* Remember, we had changes */
	    ++Changes;

	}

	/* Next entry */
	++I;

    }

    /* Free the register info */
    CS_FreeRegInfo (S);

    /* Return the number of changes made */
    return Changes;
}



unsigned OptStore2 (CodeSeg* S)
/* Search for a call to steaxysp. If the eax register is not used later, and
 * the value is constant, just use the A register and store directly into the
 * stack.
 */
{
    unsigned I;
    unsigned Changes = 0;

    /* Generate register info */
    CS_GenRegInfo (S);

    /* Walk over the entries */
    I = 0;
    while (I < CS_GetEntryCount (S)) {

      	/* Get next entry */
       	CodeEntry* E = CS_GetEntry (S, I);

        /* Get the input registers */
        const RegInfo* RI = E->RI;

     	/* Check for the call */
	if (CE_IsCallTo (E, "steaxysp")         &&
            RegValIsKnown (RI->In.RegA)         &&
            RegValIsKnown (RI->In.RegX)         &&
            RegValIsKnown (RI->In.RegY)         &&
            RegValIsKnown (RI->In.SRegLo)       &&
            RegValIsKnown (RI->In.SRegHi)       &&
            !RegEAXUsed (S, I+1)) {

            /* Get the register values */
            unsigned char A = RI->In.RegA;
            unsigned char X = RI->In.RegX;
            unsigned char Y = RI->In.RegY;
            unsigned char L = RI->In.SRegLo;
            unsigned char H = RI->In.SRegHi;

            /* Setup other variables */
            unsigned    Done = 0;
            CodeEntry*  N;
            unsigned    IP = I + 1;     /* Insertion point */

            /* Replace the store. We will not remove the loads, since this is
             * too complex and will be done by other optimizer steps.
             */
            N = NewCodeEntry (OP65_LDA, AM65_IMM, MakeHexArg (A), 0, E->LI);
            CS_InsertEntry (S, N, IP++);
            InsertStore (S, &IP, E->LI);
            Done |= 0x01;

            /* Check if we can store one of the other bytes */
            if (A == X && (Done & 0x02) == 0) {
                N = NewCodeEntry (OP65_LDY, AM65_IMM, MakeHexArg (Y+1), 0, E->LI);
                CS_InsertEntry (S, N, IP++);
                InsertStore (S, &IP, E->LI);
                Done |= 0x02;
            }
            if (A == L && (Done & 0x04) == 0) {
                N = NewCodeEntry (OP65_LDY, AM65_IMM, MakeHexArg (Y+2), 0, E->LI);
                CS_InsertEntry (S, N, IP++);
                InsertStore (S, &IP, E->LI);
                Done |= 0x04;
            }
            if (A == H && (Done & 0x08) == 0) {
                N = NewCodeEntry (OP65_LDY, AM65_IMM, MakeHexArg (Y+3), 0, E->LI);
                CS_InsertEntry (S, N, IP++);
                InsertStore (S, &IP, E->LI);
                Done |= 0x08;
            }

            /* Store the second byte */
            if ((Done & 0x02) == 0) {
                N = NewCodeEntry (OP65_LDA, AM65_IMM, MakeHexArg (X), 0, E->LI);
                CS_InsertEntry (S, N, IP++);
                N = NewCodeEntry (OP65_LDY, AM65_IMM, MakeHexArg (Y+1), 0, E->LI);
                CS_InsertEntry (S, N, IP++);
                InsertStore (S, &IP, E->LI);
                Done |= 0x02;
            }

            /* Check if we can store one of the other bytes */
            if (X == L && (Done & 0x04) == 0) {
                N = NewCodeEntry (OP65_LDY, AM65_IMM, MakeHexArg (Y+2), 0, E->LI);
                CS_InsertEntry (S, N, IP++);
                InsertStore (S, &IP, E->LI);
                Done |= 0x04;
            }
            if (X == H && (Done & 0x08) == 0) {
                N = NewCodeEntry (OP65_LDY, AM65_IMM, MakeHexArg (Y+3), 0, E->LI);
                CS_InsertEntry (S, N, IP++);
                InsertStore (S, &IP, E->LI);
                Done |= 0x08;
            }

            /* Store the third byte */
            if ((Done & 0x04) == 0) {
                N = NewCodeEntry (OP65_LDA, AM65_IMM, MakeHexArg (L), 0, E->LI);
                CS_InsertEntry (S, N, IP++);
                N = NewCodeEntry (OP65_LDY, AM65_IMM, MakeHexArg (Y+2), 0, E->LI);
                CS_InsertEntry (S, N, IP++);
                InsertStore (S, &IP, E->LI);
                Done |= 0x04;
            }

            /* Check if we can store one of the other bytes */
            if (L == H && (Done & 0x08) == 0) {
                N = NewCodeEntry (OP65_LDY, AM65_IMM, MakeHexArg (Y+3), 0, E->LI);
                CS_InsertEntry (S, N, IP++);
                InsertStore (S, &IP, E->LI);
                Done |= 0x08;
            }

            /* Store the fourth byte */
            if ((Done & 0x08) == 0) {
                N = NewCodeEntry (OP65_LDA, AM65_IMM, MakeHexArg (H), 0, E->LI);
                CS_InsertEntry (S, N, IP++);
                N = NewCodeEntry (OP65_LDY, AM65_IMM, MakeHexArg (Y+3), 0, E->LI);
                CS_InsertEntry (S, N, IP++);
                InsertStore (S, &IP, E->LI);
                Done |= 0x08;
            }

            /* Remove the call */
            CS_DelEntry (S, I);

	    /* Remember, we had changes */
	    ++Changes;

	}

	/* Next entry */
	++I;

    }

    /* Free the register info */
    CS_FreeRegInfo (S);

    /* Return the number of changes made */
    return Changes;
}



