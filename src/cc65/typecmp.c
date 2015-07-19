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
#include "funcdesc.h"
#include "global.h"
#include "symtab.h"
#include "typecmp.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static void SetResult (typecmp_t* Result, typecmp_t Val)
/* Set a new result value if it is less than the existing one */
{
    if (Val < *Result) {
        /* printf ("SetResult = %d\n", Val); */
        *Result = Val;
    }
}



static int ParamsHaveDefaultPromotions (const FuncDesc* F)
/* Check if any of the parameters of function F has a default promotion. In
** this case, the function is not compatible with an empty parameter name list
** declaration.
*/
{
    /* Get the symbol table */
    const SymTable* Tab = F->SymTab;

    /* Get the first parameter in the list */
    const SymEntry* Sym = Tab->SymHead;

    /* Walk over all parameters */
    while (Sym && (Sym->Flags & SC_PARAM)) {

        /* If this is an integer type, check if the promoted type is equal
        ** to the original type. If not, we have a default promotion.
        */
        if (IsClassInt (Sym->Type)) {
            if (IntPromotion (Sym->Type) != Sym->Type) {
                return 1;
            }
        }

        /* Get the pointer to the next param */
        Sym = Sym->NextSym;
    }

    /* No default promotions in the parameter list */
    return 0;
}



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
        Type* Type1 = Sym1->Type;
        Type* Type2 = Sym2->Type;

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

        /* Compare this field */
        if (TypeCmp (Type1, Type2) < TC_EQUAL) {
            /* Field types not equal */
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



static int EqualSymTables (SymTable* Tab1, SymTable* Tab2)
/* Compare two symbol tables. Return 1 if they are equal and 0 otherwise */
{
    /* Compare the parameter lists */
    SymEntry* Sym1 = Tab1->SymHead;
    SymEntry* Sym2 = Tab2->SymHead;

    /* Compare the fields */
    while (Sym1 && Sym2) {

        /* Compare the names of this field */
        if (!HasAnonName (Sym1) || !HasAnonName (Sym2)) {
            if (strcmp (Sym1->Name, Sym2->Name) != 0) {
                /* Names are not identical */
                return 0;
            }
        }

        /* Compare the types of this field */
        if (TypeCmp (Sym1->Type, Sym2->Type) < TC_EQUAL) {
            /* Field types not equal */
            return 0;
        }

        /* Get the pointers to the next fields */
        Sym1 = Sym1->NextSym;
        Sym2 = Sym2->NextSym;
    }

    /* Check both pointers against NULL to compare the field count */
    return (Sym1 == 0 && Sym2 == 0);
}



static void DoCompare (const Type* lhs, const Type* rhs, typecmp_t* Result)
/* Recursively compare two types. */
{
    unsigned    Indirections;
    unsigned    ElementCount;
    SymEntry*   Sym1;
    SymEntry*   Sym2;
    SymTable*   Tab1;
    SymTable*   Tab2;
    FuncDesc*   F1;
    FuncDesc*   F2;


    /* Initialize stuff */
    Indirections = 0;
    ElementCount = 0;

    /* Compare two types. Determine, where they differ */
    while (lhs->C != T_END) {

        TypeCode LeftType, RightType;
        TypeCode LeftSign, RightSign;
        TypeCode LeftQual, RightQual;
        long LeftCount, RightCount;

        /* Check if the end of the type string is reached */
        if (rhs->C == T_END) {
            /* End of comparison reached */
            return;
        }

        /* Get the raw left and right types, signs and qualifiers */
        LeftType  = GetType (lhs);
        RightType = GetType (rhs);
        LeftSign  = GetSignedness (lhs);
        RightSign = GetSignedness (rhs);
        LeftQual  = GetQualifier (lhs);
        RightQual = GetQualifier (rhs);

        /* If the left type is a pointer and the right is an array, both
        ** are compatible.
        */
        if (LeftType == T_TYPE_PTR && RightType == T_TYPE_ARRAY) {
            RightType = T_TYPE_PTR;
        }

        /* If the raw types are not identical, the types are incompatible */
        if (LeftType != RightType) {
            SetResult (Result, TC_INCOMPATIBLE);
            return;
        }

        /* On indirection level zero, a qualifier or sign difference is
        ** accepted. The types are no longer equal, but compatible.
        */
        if (LeftSign != RightSign) {
            if (ElementCount == 0) {
                SetResult (Result, TC_SIGN_DIFF);
            } else {
                SetResult (Result, TC_INCOMPATIBLE);
                return;
            }
        }

        if (LeftType == T_TYPE_FUNC) {
            /* If a calling convention wasn't set explicitly,
            ** then assume the default one.
            */
            if ((LeftQual & T_QUAL_CCONV) == T_QUAL_NONE) {
                LeftQual |= (AutoCDecl || IsVariadicFunc (lhs)) ? T_QUAL_CDECL : T_QUAL_FASTCALL;
            }
            if ((RightQual & T_QUAL_CCONV) == T_QUAL_NONE) {
                RightQual |= (AutoCDecl || IsVariadicFunc (rhs)) ? T_QUAL_CDECL : T_QUAL_FASTCALL;
            }
        }

        if (LeftQual != RightQual) {
            /* On the first indirection level, different qualifiers mean
            ** that the types still are compatible. On the second level,
            ** that is a (maybe minor) error. We create a special return-code
            ** if a qualifier is dropped from a pointer. But, different calling
            ** conventions are incompatible. Starting from the next level,
            ** the types are incompatible if the qualifiers differ.
            */
            /* (Debugging statement) */
            /* printf ("Ind = %d    %06X != %06X\n", Indirections, LeftQual, RightQual); */
            switch (Indirections) {
                case 0:
                    SetResult (Result, TC_STRICT_COMPATIBLE);
                    break;

                case 1:
                    /* A non-const value on the right is compatible to a
                    ** const one to the left, same for volatile.
                    */
                    if ((LeftQual & T_QUAL_CONST) < (RightQual & T_QUAL_CONST) ||
                        (LeftQual & T_QUAL_VOLATILE) < (RightQual & T_QUAL_VOLATILE)) {
                        SetResult (Result, TC_QUAL_DIFF);
                    } else {
                        SetResult (Result, TC_STRICT_COMPATIBLE);
                    }

                    if (LeftType != T_TYPE_FUNC || (LeftQual & T_QUAL_CCONV) == (RightQual & T_QUAL_CCONV)) {
                        break;
                    }
                    /* else fall through */

                default:
                    SetResult (Result, TC_INCOMPATIBLE);
                    return;
            }
        }

        /* Check for special type elements */
        switch (LeftType) {
            case T_TYPE_PTR:
                ++Indirections;
                break;

            case T_TYPE_FUNC:
                /* Compare the function descriptors */
                F1 = GetFuncDesc (lhs);
                F2 = GetFuncDesc (rhs);

                /* If one of both functions has an empty parameter list (which
                ** does also mean, it is not a function definition, because the
                ** flag is reset in this case), it is considered equal to any
                ** other definition, provided that the other has no default
                ** promotions in the parameter list. If none of both parameter
                ** lists is empty, we have to check the parameter lists and
                ** other attributes.
                */
                if (F1->Flags & FD_EMPTY) {
                    if ((F2->Flags & FD_EMPTY) == 0) {
                        if (ParamsHaveDefaultPromotions (F2)) {
                            /* Flags differ */
                            SetResult (Result, TC_INCOMPATIBLE);
                            return;
                        }
                    }
                } else if (F2->Flags & FD_EMPTY) {
                    if (ParamsHaveDefaultPromotions (F1)) {
                        /* Flags differ */
                        SetResult (Result, TC_INCOMPATIBLE);
                        return;
                    }
                } else {

                    /* Check the remaining flags */
                    if ((F1->Flags & ~FD_IGNORE) != (F2->Flags & ~FD_IGNORE)) {
                        /* Flags differ */
                        SetResult (Result, TC_INCOMPATIBLE);
                        return;
                    }

                    /* Compare the parameter lists */
                    if (EqualFuncParams (F1, F2) == 0) {
                        /* Parameter list is not identical */
                        SetResult (Result, TC_INCOMPATIBLE);
                        return;
                    }
                }

                /* Keep on and compare the return type */
                break;

            case T_TYPE_ARRAY:
                /* Check member count */
                LeftCount  = GetElementCount (lhs);
                RightCount = GetElementCount (rhs);
                if (LeftCount  != UNSPECIFIED &&
                    RightCount != UNSPECIFIED &&
                    LeftCount  != RightCount) {
                    /* Member count given but different */
                    SetResult (Result, TC_INCOMPATIBLE);
                    return;
                }
                break;

            case T_TYPE_STRUCT:
            case T_TYPE_UNION:
                /* Compare the fields recursively. To do that, we fetch the
                ** pointer to the struct definition from the type, and compare
                ** the fields.
                */
                Sym1 = GetSymEntry (lhs);
                Sym2 = GetSymEntry (rhs);

                /* If one symbol has a name, the names must be identical */
                if (!HasAnonName (Sym1) || !HasAnonName (Sym2)) {
                    if (strcmp (Sym1->Name, Sym2->Name) != 0) {
                        /* Names are not identical */
                        SetResult (Result, TC_INCOMPATIBLE);
                        return;
                    }
                }

                /* Get the field tables from the struct entry */
                Tab1 = Sym1->V.S.SymTab;
                Tab2 = Sym2->V.S.SymTab;

                /* One or both structs may be forward definitions. In this case,
                ** the symbol tables are both non existant. Assume that the
                ** structs are equal in this case.
                */
                if (Tab1 != 0 && Tab2 != 0) {

                    if (EqualSymTables (Tab1, Tab2) == 0) {
                        /* Field lists are not equal */
                        SetResult (Result, TC_INCOMPATIBLE);
                        return;
                    }

                }

                /* Structs are equal */
                break;
        }

        /* Next type string element */
        ++lhs;
        ++rhs;
        ++ElementCount;
    }

    /* Check if end of rhs reached */
    if (rhs->C == T_END) {
        SetResult (Result, TC_EQUAL);
    } else {
        SetResult (Result, TC_INCOMPATIBLE);
    }
}



typecmp_t TypeCmp (const Type* lhs, const Type* rhs)
/* Compare two types and return the result */
{
    /* Assume the types are identical */
    typecmp_t   Result = TC_IDENTICAL;

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
