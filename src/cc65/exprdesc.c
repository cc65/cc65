/*****************************************************************************/
/*                                                                           */
/*                                exprdesc.c                                 */
/*                                                                           */
/*                      Expression descriptor structure                      */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2002-2004 Ullrich von Bassewitz                                       */
/*               Römerstraße 52                                              */
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



/* cc65 */
#include "datatype.h"
#include "symentry.h"
#include "exprdesc.h"



/*****************************************************************************/
/*				     Code                                    */
/*****************************************************************************/



ExprDesc* ED_MakeConstInt (ExprDesc* Expr, long Value)
/* Make Expr a constant integer expression with the given value */
{
    Expr->Flags = E_MCONST | E_RVAL;
    Expr->Type = type_int;
    Expr->ConstVal = Value;
    return Expr;
}



void PrintExprDesc (FILE* F, ExprDesc* E)
/* Print an ExprDesc */
{
    fprintf (F, "Symbol:   %s\n", E->Sym? E->Sym->Name : "(none)");
    if (E->Type) {
        fprintf (F, "Type:     ");
        PrintType (F, E->Type);
        fprintf (F, "\nRaw type: ");
        PrintRawType (F, E->Type);
    } else {
        fprintf (F, "Type:     (unknown)\n"
                    "Raw type: (unknown)\n");
    }
    fprintf (F, "Value:    0x%08lX\n", E->ConstVal);
    fprintf (F, "Flags:    ");
    switch (E->Flags & E_MCTYPE) {
        case E_TCONST:    fprintf (F, "E_TCONST ");                    break;
        case E_TGLAB:     fprintf (F, "E_TGLAB ");                     break;
        case E_TLIT:      fprintf (F, "E_TLIT ");                      break;
        case E_TLOFFS:    fprintf (F, "E_TLOFFS ");                    break;
        case E_TLLAB:     fprintf (F, "E_TLLAB ");                     break;
        case E_TREGISTER: fprintf (F, "E_TREGISTER ");                 break;
        default:          fprintf (F, "0x%02X ", E->Flags & E_MCTYPE); break;
    }
    if ((E->Flags & E_MREG) == E_MREG) {
        fprintf (F, "E_MREG ");
    } else if ((E->Flags & E_MEOFFS) == E_MEOFFS) {
        fprintf (F, "E_MEOFFS ");
    } else if ((E->Flags & E_MEXPR) == E_MEXPR) {
        fprintf (F, "E_MEXPR ");
    }
    if ((E->Flags & E_MGLOBAL) == E_MGLOBAL) {
        fprintf (F, "E_MGLOBAL ");
    }
    if ((E->Flags & E_MLOCAL) == E_MLOCAL) {
        fprintf (F, "E_MLOCAL ");
    }
    if ((E->Flags & E_MCONST) == E_MCONST) {
        fprintf (F, "E_MCONST ");
    }

    fprintf (F, "\nTest:     ");
    if (E->Test & E_CC) {
        fprintf (F, "E_CC ");
    }
    if (E->Test & E_FORCETEST) {
        fprintf (F, "E_FORCETEST ");
    }

    fprintf (F, "\nName:     0x%08lX\n", E->Name);
}



type* ReplaceType (ExprDesc* Expr, const type* NewType)
/* Replace the type of Expr by a copy of Newtype and return the old type string */
{
    type* OldType = Expr->Type;
    Expr->Type = TypeDup (NewType);
    return OldType;
}



