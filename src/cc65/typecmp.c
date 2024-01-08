/*****************************************************************************/
/*                                                                           */
/*                                 typecmp.c                                 */
/*                                                                           */
/*               Type compare function for the cc65 C compiler               */
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



#include <string.h>

/* cc65 */
#include "error.h"
#include "funcdesc.h"
#include "global.h"
#include "symtab.h"
#include "typecmp.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static int EqualFuncParams (const FuncDesc* F1, const FuncDesc* F2)
/* Compare two function symbol tables regarding function parameters. Return 1
** if they are equal and 0 otherwise.
*/
{
    /* Get the symbol tables */
    const SymTable* Tab1 = F1->SymTab;
    const SymTable* Tab2 = F2->SymTab;

    /* Compare the parameter lists */
    const SymEntry* Sym1 = Tab1->SymHead;
    const SymEntry* Sym2 = Tab2->SymHead;

    /* Compare the fields */
    while (Sym1 && (Sym1->Flags & SC_PARAM) && Sym2 && (Sym2->Flags & SC_PARAM)) {

        /* Get the symbol types */
        const Type* Type1 = Sym1->Type;
        const Type* Type2 = Sym2->Type;
        typecmp_t   CmpResult;

        /* If either of both functions is old style, apply the default
        ** promotions to the parameter type.
        */
        if (F1->Flags & FD_OLDSTYLE) {
            if (IsClassInt (Type1)) {
                Type1 = IntPromotion (Type1);
            }
        }
        if (F2->Flags & FD_OLDSTYLE) {
            if (IsClassInt (Type2)) {
                Type2 = IntPromotion (Type2);
            }
        }

        /* Compare types of this parameter */
        CmpResult = TypeCmp (Type1, Type2);
        if (CmpResult.C < TC_EQUAL || (CmpResult.F & TCF_MASK_PARAM_DIFF) != 0) {
            /* The types are not compatible */
            return 0;
        }

        /* Get the pointers to the next fields */
        Sym1 = Sym1->NextSym;
        Sym2 = Sym2->NextSym;
    }

    /* Check both pointers against NULL or a non parameter to compare the
    ** field count
    */
    return (Sym1 == 0 || (Sym1->Flags & SC_PARAM) == 0) &&
           (Sym2 == 0 || (Sym2->Flags & SC_PARAM) == 0);
}



static void SetResult (typecmp_t* Result, typecmpcode_t Val)
/* Set a new result value if it is less than the existing one */
{
    if (Val < Result->C) {
        if (Result->Indirections > 0) {
            if (Val >= TC_STRICT_COMPATIBLE) {
                /* Arrays etc. */
                Result->C = Val;
            } else if (Result->Indirections == 1) {
                /* C Standard allows implicit conversion as long as one side is
                ** a pointer to void type, but doesn't care which side is.
                */
                if ((Result->F & TCF_MASK_VOID_PTR) != 0) {
                    Result->C = TC_VOID_PTR;
                } else if (Val == TC_SIGN_DIFF) {
                    /* Special treatment with pointee signedness difference */
                    Result->C = TC_PTR_SIGN_DIFF;
                } else {
                    /* Incompatible */
                    Result->C = TC_PTR_INCOMPATIBLE;
                }
            } else {
                /* Pointer-to-pointer types must have compatible pointte types,
                ** or they are just incompatible.
                */
                Result->C = TC_PTR_INCOMPATIBLE;
            }
        } else {
            Result->C = Val;
        }
        /* printf ("SetResult = %d\n", Val); */
    }
}



static typecmp_t* CmpQuals (const Type* lhst, const Type* rhst, typecmp_t* Result)
/* Compare the types regarding their qualifiers. Return via pointer *Result */
{
    TypeCode LeftQual, RightQual;

    /* Get the left and right qualifiers */
    LeftQual  = GetQualifier (lhst);
    RightQual = GetQualifier (rhst);

    /* If type is function without a calling convention set explicitly,
    ** then assume the default one.
    */
    if (IsTypeFunc (lhst)) {
        if ((LeftQual & T_QUAL_CCONV) == T_QUAL_NONE) {
            LeftQual |= (AutoCDecl || IsVariadicFunc (lhst)) ? T_QUAL_CDECL : T_QUAL_FASTCALL;
        }
    }
    if (IsTypeFunc (rhst)) {
        if ((RightQual & T_QUAL_CCONV) == T_QUAL_NONE) {
            RightQual |= (AutoCDecl || IsVariadicFunc (rhst)) ? T_QUAL_CDECL : T_QUAL_FASTCALL;
        }
    }

    /* Default address size qualifiers */
    if ((LeftQual & T_QUAL_ADDRSIZE) == T_QUAL_NONE) {
        LeftQual |= (IsTypeFunc (lhst) ? CodeAddrSizeQualifier () : DataAddrSizeQualifier ());
    }
    if ((RightQual & T_QUAL_ADDRSIZE) == T_QUAL_NONE) {
        RightQual |= (IsTypeFunc (rhst) ? CodeAddrSizeQualifier () : DataAddrSizeQualifier ());
    }

    /* Just return if nothing to do */
    if (LeftQual == RightQual) {
        return Result;
    }

    /* On the first indirection level, different qualifiers mean that the types
    ** are still compatible. On the second level, that is a (maybe minor) error.
    ** We create a special return-code if a qualifier is dropped from a pointer.
    ** But, different calling conventions are incompatible. Starting from the
    ** next level, the types are incompatible if the qualifiers differ.
    */
    /* (Debugging statement) */
    /* printf ("Ind = %d    %06X != %06X\n", Result->Indirections, LeftQual, RightQual); */
    switch (Result->Indirections) {
        case 0:
            /* Compare C qualifiers */
            if ((LeftQual & T_QUAL_CVR) > (RightQual & T_QUAL_CVR)) {
                Result->F |= TCF_QUAL_IMPLICIT;
            } else if ((LeftQual & T_QUAL_CVR) != (RightQual & T_QUAL_CVR)) {
                Result->F |= TCF_QUAL_DIFF;
            }

            /* Compare address size qualifiers */
            if ((LeftQual & T_QUAL_ADDRSIZE) != (RightQual & T_QUAL_ADDRSIZE)) {
                Result->F |= TCF_ADDRSIZE_QUAL_DIFF;
            }

            /* Compare function calling conventions */
            if ((LeftQual & T_QUAL_CCONV) != (RightQual & T_QUAL_CCONV)) {
                SetResult (Result, TC_INCOMPATIBLE);
            }
            break;

        case 1:
            /* A non-const value on the right is compatible to a
            ** const one to the left, same for volatile.
            */
            if ((LeftQual & T_QUAL_CVR) > (RightQual & T_QUAL_CVR)) {
                Result->F |= TCF_PTR_QUAL_IMPLICIT;
            } else if ((LeftQual & T_QUAL_CVR) != (RightQual & T_QUAL_CVR)) {
                Result->F |= TCF_PTR_QUAL_DIFF;
            }

            /* Compare address size qualifiers */
            if ((LeftQual & T_QUAL_ADDRSIZE) != (RightQual & T_QUAL_ADDRSIZE)) {
                Result->F |= TCF_ADDRSIZE_QUAL_DIFF;
            }

            /* Compare function calling conventions */
            if ((!IsTypeFunc (lhst) && !IsTypeFunc (rhst))  ||
                (LeftQual & T_QUAL_CCONV) == (RightQual & T_QUAL_CCONV)) {
                break;
            }
            /* else fall through */

        default:
            /* Pointer types mismatch */
            SetResult (Result, TC_INCOMPATIBLE);
            break;
    }

    return Result;
}



static void DoCompare (const Type* lhs, const Type* rhs, typecmp_t* Result)
/* Recursively compare two types. */
{
    SymEntry*   Sym1;
    SymEntry*   Sym2;
    FuncDesc*   F1;
    FuncDesc*   F2;
    TypeCode    LeftRank, RightRank;
    long        LeftCount, RightCount;


    /* Compare two types. Determine, where they differ */
    while (lhs->C != T_END && rhs->C != T_END) {
        /* Compare qualifiers */
        if (CmpQuals (lhs, rhs, Result)->C == TC_INCOMPATIBLE) {
            /* No need to compare further */
            return;
        }

        /* Get the ranks of the left and right hands */
        LeftRank  = (GetUnderlyingTypeCode (lhs) & T_MASK_RANK);
        RightRank = (GetUnderlyingTypeCode (rhs) & T_MASK_RANK);

        /* Bit-fields are considered compatible if they have the same
        ** signedness, bit-offset and bit-width.
        */
        if (IsTypeBitField (lhs) || IsTypeBitField (rhs)) {
            if (!IsTypeBitField (lhs)           ||
                !IsTypeBitField (rhs)           ||
                lhs->A.B.Offs  != rhs->A.B.Offs ||
                lhs->A.B.Width != rhs->A.B.Width) {
                /* Incompatible */
                goto Incompatible;
            }
            if (LeftRank != RightRank) {
                SetResult (Result, TC_STRICT_COMPATIBLE);
            }
        }

        /* If one side is a pointer and the other side is an array, both are
        ** compatible.
        */
        if (Result->Indirections == 0) {
            if (LeftRank == T_RANK_PTR && RightRank == T_RANK_ARRAY) {
                RightRank = T_RANK_PTR;
                SetResult (Result, TC_PTR_DECAY);
            }
            if (LeftRank == T_RANK_ARRAY && RightRank == T_RANK_PTR) {
                LeftRank = T_RANK_PTR;
                SetResult (Result, TC_STRICT_COMPATIBLE);
            }
        }

        /* If the ranks are different, the types are incompatible */
        if (LeftRank != RightRank) {
            goto Incompatible;
        }

        /* Enums must be handled specially */
        if ((IsTypeEnum (lhs) || IsTypeEnum (rhs))) {

            /* Compare the tag types */
            Sym1 = GetESUTagSym (lhs);
            Sym2 = GetESUTagSym (rhs);

            /* For the two to be identical, they must be declared in the same
            ** scope and have the same name.
            */
            if (Sym1 != Sym2) {
                if (Sym1 == 0 || Sym2 == 0) {
                    /* Only one is an enum. So they can't be identical */
                    SetResult (Result, TC_STRICT_COMPATIBLE);
                } else if (Sym1->Owner != Sym2->Owner ||
                           strcmp (Sym1->Name, Sym2->Name) != 0) {
                    /* If any one of the two is incomplete, we can't guess
                    ** their underlying types and have to assume that they
                    ** be incompatible.
                    */
                    if (SizeOf (lhs) == 0 || SizeOf (rhs) == 0) {
                        goto Incompatible;
                    }
                    SetResult (Result, TC_STRICT_COMPATIBLE);
                }
            }

        }

        /* 'char' is neither 'signed char' nor 'unsigned char' */
        if ((IsDeclTypeChar (lhs) && !IsDeclTypeChar (rhs)) ||
            (!IsDeclTypeChar (lhs) && IsDeclTypeChar (rhs))) {
            SetResult (Result, TC_SIGN_DIFF);
        }

        /* On indirection level zero, a sign difference is accepted.
        ** The types are no longer equal, but compatible.
        */
        if (GetSignedness (lhs) != GetSignedness (rhs)) {
            SetResult (Result, TC_SIGN_DIFF);
        }

        /* Check for special type elements */
        switch (LeftRank) {
            case T_RANK_PTR:
                ++Result->Indirections;
                if (Result->Indirections == 1) {
                    if ((GetUnderlyingTypeCode (lhs + 1) & T_MASK_RANK) == T_RANK_VOID) {
                        Result->F |= TCF_VOID_PTR_ON_LEFT;
                    }
                    if ((GetUnderlyingTypeCode (rhs + 1) & T_MASK_RANK) == T_RANK_VOID) {
                        Result->F |= TCF_VOID_PTR_ON_RIGHT;
                    }
                } else {
                    Result->F &= ~TCF_MASK_VOID_PTR;
                }
                break;

            case T_RANK_FUNC:
                /* Compare the function descriptors */
                F1 = GetFuncDesc (lhs);
                F2 = GetFuncDesc (rhs);

                /* If one of both function declarations has an empty parameter
                ** list (which does also mean, it is not a function definition,
                ** because the flag is reset in this case), it is ignored for
                ** parameter comparison and considered equal to the other one,
                ** provided both have the same return type and other attributes.
                ** If neither of both parameter lists is empty, we have to check
                ** the parameter lists.
                */
                if ((F1->Flags & FD_EMPTY) == 0 &&
                    (F2->Flags & FD_EMPTY) == 0) {

                    /* Check the remaining flags */
                    if ((F1->Flags & ~FD_IGNORE) != (F2->Flags & ~FD_IGNORE)) {
                        /* Flags differ */
                        goto Incompatible;
                    }

                    /* Compare the parameter lists */
                    if (EqualFuncParams (F1, F2) == 0) {
                        /* Parameter list is not identical */
                        goto Incompatible;
                    }
                }

                /* Keep on and compare the return type */
                break;

            case T_RANK_ARRAY:
                /* Check member count */
                LeftCount  = GetElementCount (lhs);
                RightCount = GetElementCount (rhs);
                if (LeftCount != RightCount) {
                    if (LeftCount  != UNSPECIFIED &&
                        RightCount != UNSPECIFIED) {
                        /* Member count given but different */
                        goto Incompatible;
                    }

                    /* We take into account which side is more specified */
                    if (LeftCount == UNSPECIFIED) {
                        SetResult (Result, TC_UNSPECIFY);
                    } else {
                        SetResult (Result, TC_EQUAL);
                    }
                }
                break;

            case T_RANK_STRUCT:
            case T_RANK_UNION:
                /* Compare the tag types */
                Sym1 = GetESUTagSym (lhs);
                Sym2 = GetESUTagSym (rhs);

                CHECK (Sym1 != 0 || Sym2 != 0);

                if (Sym1 != Sym2) {
                    /* Both must be in the same scope and have the same name to
                    ** be identical.
                    */
                    if (Sym1->Owner != Sym2->Owner ||
                        strcmp (Sym1->Name, Sym2->Name) != 0) {
                        /* This shouldn't happen in the current code base, but
                        ** we still handle this case to be future-proof.
                        */
                        goto Incompatible;
                    }
                }

                /* Both are identical */
                break;
        }

        /* Next type string element */
        ++lhs;
        ++rhs;
    }

    /* Check if lhs and rhs both reached ends */
    if (lhs->C == T_END && rhs->C == T_END) {
        SetResult (Result, TC_IDENTICAL);
        return;
    }

Incompatible:
    SetResult (Result, TC_INCOMPATIBLE);
}



typecmp_t TypeCmp (const Type* lhs, const Type* rhs)
/* Compare two types and return the result */
{
    /* Assume the types are identical */
    typecmp_t Result = TYPECMP_INITIALIZER;

#if 0
    printf ("Left : "); PrintRawType (stdout, lhs);
    printf ("Right: "); PrintRawType (stdout, rhs);
#endif

    /* Recursively compare the types if they aren't identical */
    if (rhs != lhs) {
        DoCompare (lhs, rhs, &Result);
    }

    /* Return the result */
    return Result;
}



void TypeCompatibilityDiagnostic (const Type* NewType, const Type* OldType, int IsError, const char* Msg)
/* Print error or warning message about type compatibility with proper type
** names. The format string shall contain two '%s' specifiers for the names of
** the two types.
*/
{
    StrBuf NewTypeName = STATIC_STRBUF_INITIALIZER;
    StrBuf OldTypeName = STATIC_STRBUF_INITIALIZER;
    GetFullTypeNameBuf (&NewTypeName, NewType);
    GetFullTypeNameBuf (&OldTypeName, OldType);
    if (IsError) {
        Error (Msg, SB_GetConstBuf (&NewTypeName), SB_GetConstBuf (&OldTypeName));
    } else {
        Warning (Msg, SB_GetConstBuf (&NewTypeName), SB_GetConstBuf (&OldTypeName));
    }
    SB_Done (&OldTypeName);
    SB_Done (&NewTypeName);
}
