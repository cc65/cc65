/*****************************************************************************/
/*                                                                           */
/*                                 coptlong.c                                */
/*                                                                           */
/*                       Long integers optimizations                         */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001-2009, Ullrich von Bassewitz                                      */
/*                Roemerstrasse 52                                           */
/*                D-70794 Filderstadt                                        */
/* EMail:         uz@cc65.org                                                */
/* (C) 2023, Colin Leroy-Mira <colin@colino.net                              */
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
#include "cpu.h"

/* cc65 */
#include "codeent.h"
#include "coptind.h"
#include "codeinfo.h"
#include "codeopt.h"
#include "error.h"


/*****************************************************************************/
/*                      Remove unused loads and stores                       */
/*****************************************************************************/

unsigned OptLongAssign (CodeSeg* S)
/* Simplify long assignments.
** Recognize
**      lda     #IMM      0
**      sta     sreg+1    1
**      lda     #IMM      2
**      sta     sreg      3
**      lda     #IMM      4
**      ldx     #IMM      5
**      sta     YYY       6
**      stx     YYY+1     7
**      ldy     sreg      8
**      sty     YYY+2     9
**      ldy     sreg+1    10
**      sty     YYY+3     11
** and simplify, if not used right after and no branching occurs, to
**      lda    XXX+3
**      sta    YYY+3
**      lda    XXX+2
**      sta    YYY+2
**      ldx    XXX
**      lda    XXX+1
**      sta    YYY
**      stx    YYY+1
*/
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

        CodeEntry* L[13];

        /* Get next entry */
        L[0] = CS_GetEntry (S, I);

        if (CS_GetEntries (S, L+1, I+1, 12)) {
          CodeEntry* N;
          if (/* Check the opcode sequence */
              L[0]->OPC == OP65_LDA                                &&
              L[1]->OPC == OP65_STA                                &&
              L[2]->OPC == OP65_LDA                                &&
              L[3]->OPC == OP65_STA                                &&
              L[4]->OPC == OP65_LDA                                &&
              L[5]->OPC == OP65_LDX                                &&
              L[6]->OPC == OP65_STA                                &&
              L[7]->OPC == OP65_STX                                &&
              L[8]->OPC == OP65_LDY                                &&
              L[9]->OPC == OP65_STY                                &&
              L[10]->OPC == OP65_LDY                               &&
              L[11]->OPC == OP65_STY                               &&
              /* Check the arguments match */
              L[0]->AM == AM65_IMM                                 &&
              !strcmp (L[1]->Arg, "sreg+1")                        &&
              L[2]->AM == AM65_IMM                                 &&
              !strcmp (L[3]->Arg, "sreg")                          &&
              L[4]->AM == AM65_IMM                                 &&
              L[5]->AM == AM65_IMM                                 &&
              !strncmp(L[7]->Arg, L[6]->Arg, strlen(L[6]->Arg))    &&
                !strcmp(L[7]->Arg + strlen(L[6]->Arg), "+1")       &&
              !strcmp (L[8]->Arg, "sreg")                          &&
              !strncmp(L[9]->Arg, L[6]->Arg, strlen(L[6]->Arg))    &&
                !strcmp(L[9]->Arg + strlen(L[6]->Arg), "+2")       &&
              !strcmp (L[10]->Arg, "sreg+1")                       &&
              !strncmp(L[11]->Arg, L[6]->Arg, strlen(L[6]->Arg))   &&
                !strcmp(L[11]->Arg + strlen(L[6]->Arg), "+3")      &&
              /* Check there's nothing more */
              !RegXUsed (S, I+12)                                  &&
              !CS_RangeHasLabel(S, I, 12)) {

              N = NewCodeEntry (OP65_STA, L[11]->AM, L[11]->Arg, 0, L[11]->LI);
              CS_DelEntry (S, I+1);
              CS_InsertEntry (S, N, I+1);

              N = NewCodeEntry (OP65_STA, L[9]->AM, L[9]->Arg, 0, L[9]->LI);
              CS_DelEntry (S, I+3);
              CS_InsertEntry (S, N, I+3);

              CS_DelEntries (S, I+8, 4);

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

unsigned OptLongCopy (CodeSeg* S)
/* Simplify long copies.
** Recognize
**      lda     XXX+3       0
**      sta     sreg+1      1
**      lda     XXX+2       2
**      sta     sreg        3
**      ldx     XXX+1       4
**      lda     XXX         5
**      sta     YYY         6
**      stx     YYY+1       7
**      ldy     sreg        8
**      sty     YYY+2       9
**      ldy     sreg+1      10
**      sty     YYY+3       11
** and simplify, if not used right after and no branching occurs, to
**      lda    XXX+3
**      sta    YYY+3
**      lda    XXX+2
**      sta    YYY+2
**      ldx    XXX
**      lda    XXX+1
**      sta    YYY
**      stx    YYY+1
*/
{
    unsigned Changes = 0;

    /* Walk over the entries */
    unsigned I = 0;
    while (I < CS_GetEntryCount (S)) {

        CodeEntry* L[13];

        /* Get next entry */
        L[0] = CS_GetEntry (S, I);

        if (CS_GetEntries (S, L+1, I+1, 12)) {
          CodeEntry *N;
          if (L[0]->OPC == OP65_LDA                                &&
                !strncmp(L[0]->Arg, L[5]->Arg, strlen(L[5]->Arg))  &&
                !strcmp(L[0]->Arg + strlen(L[5]->Arg), "+3")       &&
              L[1]->OPC == OP65_STA                                &&
                !strcmp (L[1]->Arg, "sreg+1")                      &&
              L[2]->OPC == OP65_LDA                                &&
                !strncmp(L[2]->Arg, L[5]->Arg, strlen(L[5]->Arg))  &&
                !strcmp(L[2]->Arg + strlen(L[5]->Arg), "+2")       &&
              L[3]->OPC == OP65_STA                                &&
                !strcmp (L[3]->Arg, "sreg")                        &&
              L[4]->OPC == OP65_LDX                                &&
                !strncmp(L[4]->Arg, L[5]->Arg, strlen(L[5]->Arg))  &&
                !strcmp(L[4]->Arg + strlen(L[5]->Arg), "+1")       &&
              L[5]->OPC == OP65_LDA                                &&
              L[6]->OPC == OP65_STA                                &&
              L[7]->OPC == OP65_STX                                &&
                !strncmp(L[7]->Arg, L[6]->Arg, strlen(L[6]->Arg))  &&
                !strcmp(L[7]->Arg + strlen(L[6]->Arg), "+1")       &&
              L[8]->OPC == OP65_LDY                                &&
                !strcmp (L[8]->Arg, "sreg")                        &&
              L[9]->OPC == OP65_STY                                &&
                !strncmp(L[9]->Arg, L[6]->Arg, strlen(L[6]->Arg))  &&
                !strcmp(L[9]->Arg + strlen(L[6]->Arg), "+2")       &&
              L[10]->OPC == OP65_LDY                               &&
                !strcmp (L[10]->Arg, "sreg+1")                     &&
              L[11]->OPC == OP65_STY                               &&
                !strncmp(L[11]->Arg, L[6]->Arg, strlen(L[6]->Arg)) &&
                !strcmp(L[11]->Arg + strlen(L[6]->Arg), "+3")      &&
              !RegXUsed (S, I+11)                                  &&
              !CS_RangeHasLabel(S, I, 12)) {

              N = NewCodeEntry (OP65_STA, L[11]->AM, L[11]->Arg, 0, L[11]->LI);
              CS_DelEntry (S, I+1);
              CS_InsertEntry (S, N, I+1);

              N = NewCodeEntry (OP65_STA, L[9]->AM, L[9]->Arg, 0, L[9]->LI);
              CS_DelEntry (S, I+3);
              CS_InsertEntry (S, N, I+3);

              CS_DelEntries (S, I+8, 4);

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
