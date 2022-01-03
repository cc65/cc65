/*****************************************************************************/
/*                                                                           */
/*                                initdata.c                                 */
/*                                                                           */
/*                    Parse and generate initializer data                    */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2015, Ullrich von Bassewitz                                      */
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



#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

/* common */
#include "addrsize.h"
#include "mmodel.h"
#include "shift.h"
#include "xmalloc.h"

/* cc65 */
#include "anonname.h"
#include "codegen.h"
#include "datatype.h"
#include "declattr.h"
#include "error.h"
#include "expr.h"
#include "exprdesc.h"
#include "funcdesc.h"
#include "function.h"
#include "global.h"
#include "litpool.h"
#include "pragma.h"
#include "scanner.h"
#include "shift.h"
#include "standard.h"
#include "symtab.h"
#include "wrappedcall.h"
#include "typeconv.h"
#include "initdata.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



typedef struct StructInitData StructInitData;
struct StructInitData {
    unsigned    Size;                   /* Size of struct */
    unsigned    Offs;                   /* Current offset in struct */
    unsigned    BitVal;                 /* Summed up bit-field value */
    unsigned    ValBits;                /* Valid bits in Val */
};



/*****************************************************************************/
/*                                 Forwards                                  */
/*****************************************************************************/



static unsigned ParseInitInternal (Type* T, int* Braces, int AllowFlexibleMembers);
/* Parse initialization of variables. Return the number of data bytes. */



/*****************************************************************************/
/*                                   code                                    */
/*****************************************************************************/



static void SkipInitializer (int BracesExpected)
/* Skip the remainder of an initializer in case of errors. Try to be somewhat
** smart so we don't have too many following errors.
*/
{
    while (CurTok.Tok != TOK_CEOF && CurTok.Tok != TOK_SEMI && BracesExpected >= 0) {
        switch (CurTok.Tok) {
            case TOK_RCURLY:    --BracesExpected;   break;
            case TOK_LCURLY:    ++BracesExpected;   break;
            default:                                break;
        }
        if (BracesExpected >= 0) {
            NextToken ();
        }
    }
}



static unsigned OpeningCurlyBraces (unsigned BracesNeeded)
/* Accept any number of opening curly braces around an initialization, skip
** them and return the number. If the number of curly braces is less than
** BracesNeeded, issue a warning.
*/
{
    unsigned BraceCount = 0;
    while (CurTok.Tok == TOK_LCURLY) {
        ++BraceCount;
        NextToken ();
    }
    if (BraceCount < BracesNeeded) {
        Error ("'{' expected");
    }
    return BraceCount;
}



static void ClosingCurlyBraces (unsigned BracesExpected)
/* Accept and skip the given number of closing curly braces together with
** an optional comma. Output an error messages, if the input does not contain
** the expected number of braces.
*/
{
    while (BracesExpected) {
        /* TODO: Skip all excess initializers until next closing curly brace */
        if (CurTok.Tok == TOK_RCURLY) {
            NextToken ();
        } else if (CurTok.Tok == TOK_COMMA && NextTok.Tok == TOK_RCURLY) {
            NextToken ();
            NextToken ();
        } else {
            Error ("'}' expected");
            return;
        }
        --BracesExpected;
    }
}



static void DefineData (ExprDesc* Expr)
/* Output a data definition for the given expression */
{
    switch (ED_GetLoc (Expr)) {

        case E_LOC_NONE:
            /* Immediate numeric value with no storage */
            g_defdata (CF_IMM | TypeOf (Expr->Type) | CF_CONST, Expr->IVal, 0);
            break;

        case E_LOC_ABS:
            /* Absolute numeric address */
            g_defdata (CF_ABSOLUTE | TypeOf (Expr->Type) | CF_CONST, Expr->IVal, 0);
            break;

        case E_LOC_GLOBAL:
            /* Global variable */
            g_defdata (CF_EXTERNAL, Expr->Name, Expr->IVal);
            break;

        case E_LOC_STATIC:
            /* Static variable */
            g_defdata (CF_STATIC, Expr->Name, Expr->IVal);
            break;

        case E_LOC_LITERAL:
            /* Literal in the literal pool */
            g_defdata (CF_LITERAL, Expr->Name, Expr->IVal);
            break;

        case E_LOC_REGISTER:
            /* Register variable. Taking the address is usually not
            ** allowed.
            */
            if (IS_Get (&AllowRegVarAddr) == 0) {
                Error ("Cannot take the address of a register variable");
            }
            g_defdata (CF_REGVAR, Expr->Name, Expr->IVal);
            break;

        case E_LOC_CODE:
            /* Code label location */
            g_defdata (CF_CODE, Expr->Name, Expr->IVal);
            break;

        case E_LOC_STACK:
        case E_LOC_PRIMARY:
        case E_LOC_EXPR:
            Error ("Non constant initializer");
            break;

        default:
            Internal ("Unknown constant type: 0x%04X", ED_GetLoc (Expr));
    }
}



static void DefineBitFieldData (StructInitData* SI)
/* Output bit field data */
{
    /* Ignore if we have no data */
    if (SI->ValBits > 0) {

        /* Output the data */
        g_defdata (CF_CHAR | CF_UNSIGNED | CF_CONST, SI->BitVal, 0);

        /* Update the data from SI and account for the size */
        if (SI->ValBits >= CHAR_BITS) {
            SI->BitVal >>= CHAR_BITS;
            SI->ValBits -= CHAR_BITS;
        } else {
            SI->BitVal  = 0;
            SI->ValBits = 0;
        }
        SI->Offs += SIZEOF_CHAR;
    }
}



static void DefineStrData (Literal* Lit, unsigned Count)
{
    /* Translate into target charset */
    TranslateLiteral (Lit);

    /* Output the data */
    g_defbytes (GetLiteralStr (Lit), Count);
}



static ExprDesc ParseScalarInitInternal (const Type* T)
/* Parse initializaton for scalar data types. This function will not output the
** data but return it in ED.
*/
{
    /* Optional opening brace */
    unsigned BraceCount = OpeningCurlyBraces (0);

    /* We warn if an initializer for a scalar contains braces, because this is
    ** quite unusual and often a sign for some problem in the input.
    */
    if (BraceCount > 0) {
        Warning ("Braces around scalar initializer");
    }

    /* Get the expression and convert it to the target type */
    ExprDesc ED = NoCodeConstExpr (hie1);
    TypeConversion (&ED, T);

    /* Close eventually opening braces */
    ClosingCurlyBraces (BraceCount);

    return ED;
}



static unsigned ParseScalarInit (const Type* T)
/* Parse initializaton for scalar data types. Return the number of data bytes. */
{
    /* Parse initialization */
    ExprDesc ED = ParseScalarInitInternal (T);

    /* Output the data */
    DefineData (&ED);

    /* Do this anyways for safety */
    DoDeferred (SQP_KEEP_NONE, &ED);

    /* Done */
    return SizeOf (T);
}



static unsigned ParsePointerInit (const Type* T)
/* Parse initializaton for pointer data types. Return the number of data bytes. */
{
    /* Optional opening brace */
    unsigned BraceCount = OpeningCurlyBraces (0);

    /* Expression */
    ExprDesc ED = NoCodeConstExpr (hie1);
    TypeConversion (&ED, T);

    /* Output the data */
    DefineData (&ED);

    /* Do this anyways for safety */
    DoDeferred (SQP_KEEP_NONE, &ED);

    /* Close eventually opening braces */
    ClosingCurlyBraces (BraceCount);

    /* Done */
    return SIZEOF_PTR;
}



static unsigned ParseArrayInit (Type* T, int* Braces, int AllowFlexibleMembers)
/* Parse initializaton for arrays. Return the number of data bytes. */
{
    int Count;
    int HasCurly = 0;

    /* Get the array data */
    Type* ElementType    = IndirectModifiable (T);
    unsigned ElementSize = SizeOf (ElementType);
    long ElementCount    = GetElementCount (T);

    /* Special handling for a character array initialized by a literal */
    if (IsClassChar (ElementType) &&
        (CurTok.Tok == TOK_SCONST || CurTok.Tok == TOK_WCSCONST ||
        (CurTok.Tok == TOK_LCURLY &&
         (NextTok.Tok == TOK_SCONST || NextTok.Tok == TOK_WCSCONST)))) {

        /* Char array initialized by string constant */
        int NeedParen;

        /* If we initializer is enclosed in brackets, remember this fact and
        ** skip the opening bracket.
        */
        NeedParen = (CurTok.Tok == TOK_LCURLY);
        if (NeedParen) {
            NextToken ();
        }

        /* If the array is one too small for the string literal, omit the
        ** trailing zero.
        */
        Count = GetLiteralSize (CurTok.SVal);
        if (ElementCount != UNSPECIFIED &&
            ElementCount != FLEXIBLE    &&
            Count        == ElementCount + 1) {
            /* Omit the trailing zero */
            --Count;
        }

        /* Output the data */
        DefineStrData (CurTok.SVal, Count);

        /* Skip the string */
        NextToken ();

        /* If the initializer was enclosed in curly braces, we need a closing
        ** one.
        */
        if (NeedParen) {
            ConsumeRCurly ();
        }

    } else {

        /* Arrays can be initialized without a pair of curly braces */
        if (*Braces == 0 || CurTok.Tok == TOK_LCURLY) {
            /* Consume the opening curly brace */
            HasCurly = ConsumeLCurly ();
            *Braces += HasCurly;
        }

        /* Initialize the array members */
        Count = 0;
        while (CurTok.Tok != TOK_RCURLY) {
            /* Flexible array members may not be initialized within
            ** an array (because the size of each element may differ
            ** otherwise).
            */
            ParseInitInternal (ElementType, Braces, 0);
            ++Count;
            if (CurTok.Tok != TOK_COMMA)
                break;
            NextToken ();
        }

        if (HasCurly) {
            /* Closing curly braces */
            ConsumeRCurly ();
        }
    }

    /* Size of 'void' elements are determined after initialization */
    if (ElementSize == 0) {
        ElementSize = SizeOf (ElementType);
    }

    if (ElementCount == UNSPECIFIED) {
        /* Number of elements determined by initializer */
        SetElementCount (T, Count);
        ElementCount = Count;
    } else if (ElementCount == FLEXIBLE) {
        if (AllowFlexibleMembers) {
            /* In non ANSI mode, allow initialization of flexible array
            ** members.
            */
            ElementCount = Count;
        } else {
            /* Forbid */
            Error ("Initializing flexible array member is forbidden");
            ElementCount = Count;
        }
    } else if (Count < ElementCount) {
        g_zerobytes ((ElementCount - Count) * ElementSize);
    } else if (Count > ElementCount && HasCurly) {
        Error ("Excess elements in array initializer");
    }
    return ElementCount * ElementSize;
}



static unsigned ParseStructInit (Type* T, int* Braces, int AllowFlexibleMembers)
/* Parse initialization of a struct or union. Return the number of data bytes. */
{
    SymEntry*       Sym;
    SymTable*       Tab;
    StructInitData  SI;
    int             HasCurly  = 0;
    int             SkipComma = 0;


    /* Fields can be initialized without a pair of curly braces */
    if (*Braces == 0 || CurTok.Tok == TOK_LCURLY) {
        /* Consume the opening curly brace */
        HasCurly = ConsumeLCurly ();
        *Braces += HasCurly;
    }

    /* Get a pointer to the struct entry from the type */
    Sym = GetESUSymEntry (T);

    /* Get the size of the struct from the symbol table entry */
    SI.Size = Sym->V.S.Size;

    /* Check if this struct definition has a field table. If it doesn't, it
    ** is an incomplete definition.
    */
    Tab = Sym->V.S.SymTab;
    if (Tab == 0) {
        Error ("Cannot initialize variables with incomplete type");
        /* Try error recovery */
        SkipInitializer (HasCurly);
        /* Nothing initialized */
        return 0;
    }

    /* Get a pointer to the list of symbols */
    Sym = Tab->SymHead;

    /* Initialize fields */
    SI.Offs    = 0;
    SI.BitVal  = 0;
    SI.ValBits = 0;
    while (CurTok.Tok != TOK_RCURLY) {

        /* Check for excess elements */
        if (Sym == 0) {
            /* Is there just one trailing comma before a closing curly? */
            if (NextTok.Tok == TOK_RCURLY && CurTok.Tok == TOK_COMMA) {
                /* Skip comma and exit scope */
                NextToken ();
                break;
            }

            if (HasCurly) {
                Error ("Excess elements in %s initializer", GetBasicTypeName (T));
                SkipInitializer (HasCurly);
            }
            return SI.Offs;
        }

        /* Check for special members that don't consume the initializer */
        if ((Sym->Flags & SC_ALIAS) == SC_ALIAS) {
            /* Just skip */
            goto NextMember;
        }

        /* This may be an anonymous bit-field, in which case it doesn't
        ** have an initializer.
        */
        if (SymIsBitField (Sym) && (IsAnonName (Sym->Name))) {
            /* Account for the data and output it if we have at least a full
            ** word. We may have more if there was storage unit overlap, for
            ** example two consecutive 10 bit fields. These will be packed
            ** into 3 bytes.
            */
            SI.ValBits += Sym->Type->A.B.Width;
            /* TODO: Generalize this so any type can be used. */
            CHECK (SI.ValBits <= CHAR_BITS + INT_BITS - 2);
            while (SI.ValBits >= CHAR_BITS) {
                DefineBitFieldData (&SI);
            }
            /* Avoid consuming the comma if any */
            goto NextMember;
        }

        /* Skip comma this round */
        if (SkipComma) {
            NextToken ();
            SkipComma = 0;
        }

        if (SymIsBitField (Sym)) {

            /* Parse initialization of one field. Bit-fields need a special
            ** handling.
            */
            ExprDesc ED;
            ED_Init (&ED);
            unsigned Val;
            unsigned Shift;

            /* Calculate the bitmask from the bit-field data */
            unsigned Mask = (1U << Sym->Type->A.B.Width) - 1U;

            /* Safety ... */
            CHECK (Sym->V.Offs * CHAR_BITS + Sym->Type->A.B.Offs ==
                   SI.Offs     * CHAR_BITS + SI.ValBits);

            /* Read the data, check for a constant integer, do a range check */
            ED = ParseScalarInitInternal (IntPromotion (Sym->Type));
            if (!ED_IsConstAbsInt (&ED)) {
                Error ("Constant initializer expected");
                ED_MakeConstAbsInt (&ED, 1);
            }

            /* Truncate the initializer value to the width of the bit-field and check if we lost
            ** any useful bits.
            */
            Val = (unsigned) ED.IVal & Mask;
            if (IsSignUnsigned (Sym->Type)) {
                if (ED.IVal < 0 || (unsigned long) ED.IVal != Val) {
                    Warning ("Implicit truncation from '%s' to '%s : %u' in bit-field initializer"
                             " changes value from %ld to %u",
                             GetFullTypeName (ED.Type), GetFullTypeName (Sym->Type),
                             Sym->Type->A.B.Width, ED.IVal, Val);
                }
            } else {
                /* Sign extend back to full width of host long. */
                unsigned ShiftBits = sizeof (long) * CHAR_BIT - Sym->Type->A.B.Width;
                long RestoredVal = asr_l(asl_l (Val, ShiftBits), ShiftBits);
                if (ED.IVal != RestoredVal) {
                    Warning ("Implicit truncation from '%s' to '%s : %u' in bit-field initializer "
                             "changes value from %ld to %ld",
                             GetFullTypeName (ED.Type), GetFullTypeName (Sym->Type),
                             Sym->Type->A.B.Width, ED.IVal, RestoredVal);
                }
            }

            /* Add the value to the currently stored bit-field value */
            Shift = (Sym->V.Offs - SI.Offs) * CHAR_BITS + Sym->Type->A.B.Offs;
            SI.BitVal |= (Val << Shift);

            /* Account for the data and output any full bytes we have. */
            SI.ValBits += Sym->Type->A.B.Width;
            /* Make sure unsigned is big enough to hold the value, 22 bits.
            ** This is 22 bits because the most we can have is 7 bits left
            ** over from the previous OutputBitField call, plus 15 bits
            ** from this field.  A 16-bit bit-field will always be byte
            ** aligned, so will have padding before it.
            */
            CHECK (SI.ValBits <= CHAR_BIT * sizeof(SI.BitVal));
            /* TODO: Generalize this so any type can be used. */
            CHECK (SI.ValBits <= CHAR_BITS + INT_BITS - 2);
            while (SI.ValBits >= CHAR_BITS) {
                DefineBitFieldData (&SI);
            }

        } else {

            /* Standard member. We should never have stuff from a
            ** bit-field left because an anonymous member was added
            ** for padding by ParseStructDecl.
            */
            CHECK (SI.ValBits == 0);

            /* Flexible array members may only be initialized if they are
            ** the last field (or part of the last struct field).
            */
            SI.Offs += ParseInitInternal (Sym->Type, Braces, AllowFlexibleMembers && Sym->NextSym == 0);
        }

        /* More initializers? */
        if (CurTok.Tok != TOK_COMMA) {
            break;
        }

        /* Skip the comma next round */
        SkipComma = 1;

NextMember:
        /* Next member. For unions, only the first one can be initialized */
        if (IsTypeUnion (T)) {
            /* Union */
            Sym = 0;
        } else {
            /* Struct */
            Sym = Sym->NextSym;
        }
    }

    if (HasCurly) {
        /* Consume the closing curly brace */
        ConsumeRCurly ();
    }

    /* If we have data from a bit-field left, output it now */
    CHECK (SI.ValBits < CHAR_BITS);
    DefineBitFieldData (&SI);

    /* If there are struct fields left, reserve additional storage */
    if (SI.Offs < SI.Size) {
        g_zerobytes (SI.Size - SI.Offs);
        SI.Offs = SI.Size;
    }

    /* Return the actual number of bytes initialized. This number may be
    ** larger than sizeof (Struct) if flexible array members are present and
    ** were initialized (possible in non ANSI mode).
    */
    return SI.Offs;
}



static unsigned ParseVoidInit (Type* T)
/* Parse an initialization of a void variable (special cc65 extension).
** Return the number of bytes initialized.
*/
{
    unsigned Size;

    /* Opening brace */
    ConsumeLCurly ();

    /* Allow an arbitrary list of values */
    Size = 0;
    do {
        ExprDesc Expr = NoCodeConstExpr (hie1);
        switch (GetUnderlyingTypeCode (&Expr.Type[0])) {

            case T_SCHAR:
            case T_UCHAR:
                if (ED_IsConstAbsInt (&Expr)) {
                    /* Make it byte sized */
                    Expr.IVal &= 0xFF;
                }
                DefineData (&Expr);
                Size += SIZEOF_CHAR;
                break;

            case T_SHORT:
            case T_USHORT:
            case T_INT:
            case T_UINT:
            case T_PTR:
            case T_ARRAY:
                if (ED_IsConstAbsInt (&Expr)) {
                    /* Make it word sized */
                    Expr.IVal &= 0xFFFF;
                }
                DefineData (&Expr);
                Size += SIZEOF_INT;
                break;

            case T_LONG:
            case T_ULONG:
                if (ED_IsConstAbsInt (&Expr)) {
                    /* Make it dword sized */
                    Expr.IVal &= 0xFFFFFFFF;
                }
                DefineData (&Expr);
                Size += SIZEOF_LONG;
                break;

            default:
                Error ("Illegal type in initialization");
                break;

        }

        if (CurTok.Tok != TOK_COMMA) {
            break;
        }
        NextToken ();

    } while (CurTok.Tok != TOK_RCURLY);

    /* Closing brace */
    ConsumeRCurly ();

    /* Number of bytes determined by initializer */
    if (T->A.U != 0 && T->A.U != Size) {
        Error ("'void' array initialized with elements of variant sizes");
    } else {
        T->A.U = Size;
    }

    /* Return the number of bytes initialized */
    return Size;
}



static unsigned ParseInitInternal (Type* T, int *Braces, int AllowFlexibleMembers)
/* Parse initialization of variables. Return the number of data bytes. */
{
    switch (GetUnderlyingTypeCode (T)) {

        case T_SCHAR:
        case T_UCHAR:
        case T_SHORT:
        case T_USHORT:
        case T_INT:
        case T_UINT:
        case T_LONG:
        case T_ULONG:
        case T_FLOAT:
        case T_DOUBLE:
            return ParseScalarInit (T);

        case T_PTR:
            return ParsePointerInit (T);

        case T_ARRAY:
            return ParseArrayInit (T, Braces, AllowFlexibleMembers);

        case T_STRUCT:
        case T_UNION:
            return ParseStructInit (T, Braces, AllowFlexibleMembers);

        case T_ENUM:
            /* Incomplete enum type must have already raised errors.
            ** Just proceed to consume the value.
            */
            return ParseScalarInit (T);

        case T_VOID:
            if (IS_Get (&Standard) == STD_CC65) {
                /* Special cc65 extension in non-ANSI mode */
                return ParseVoidInit (T);
            }
            /* FALLTHROUGH */

        default:
            Error ("Illegal type");
            return SIZEOF_CHAR;

    }
}



unsigned ParseInit (Type* T)
/* Parse initialization of variables. Return the number of data bytes. */
{
    /* Current curly braces layers */
    int Braces = 0;

    /* Parse the initialization. Flexible array members can only be initialized
    ** in cc65 mode.
    */
    unsigned Size = ParseInitInternal (T, &Braces, IS_Get (&Standard) == STD_CC65);

    /* The initialization may not generate code on global level, because code
    ** outside function scope will never get executed.
    */
    if (HaveGlobalCode ()) {
        Error ("Non constant initializers");
        RemoveGlobalCode ();
    }

    /* Return the size needed for the initialization */
    return Size;
}
