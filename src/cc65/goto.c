/*****************************************************************************/
/*                                                                           */
/*                                  goto.c                                   */
/*                                                                           */
/*              Goto and label handling for the cc65 C compiler              */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000     Ullrich von Bassewitz                                        */
/*              Wacholderweg 14                                              */
/*              D-70597 Stuttgart                                            */
/* EMail:       uz@musoftware.de                                             */
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



#include "asmlabel.h"
#include "codeent.h"
#include "codegen.h"
#include "codeseg.h"
#include "cpu.h"
#include "error.h"
#include "exprdesc.h"
#include "expr.h"
#include "loadexpr.h"
#include "scanner.h"
#include "seqpoint.h"
#include "standard.h"
#include "symtab.h"
#include "goto.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void GotoStatement (void)
/* Process a goto statement. */
{
    /* Eat the "goto" */
    NextToken ();

    /* Label name must follow */
    if (CurTok.Tok == TOK_IDENT) {

        /* Add a new label symbol if we don't have one until now */
        SymEntry* Entry = AddLabelSym (CurTok.Ident, SC_REF | SC_GOTO);

        /* Jump to the label */
        g_jump (Entry->V.L.Label);

        /* Eat the label name */
        NextToken ();

    } else if (CurTok.Tok == TOK_STAR && IS_Get (&Standard) >= STD_CC65) {
        SymEntry *arr, *idx, *cur;
        SymTable *tab;
        ExprDesc desc;
        CodeEntry *E;
        unsigned char val;

        ED_Init (&desc);

        NextToken ();

        /* arr[foo], we only support simple foo for now */
        if (CurTok.Tok == TOK_IDENT &&
            (arr = FindSym (CurTok.Ident))) {
            NextToken ();

            /* Find array size */
            if (!IsTypeArray (arr->Type) || SizeOf (arr->Type) == 0 ||
                (arr->Flags & SC_STORAGEMASK) != SC_STATIC ||
                SizeOf (GetElementType(arr->Type)) != 2) {
                Error ("Expected a static array");
            } else if (GetElementCount (arr->Type) > 127) {
                Error ("Only arrays with <= 127 labels are supported, got %lu",
                       GetElementCount (arr->Type));
            }

            ConsumeLBrack ();

            if (CurTok.Tok == TOK_ICONST) {
                val = (unsigned char)CurTok.IVal;
                NextToken ();

                /* Append deferred inc/dec at sequence point */
                DoDeferred (SQP_KEEP_NONE, &desc);

                if (CPUIsets[CPU] & CPU_ISET_65SC02) {
                    AddCodeLine ("ldx #$%02X", val * 2);
                    AddCodeLine ("jmp (.loword(%s),x)", arr->AsmName);
                } else {
                    AddCodeLine ("ldy #$%02X", val * 2);
                    AddCodeLine ("lda %s,y", arr->AsmName);
                    AddCodeLine ("ldx %s+1,y", arr->AsmName);
                    AddCodeLine ("jmp callax");
                }
            } else if (CurTok.Tok == TOK_IDENT &&
                       (idx = FindSym (CurTok.Ident))) {
                hie10 (&desc);
                LoadExpr (CF_NONE, &desc);

                /* Append deferred inc/dec at sequence point */
                DoDeferred (SQP_KEEP_EAX, &desc);

                AddCodeLine ("asl a");

                if (CPUIsets[CPU] & CPU_ISET_65SC02) {
                    AddCodeLine ("tax");
                    AddCodeLine ("jmp (.loword(%s),x)", arr->AsmName);
                } else {
                    AddCodeLine ("tay");
                    AddCodeLine ("lda %s,y", arr->AsmName);
                    AddCodeLine ("ldx %s+1,y", arr->AsmName);
                    AddCodeLine ("jmp callax");
                }
            } else {
                Error ("Only simple expressions are supported for computed goto");
            }

            ConsumeRBrack ();

            /* Loop over all target labels, specifying this as a jump point.
            ** It's not exact -- if there's multiple gotos, the last will be used;
            ** but, it's needed only so the optimizer does not remove the labels.
            */
            E = CS_GetEntry (CS->Code, CS_GetEntryCount (CS->Code) - 1);
            tab = GetLabelSymTab ();
            if (tab) {
                cur = tab->SymHead;
                while (cur) {
                    if ((cur->Flags & SC_GOTO_IND) != 0) {
                        cur->V.L.IndJumpFrom = E;
                    }
                    cur = cur->NextSym;
                }
            }
        } else {
            /* It was not TOK_IDENT, or we couldn't find the symbol */
            Error ("Array name expected");
        }
    } else {
        Error ("Label name expected");
    }
}



void DoLabel (void)
/* Define a label. */
{
    /* Add a label symbol */
    SymEntry* Entry = AddLabelSym (CurTok.Ident, SC_DEF);

    /* Emit the jump label */
    CodeLabel* L = CS_AddLabel (CS->Code, LocalLabelName (Entry->V.L.Label));

    if (Entry->V.L.IndJumpFrom) {
        CollAppend (&L->JumpFrom, Entry->V.L.IndJumpFrom);
    }

    /* Eat the ident and colon */
    NextToken ();
    NextToken ();
}
