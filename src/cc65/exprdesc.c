/*****************************************************************************/
/*                                                                           */
/*                                exprdesc.c                                 */
/*                                                                           */
/*                      Expression descriptor structure                      */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2002-2010, Ullrich von Bassewitz                                      */
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
#include "check.h"
#include "strbuf.h"

/* cc65 */
#include "asmlabel.h"
#include "datatype.h"
#include "error.h"
#include "exprdesc.h"
#include "stackptr.h"
#include "symentry.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



ExprDesc* ED_Init (ExprDesc* Expr)
/* Initialize an ExprDesc */
{
    Expr->Sym       = 0;
    Expr->Type      = 0;
    Expr->Flags     = E_NEED_EAX;
    Expr->Name      = 0;
    Expr->IVal      = 0;
    Expr->FVal      = FP_D_Make (0.0);
    Expr->LVal      = 0;
    Expr->BitOffs   = 0;
    Expr->BitWidth  = 0;
    return Expr;
}



void ED_MakeBitField (ExprDesc* Expr, unsigned BitOffs, unsigned BitWidth)
/* Make this expression a bit field expression */
{
    Expr->Flags   |= E_BITFIELD;
    Expr->BitOffs  = BitOffs;
    Expr->BitWidth = BitWidth;
}



#if !defined(HAVE_INLINE)
int ED_IsLocQuasiConst (const ExprDesc* Expr)
/* Return true if the expression is a constant location of some sort or on the
** stack.
*/
{
    return ED_IsLocConst (Expr) || ED_IsLocStack (Expr);
}
#endif



#if !defined(HAVE_INLINE)
int ED_IsLocPrimaryOrExpr (const ExprDesc* Expr)
/* Return true if the expression is E_LOC_PRIMARY or E_LOC_EXPR */
{
    return ED_IsLocPrimary (Expr) || ED_IsLocExpr (Expr);
}
#endif



#if !defined(HAVE_INLINE)
int ED_IsIndExpr (const ExprDesc* Expr)
/* Check if the expression is a reference to its value */
{
    return (Expr->Flags & E_ADDRESS_OF) == 0 &&
           !ED_IsLocNone (Expr) && !ED_IsLocPrimary (Expr);
}
#endif



int ED_YetToLoad (const ExprDesc* Expr)
/* Check if the expression needs to be loaded somehow. */
{
    return ED_NeedsPrimary (Expr)   ||
           ED_YetToTest (Expr)      ||
           (ED_IsLVal (Expr) && IsQualVolatile (Expr->Type));
}



void ED_MarkForUneval (ExprDesc* Expr)
/* Mark the expression as not to be evaluated */
{
    Expr->Flags = (Expr->Flags & ~E_MASK_EVAL) | E_EVAL_UNEVAL;
}



void ED_SetCodeRange (ExprDesc* Expr, const CodeMark* Start, const CodeMark* End)
/* Set the code range for this expression */
{
    Expr->Flags |= E_HAVE_MARKS;
    Expr->Start = *Start;
    Expr->End   = *End;
}



int ED_CodeRangeIsEmpty (const ExprDesc* Expr)
/* Return true if no code was output for this expression */
{
    /* We must have code marks */
    PRECONDITION (Expr->Flags & E_HAVE_MARKS);

    return CodeRangeIsEmpty (&Expr->Start, &Expr->End);
}



const char* ED_GetLabelName (const ExprDesc* Expr, long Offs)
/* Return the assembler label name of the given expression. Beware: This
** function may use a static buffer, so the name may get "lost" on the second
** call to the function.
*/
{
    static StrBuf Buf = STATIC_STRBUF_INITIALIZER;

    /* Expr may have it's own offset, adjust Offs accordingly */
    Offs += Expr->IVal;

    /* Generate a label depending on the location */
    switch (ED_GetLoc (Expr)) {

        case E_LOC_NONE:
        case E_LOC_ABS:
            /* Absolute numeric addressed variable */
            SB_Printf (&Buf, "$%04X", (int)(Offs & 0xFFFF));
            break;

        case E_LOC_GLOBAL:
        case E_LOC_STATIC:
            /* Global or static variable */
            if (Offs) {
                SB_Printf (&Buf, "%s%+ld", SymGetAsmName (Expr->Sym), Offs);
            } else {
                SB_Printf (&Buf, "%s", SymGetAsmName (Expr->Sym));
            }
            break;

        case E_LOC_REGISTER:
            /* Register variable */
            SB_Printf (&Buf, "regbank+%u", (unsigned)((Offs + Expr->Name) & 0xFFFFU));
            break;

        case E_LOC_LITERAL:
            /* Literal in the literal pool */
            if (Offs) {
                SB_Printf (&Buf, "%s%+ld", PooledLiteralLabelName (Expr->Name), Offs);
            } else {
                SB_Printf (&Buf, "%s", PooledLiteralLabelName (Expr->Name));
            }
            break;

        case E_LOC_CODE:
            /* Code label location */
            if (Offs) {
                SB_Printf (&Buf, "%s%+ld", LocalLabelName (Expr->Name), Offs);
            } else {
                SB_Printf (&Buf, "%s", LocalLabelName (Expr->Name));
            }
            break;

        default:
            Internal ("Invalid location in ED_GetLabelName: 0x%04X", ED_GetLoc (Expr));
    }

    /* Return a pointer to the static buffer */
    return SB_GetConstBuf (&Buf);
}



int ED_GetStackOffs (const ExprDesc* Expr, int Offs)
/* Get the stack offset of an address on the stack in Expr taking into account
** an additional offset in Offs.
*/
{
    PRECONDITION (ED_IsLocStack (Expr));
    Offs += ((int) Expr->IVal) - StackPtr;
    CHECK (Offs >= 0);          /* Cannot handle negative stack offsets */
    return Offs;
}



ExprDesc* ED_MakeConstAbs (ExprDesc* Expr, long Value, Type* Type)
/* Replace Expr with an absolute const with the given value and type */
{
    Expr->Sym   = 0;
    Expr->Type  = Type;
    Expr->Flags = E_LOC_NONE | E_RTYPE_RVAL | (Expr->Flags & E_MASK_KEEP_MAKE);
    Expr->Name  = 0;
    Expr->IVal  = Value;
    Expr->FVal  = FP_D_Make (0.0);
    return Expr;
}



ExprDesc* ED_MakeConstAbsInt (ExprDesc* Expr, long Value)
/* Replace Expr with a constant integer expression with the given value */
{
    Expr->Sym   = 0;
    Expr->Type  = type_int;
    Expr->Flags = E_LOC_NONE | E_RTYPE_RVAL | (Expr->Flags & E_MASK_KEEP_MAKE);
    Expr->Name  = 0;
    Expr->IVal  = Value;
    Expr->FVal  = FP_D_Make (0.0);
    return Expr;
}



ExprDesc* ED_MakeConstBool (ExprDesc* Expr, long Value)
/* Replace Expr with a constant boolean expression with the given value */
{
    Expr->Sym   = 0;
    Expr->Type  = type_bool;
    Expr->Flags = E_LOC_NONE | E_RTYPE_RVAL | (Expr->Flags & E_HAVE_MARKS);
    Expr->Name  = 0;
    Expr->IVal  = Value;
    Expr->FVal  = FP_D_Make (0.0);
    return Expr;
}



ExprDesc* ED_FinalizeRValLoad (ExprDesc* Expr)
/* Finalize the result of LoadExpr to be an rvalue in the primary register */
{
    Expr->Sym   = 0;
    Expr->Flags &= ~(E_MASK_LOC | E_MASK_RTYPE | E_BITFIELD | E_ADDRESS_OF);
    Expr->Flags &= ~E_CC_SET;
    Expr->Flags |= (E_LOC_PRIMARY | E_RTYPE_RVAL);
    Expr->Name  = 0;
    Expr->IVal  = 0;    /* No offset */
    Expr->FVal  = FP_D_Make (0.0);
    return Expr;
}



ExprDesc* ED_AddrExpr (ExprDesc* Expr)
/* Take address of Expr. The result is always an rvalue */
{
    switch (Expr->Flags & E_MASK_LOC) {
        case E_LOC_NONE:
            Error ("Cannot get the address of a numeric constant");
            break;

        case E_LOC_EXPR:
            Expr->Flags &= ~(E_MASK_LOC | E_MASK_RTYPE);
            Expr->Flags |= E_LOC_PRIMARY | E_RTYPE_RVAL;
            break;

        default:
            if ((Expr->Flags & E_ADDRESS_OF) == 0) {
                Expr->Flags &= ~E_MASK_RTYPE;
                Expr->Flags |= E_ADDRESS_OF | E_RTYPE_RVAL;
            } else {
                /* Due to the way we handle arrays, this may happen if we take
                ** the address of a pointer to an array element.
                */
                if (!IsTypePtr (Expr->Type)) {
                    Error ("Cannot get the address of an address");
                }
                Expr->Flags &= ~E_MASK_RTYPE;
                Expr->Flags |= E_RTYPE_RVAL;
            }
            break;
    }
    return Expr;
}



ExprDesc* ED_IndExpr (ExprDesc* Expr)
/* Dereference Expr */
{
    switch (Expr->Flags & E_MASK_LOC) {
        case E_LOC_NONE:
            Expr->Flags &= ~(E_MASK_LOC | E_MASK_RTYPE);
            Expr->Flags |= E_LOC_ABS | E_RTYPE_LVAL;
            break;

        case E_LOC_PRIMARY:
            Expr->Flags &= ~(E_MASK_LOC | E_MASK_RTYPE);
            Expr->Flags |= E_LOC_EXPR | E_RTYPE_LVAL;
            break;

        default:
            if ((Expr->Flags & E_ADDRESS_OF) != 0) {
                Expr->Flags &= ~(E_MASK_RTYPE | E_ADDRESS_OF);
                Expr->Flags |= E_RTYPE_LVAL;
            } else {
                /* Due to the limitation of LoadExpr, this may happen after we
                ** have loaded the value from a referenced address, in which
                ** case the content in the primary no longer refers to the
                ** original address. We simply mark this as E_LOC_EXPR so that
                ** some info about the original location can be retained.
                ** If it's really meant to dereference a "pointer value", it
                ** should be done in two steps where the pointervalue should
                ** be the manually loaded first before a call into this, and
                ** the offset should be manually cleared somewhere outside.
                */
                Expr->Flags &= ~(E_MASK_LOC | E_MASK_RTYPE);
                Expr->Flags |= E_LOC_EXPR | E_RTYPE_LVAL;
            }
            break;
    }
    return Expr;
}



#if !defined(HAVE_INLINE)
int ED_IsAbs (const ExprDesc* Expr)
/* Return true if the expression denotes a numeric value or address. */
{
    return (Expr->Flags & (E_MASK_LOC)) == (E_LOC_NONE) ||
           (Expr->Flags & (E_MASK_LOC|E_ADDRESS_OF)) == (E_LOC_ABS|E_ADDRESS_OF);
}
#endif



#if !defined(HAVE_INLINE)
int ED_IsConstAbs (const ExprDesc* Expr)
/* Return true if the expression denotes a constant absolute value. This can be
** a numeric constant, cast to any type.
*/
{
    return ED_IsRVal (Expr) && ED_IsAbs (Expr);
}
#endif



int ED_IsConstAbsInt (const ExprDesc* Expr)
/* Return true if the expression is a constant (numeric) integer. */
{
    return ED_IsConstAbs (Expr) && IsClassInt (Expr->Type);
}



int ED_IsConstBool (const ExprDesc* Expr)
/* Return true if the expression can be constantly evaluated as a boolean. */
{
    return ED_IsConstAbsInt (Expr) || ED_IsAddrExpr (Expr);
}



int ED_IsConst (const ExprDesc* Expr)
/* Return true if the expression denotes a constant of some sort. This can be a
** numeric constant, the address of a global variable (maybe with offset) or
** similar.
*/
{
    return (Expr->Flags & E_MASK_LOC) == E_LOC_NONE || ED_IsConstAddr (Expr);
}



int ED_IsConstAddr (const ExprDesc* Expr)
/* Return true if the expression denotes a constant address of some sort. This
** can be the address of a global variable (maybe with offset) or similar.
*/
{
    return ED_IsAddrExpr (Expr) && ED_IsLocConst (Expr);
}



int ED_IsQuasiConstAddr (const ExprDesc* Expr)
/* Return true if the expression denotes a quasi-constant address of some sort.
** This can be a constant address or a stack variable address.
*/
{
    return ED_IsAddrExpr (Expr) && ED_IsLocQuasiConst (Expr);
}



int ED_IsNullPtr (const ExprDesc* Expr)
/* Return true if the given expression is a NULL pointer constant */
{
    return (Expr->Flags & (E_MASK_LOC|E_MASK_RTYPE|E_BITFIELD)) ==
                                (E_LOC_NONE|E_RTYPE_RVAL) &&
           Expr->IVal == 0                               &&
           IsClassInt (Expr->Type);
}



int ED_IsBool (const ExprDesc* Expr)
/* Return true of the expression can be treated as a boolean, that is, it can
** be an operand to a compare operation.
*/
{
    /* Either ints, floats, or pointers can be used in a boolean context */
    return IsClassInt (Expr->Type)   ||
           IsClassFloat (Expr->Type) ||
           IsClassPtr (Expr->Type)   ||
           IsClassFunc (Expr->Type);
}



void PrintExprDesc (FILE* F, ExprDesc* E)
/* Print an ExprDesc */
{
    unsigned Flags;
    char     Sep;

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
    fprintf (F, "IVal:     0x%08lX\n", E->IVal);
    fprintf (F, "FVal:     %f\n", FP_D_ToFloat (E->FVal));

    Flags = E->Flags;
    Sep   = '(';
    fprintf (F, "Flags:    0x%04X ", Flags);
    if ((Flags & E_MASK_LOC) == E_LOC_NONE) {
        fprintf (F, "%cE_LOC_NONE", Sep);
        Flags &= ~E_LOC_NONE;
        Sep = ',';
    }
    if (Flags & E_LOC_ABS) {
        fprintf (F, "%cE_LOC_ABS", Sep);
        Flags &= ~E_LOC_ABS;
        Sep = ',';
    }
    if (Flags & E_LOC_GLOBAL) {
        fprintf (F, "%cE_LOC_GLOBAL", Sep);
        Flags &= ~E_LOC_GLOBAL;
        Sep = ',';
    }
    if (Flags & E_LOC_STATIC) {
        fprintf (F, "%cE_LOC_STATIC", Sep);
        Flags &= ~E_LOC_STATIC;
        Sep = ',';
    }
    if (Flags & E_LOC_REGISTER) {
        fprintf (F, "%cE_LOC_REGISTER", Sep);
        Flags &= ~E_LOC_REGISTER;
        Sep = ',';
    }
    if (Flags & E_LOC_STACK) {
        fprintf (F, "%cE_LOC_STACK", Sep);
        Flags &= ~E_LOC_STACK;
        Sep = ',';
    }
    if (Flags & E_LOC_PRIMARY) {
        fprintf (F, "%cE_LOC_PRIMARY", Sep);
        Flags &= ~E_LOC_PRIMARY;
        Sep = ',';
    }
    if (Flags & E_LOC_EXPR) {
        fprintf (F, "%cE_LOC_EXPR", Sep);
        Flags &= ~E_LOC_EXPR;
        Sep = ',';
    }
    if (Flags & E_LOC_LITERAL) {
        fprintf (F, "%cE_LOC_LITERAL", Sep);
        Flags &= ~E_LOC_LITERAL;
        Sep = ',';
    }
    if (Flags & E_LOC_CODE) {
        fprintf (F, "%cE_LOC_CODE", Sep);
        Flags &= ~E_LOC_CODE;
        Sep = ',';
    }
    if (Flags & E_BITFIELD) {
        fprintf (F, "%cE_BITFIELD", Sep);
        Flags &= ~E_BITFIELD;
        Sep = ',';
    }
    if (Flags & E_NEED_TEST) {
        fprintf (F, "%cE_NEED_TEST", Sep);
        Flags &= ~E_NEED_TEST;
        Sep = ',';
    }
    if (Flags & E_CC_SET) {
        fprintf (F, "%cE_CC_SET", Sep);
        Flags &= ~E_CC_SET;
        Sep = ',';
    }
    if (Flags & E_RTYPE_LVAL) {
        fprintf (F, "%cE_RTYPE_LVAL", Sep);
        Flags &= ~E_RTYPE_LVAL;
        Sep = ',';
    }
    if (Flags & E_ADDRESS_OF) {
        fprintf (F, "%cE_ADDRESS_OF", Sep);
        Flags &= ~E_ADDRESS_OF;
        Sep = ',';
    }
    if (Flags) {
        fprintf (F, "%c,0x%04X", Sep, Flags);
        Sep = ',';
    }
    if (Sep != '(') {
        fputc (')', F);
    }
    fprintf (F, "\nName:     0x%08lX\n", (unsigned long)E->Name);
}



Type* ReplaceType (ExprDesc* Expr, const Type* NewType)
/* Replace the type of Expr by a copy of Newtype and return the old type string */
{
    Type* OldType = Expr->Type;
    Expr->Type = TypeDup (NewType);
    return OldType;
}
