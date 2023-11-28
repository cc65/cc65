/*****************************************************************************/
/*                                                                           */
/*                                 declare.c                                 */
/*                                                                           */
/*                 Parse variable and function declarations                  */
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
#include "declare.h"
#include "declattr.h"
#include "error.h"
#include "expr.h"
#include "funcdesc.h"
#include "function.h"
#include "global.h"
#include "litpool.h"
#include "pragma.h"
#include "scanner.h"
#include "standard.h"
#include "staticassert.h"
#include "symtab.h"
#include "wrappedcall.h"
#include "typeconv.h"



/*****************************************************************************/
/*                                 Forwards                                  */
/*****************************************************************************/



static void ParseTypeSpec (DeclSpec* Spec, typespec_t TSFlags, int* SignednessSpecified);
/* Parse a type specifier */



/*****************************************************************************/
/*                            Internal functions                             */
/*****************************************************************************/



static unsigned ParseOneStorageClass (void)
/* Parse and return a storage class specifier */
{
    unsigned StorageClass = 0;

    /* Check the storage class given */
    switch (CurTok.Tok) {

        case TOK_EXTERN:
            StorageClass = SC_EXTERN | SC_STATIC;
            NextToken ();
            break;

        case TOK_STATIC:
            StorageClass = SC_STATIC;
            NextToken ();
            break;

        case TOK_REGISTER:
            StorageClass = SC_REGISTER | SC_STATIC;
            NextToken ();
            break;

        case TOK_AUTO:
            StorageClass = SC_AUTO;
            NextToken ();
            break;

        case TOK_TYPEDEF:
            StorageClass = SC_TYPEDEF;
            NextToken ();
            break;

        default:
            break;
    }

    return StorageClass;
}



static int ParseStorageClass (DeclSpec* Spec)
/* Parse storage class specifiers. Return true if a specifier is read even if
** it was duplicated or disallowed. */
{
    /* Check the storage class given */
    unsigned StorageClass = ParseOneStorageClass ();

    if (StorageClass == 0) {
        return 0;
    }

    while (StorageClass != 0) {
        if (Spec->StorageClass == 0) {
            Spec->StorageClass = StorageClass;
        } else if (Spec->StorageClass == StorageClass) {
            Warning ("Duplicate storage class specifier");
        } else {
            Error ("Conflicting storage class specifier");
        }
        StorageClass = ParseOneStorageClass ();
    }

    return 1;
}



static void DuplicateQualifier (const char* Name)
/* Print an error message */
{
    Warning ("Duplicate qualifier: '%s'", Name);
}



static TypeCode OptionalQualifiers (TypeCode Qualifiers, TypeCode Allowed)
/* Read type qualifiers if we have any. Allowed specifies the allowed
** qualifiers. Return any read qualifiers even if they caused errors.
*/
{
    /* We start without any qualifiers */
    TypeCode Q = T_QUAL_NONE;

    /* Check for more qualifiers */
    while (1) {

        switch (CurTok.Tok) {

            case TOK_CONST:
                if (Allowed & T_QUAL_CONST) {
                    if (Qualifiers & T_QUAL_CONST) {
                        DuplicateQualifier ("const");
                    }
                    Q |= T_QUAL_CONST;
                } else {
                    goto Done;
                }
                break;

            case TOK_VOLATILE:
                if (Allowed & T_QUAL_VOLATILE) {
                    if (Qualifiers & T_QUAL_VOLATILE) {
                        DuplicateQualifier ("volatile");
                    }
                    Q |= T_QUAL_VOLATILE;
                } else {
                    goto Done;
                }
                break;

            case TOK_RESTRICT:
                if (Allowed & T_QUAL_RESTRICT) {
                    if (Qualifiers & T_QUAL_RESTRICT) {
                        DuplicateQualifier ("restrict");
                    }
                    Q |= T_QUAL_RESTRICT;
                } else {
                    goto Done;
                }
                break;

            case TOK_NEAR:
                if (Allowed & T_QUAL_NEAR) {
                    if (Qualifiers & T_QUAL_NEAR) {
                        DuplicateQualifier ("near");
                    }
                    Q |= T_QUAL_NEAR;
                } else {
                    goto Done;
                }
                break;

            case TOK_FAR:
                if (Allowed & T_QUAL_FAR) {
                    if (Qualifiers & T_QUAL_FAR) {
                        DuplicateQualifier ("far");
                    }
                    Q |= T_QUAL_FAR;
                } else {
                    goto Done;
                }
                break;

            case TOK_FASTCALL:
                if (Allowed & T_QUAL_FASTCALL) {
                    if (Qualifiers & T_QUAL_FASTCALL) {
                        DuplicateQualifier ("fastcall");
                    }
                    Q |= T_QUAL_FASTCALL;
                } else {
                    goto Done;
                }
                break;

            case TOK_CDECL:
                if (Allowed & T_QUAL_CDECL) {
                    if (Qualifiers & T_QUAL_CDECL) {
                        DuplicateQualifier ("cdecl");
                    }
                    Q |= T_QUAL_CDECL;
                } else {
                    goto Done;
                }
                break;

            default:
                goto Done;

        }

        /* Combine with newly read qualifiers */
        Qualifiers |= Q;

        /* Skip the token */
        NextToken ();
    }

Done:
    /* We cannot have more than one address size far qualifier */
    switch (Qualifiers & T_QUAL_ADDRSIZE) {

        case T_QUAL_NONE:
        case T_QUAL_NEAR:
        case T_QUAL_FAR:
            break;

        default:
            Error ("Cannot specify more than one address size qualifier");
            Qualifiers &= ~T_QUAL_ADDRSIZE;
    }

    /* We cannot have more than one calling convention specifier */
    switch (Qualifiers & T_QUAL_CCONV) {

        case T_QUAL_NONE:
        case T_QUAL_FASTCALL:
        case T_QUAL_CDECL:
            break;

        default:
            Error ("Cannot specify more than one calling convention qualifier");
            Qualifiers &= ~T_QUAL_CCONV;
    }

    /* Return any qualifiers just read */
    return Q;
}



static void OptionalSpecifiers (DeclSpec* Spec, TypeCode* Qualifiers, typespec_t TSFlags)
/* Read storage specifiers and/or type qualifiers if we have any. Storage class
** specifiers require the corresponding typespec_t flag set to be allowed, and
** only const and volatile type qualifiers are allowed under any circumstance.
** Read storage class specifiers are output in *Spec and type qualifiers are
** output in *Qualifiers with error checking.
*/
{
    TypeCode Q = T_QUAL_NONE;
    int Continue;

    do {
        /* There may be type qualifiers *before* any storage class specifiers */
        Q = OptionalQualifiers (*Qualifiers, T_QUAL_CONST | T_QUAL_VOLATILE);
        *Qualifiers |= Q;

        /* Parse storage class specifiers anyway then check */
        Continue = ParseStorageClass (Spec);
        if (Continue && (TSFlags & (TS_STORAGE_CLASS_SPEC | TS_FUNCTION_SPEC)) == 0) {
            Error ("Unexpected storage class specified");
        }
    } while (Continue || Q != T_QUAL_NONE);
}



static void OptionalInt (void)
/* Eat an optional "int" token */
{
    if (CurTok.Tok == TOK_INT) {
        /* Skip it */
        NextToken ();
    }
}



static void OptionalSigned (int* SignednessSpecified)
/* Eat an optional "signed" token */
{
    if (CurTok.Tok == TOK_SIGNED) {
        /* Skip it */
        NextToken ();
        if (SignednessSpecified != NULL) {
            *SignednessSpecified = 1;
        }
    }
}



static void InitDeclSpec (DeclSpec* Spec)
/* Initialize the DeclSpec struct for use */
{
    Spec->StorageClass  = 0;
    Spec->Type[0].C     = T_END;
    Spec->Flags         = 0;
}



static void InitDeclarator (Declarator* D)
/* Initialize the Declarator struct for use */
{
    D->Ident[0]   = '\0';
    D->Type[0].C  = T_END;
    D->Index      = 0;
    D->Attributes = 0;
}



static void NeedTypeSpace (Declarator* D, unsigned Count)
/* Check if there is enough space for Count type specifiers within D */
{
    if (D->Index + Count >= MAXTYPELEN) {
        /* We must call Fatal() here, since calling Error() will try to
        ** continue, and the declaration type is not correctly terminated
        ** in case we come here.
        */
        Fatal ("Too many type specifiers");
    }
}



static void AddTypeCodeToDeclarator (Declarator* D, TypeCode T)
/* Add a type specifier to the type of a declarator */
{
    NeedTypeSpace (D, 1);
    D->Type[D->Index++].C = T;
}



static void FixQualifiers (Type* DataType)
/* Apply several fixes to qualifiers */
{
    Type*    T;
    TypeCode Q;

    /* Using typedefs, it is possible to generate declarations that have
    ** type qualifiers attached to an array, not the element type. Go and
    ** fix these here.
    */
    T = DataType;
    Q = T_QUAL_NONE;
    while (T->C != T_END) {
        if (IsTypeArray (T)) {
            /* Extract any type qualifiers */
            Q |= GetQualifier (T);
            T->C = GetUnqualRawTypeCode (T);
        } else {
            /* Add extracted type qualifiers here */
            T->C |= Q;
            Q = T_QUAL_NONE;
        }
        ++T;
    }
    /* Q must be empty now */
    CHECK (Q == T_QUAL_NONE);

    /* Do some fixes on pointers and functions. */
    T = DataType;
    while (T->C != T_END) {
        if (IsTypePtr (T)) {
            /* Calling convention qualifier on the pointer? */
            if (IsQualCConv (T)) {
                /* Pull the convention off of the pointer */
                Q = T[0].C & T_QUAL_CCONV;
                T[0].C &= ~T_QUAL_CCONV;

                /* Pointer to a function which doesn't have an explicit convention? */
                if (IsTypeFunc (T + 1)) {
                    if (IsQualCConv (T + 1)) {
                        if ((T[1].C & T_QUAL_CCONV) == Q) {
                            Warning ("Pointer duplicates function's calling convention");
                        } else {
                            Error ("Function's and pointer's calling conventions are different");
                        }
                    } else {
                        if (Q == T_QUAL_FASTCALL && IsVariadicFunc (T + 1)) {
                            Error ("Variadic-function pointers cannot be __fastcall__");
                        } else {
                            /* Move the qualifier from the pointer to the function. */
                            T[1].C |= Q;
                        }
                    }
                } else {
                    Error ("Not pointer to a function; can't use a calling convention");
                }
            }

            /* Apply the default far and near qualifiers if none are given */
            Q = (T[0].C & T_QUAL_ADDRSIZE);
            if (Q == T_QUAL_NONE) {
                /* No address size qualifiers specified */
                if (IsTypeFunc (T+1)) {
                    /* Pointer to function. Use the qualifier from the function,
                    ** or the default if the function doesn't have one.
                    */
                    Q = (T[1].C & T_QUAL_ADDRSIZE);
                    if (Q == T_QUAL_NONE) {
                        Q = CodeAddrSizeQualifier ();
                    }
                } else {
                    Q = DataAddrSizeQualifier ();
                }
                T[0].C |= Q;
            } else {
                /* We have address size qualifiers. If followed by a function,
                ** apply them to the function also.
                */
                if (IsTypeFunc (T+1)) {
                    TypeCode FQ = (T[1].C & T_QUAL_ADDRSIZE);
                    if (FQ == T_QUAL_NONE) {
                        T[1].C |= Q;
                    } else if (FQ != Q) {
                        Error ("Address size qualifier mismatch");
                        T[1].C = (T[1].C & ~T_QUAL_ADDRSIZE) | Q;
                    }
                }
            }

        } else if (IsTypeFunc (T)) {

            /* Apply the default far and near qualifiers if none are given */
            if ((T[0].C & T_QUAL_ADDRSIZE) == 0) {
                T[0].C |= CodeAddrSizeQualifier ();
            }

        }
        ++T;
    }
}



static void FixFunctionReturnType (Type* T)
/* Check if the data type consists of any functions returning forbidden return
** types and remove qualifiers from the return types if they are not void.
*/
{
    while (T->C != T_END) {
        if (IsTypeFunc (T)) {
            ++T;

            /* Functions may not return functions or arrays */
            if (IsTypeFunc (T)) {
                Error ("Functions are not allowed to return functions");
            } else if (IsTypeArray (T)) {
                Error ("Functions are not allowed to return arrays");
            }

            /* The return type must not be qualified */
            if ((GetQualifier (T) & T_QUAL_CVR) != T_QUAL_NONE) {
                /* We are stricter than the standard here */
                if (GetRawTypeRank (T) == T_RANK_VOID) {
                    /* A qualified void type is always an error */
                    Error ("Function definition has qualified void return type");
                } else {
                    /* For others, qualifiers are ignored */
                    Warning ("Type qualifiers ignored on function return type");
                    T[0].C &= ~T_QUAL_CVR;
                }
            }
        } else {
            ++T;
        }
    }
}



static void CheckArrayElementType (const Type* T)
/* Check recursively if type consists of arrays of forbidden element types */
{
    while (T->C != T_END) {
        if (IsTypeArray (T)) {
            /* If the array is multi-dimensional, keep going until we get the
            ** true element type.
            */
            ++T;
            if (SizeOf (T) == 0) {
                if (IsTypeArray (T) || IsIncompleteESUType (T)) {
                    /* We cannot have an array of incomplete elements */
                    if (!IsTypeArray (T) || GetElementCount (T) == UNSPECIFIED) {
                        Error ("Array of incomplete element type '%s'",
                               GetFullTypeName (T));
                        return;
                    }
                } else if (!IsTypeVoid (T) || IS_Get (&Standard) != STD_CC65) {
                    /* We could support certain 0-size element types as an extension */
                    Error ("Array of 0-size element type '%s'",
                           GetFullTypeName (T));
                    return;
                }
            } else {
                /* Elements cannot contain flexible array members themselves */
                if (IsClassStruct (T)) {
                    SymEntry* TagEntry = GetESUTagSym (T);
                    if (TagEntry && SymHasFlexibleArrayMember (TagEntry)) {
                        Error ("Invalid use of struct with flexible array member");
                        return;
                    }
                }
            }
        } else {
            ++T;
        }
    }
}



static SymEntry* ForwardESU (const char* Name, unsigned Flags, unsigned* DSFlags)
/* Handle an enum, struct or union forward declaration */
{
    /* Try to find an enum/struct/union with the given name. If there is none,
    ** insert a forward declaration into the current lexical level.
    */
    SymEntry* TagEntry = FindTagSym (Name);
    if (TagEntry == 0) {
        if ((Flags & SC_ESUTYPEMASK) != SC_ENUM) {
            TagEntry = AddStructSym (Name, Flags, 0, 0, DSFlags);
        } else {
            TagEntry = AddEnumSym (Name, Flags, 0, 0, DSFlags);
        }
    } else if ((TagEntry->Flags & SC_TYPEMASK) != (Flags & SC_ESUTYPEMASK)) {
        /* Already defined, but not the same type class */
        Error ("Symbol '%s' is already different kind", Name);
    }
    return TagEntry;
}



static const Type* GetEnumeratorType (long Min, unsigned long Max, int Signed)
/* GitHub #1093 - We use unsigned types to save spaces whenever possible.
** If both the signed and unsigned integer types of the same minimum size
** capable of representing all values of the enum, we prefer the unsigned
** one.
** Return 0 if impossible to represent Min and Max as the same integer type.
*/
{
    const Type* Underlying = type_int;      /* default type */

    /* Change the underlying type if necessary */
    if (Min < 0 || Signed) {
        /* We can't use unsigned types if there are any negative values */
        if (Max > (unsigned long)INT32_MAX) {
            /* No way to represent both Min and Max as the same integer type */
            Underlying = 0;
        } else if (Min < INT16_MIN || Max > (unsigned long)INT16_MAX) {
            Underlying = type_long;
        } else if (Min < INT8_MIN || Max > (unsigned long)INT8_MAX) {
            Underlying = type_int;
        } else {
            Underlying = type_schar;
        }
    } else {
        if (Max > UINT16_MAX) {
            Underlying = type_ulong;
        } else if (Max > UINT8_MAX) {
            Underlying = type_uint;
        } else {
            Underlying = type_uchar;
        }
    }

    return Underlying;
}



static SymEntry* ParseEnumSpec (const char* Name, unsigned* DSFlags)
/* Process an enum specifier */
{
    SymTable*       FieldTab;
    long            EnumVal;
    int             IsSigned;
    int             IsIncremented;
    ident           Ident;
    long            MinConstant = 0;
    unsigned long   MaxConstant = 0;
    const Type*     NewType     = 0;        /* new member type */
    const Type*     MemberType  = type_int; /* default member type */
    unsigned        Flags       = 0;
    unsigned        PrevErrorCount = ErrorCount;


    if (CurTok.Tok != TOK_LCURLY) {
        /* Just a forward definition */
        return ForwardESU (Name, SC_ENUM, DSFlags);
    }

    /* Add a forward declaration for the enum tag in the current lexical level */
    AddEnumSym (Name, 0, 0, 0, DSFlags);

    /* Skip the opening curly brace */
    NextToken ();

    /* Read the enum tags */
    EnumVal = -1L;
    while (CurTok.Tok != TOK_RCURLY) {

        /* We expect an identifier */
        if (CurTok.Tok != TOK_IDENT) {
            Error ("Identifier expected for enumerator declarator");
            /* Avoid excessive errors */
            NextToken ();
            continue;
        }

        /* Remember the identifier and skip it */
        strcpy (Ident, CurTok.Ident);
        NextToken ();

        /* Check for an assigned value */
        if (CurTok.Tok == TOK_ASSIGN) {

            NextToken ();
            ExprDesc Expr = NoCodeConstAbsIntExpr (hie1);
            EnumVal       = Expr.IVal;
            MemberType    = Expr.Type;
            IsSigned      = IsSignSigned (MemberType);
            IsIncremented = 0;

        } else {

            /* Defaulted with the same signedness as the previous member's */
            IsSigned = IsSignSigned (MemberType) &&
                       (unsigned long)EnumVal != GetIntegerTypeMax (MemberType);

            /* Enumerate by adding one to the previous value */
            EnumVal = (long)(((unsigned long)EnumVal + 1UL) & 0xFFFFFFFFUL);

            if (GetUnqualRawTypeCode (MemberType) == T_ULONG && EnumVal == 0) {
                /* Error since the new value cannot be represented in the
                ** largest unsigned integer type supported by cc65 for enum.
                */
                Error ("Enumerator '%s' overflows the range of '%s'",
                       Ident,
                       GetBasicTypeName (type_ulong));
            }

            IsIncremented = 1;
        }

        /* Track down the min/max values and evaluate the type of EnumVal
        ** using GetEnumeratorType in a tricky way.
        */
        if (!IsSigned || EnumVal >= 0) {
            if ((unsigned long)EnumVal > MaxConstant) {
                MaxConstant = (unsigned long)EnumVal;
            }
            NewType = GetEnumeratorType (0, EnumVal, IsSigned);
        } else {
            if (EnumVal < MinConstant) {
                MinConstant = EnumVal;
            }
            NewType = GetEnumeratorType (EnumVal, 0, 1);
        }

        /* GetEnumeratorType above should never fail, but just in case */
        if (NewType == 0) {
            Internal ("Unexpected failure with GetEnumeratorType: %lx", EnumVal);
            NewType = type_ulong;
        } else if (SizeOf (NewType) < SizeOf (type_int)) {
            /* Integer constants are not shorter than int */
            NewType = type_int;
        }

        /* Warn if the incremented value exceeds the range of the previous
        ** type.
        */
        if (PrevErrorCount == ErrorCount    &&
            IsIncremented                   &&
            (!IsSigned || EnumVal >= 0)     &&
            NewType->C != GetUnqualRawTypeCode (MemberType)) {
            /* The possible overflow here can only be when EnumVal > 0 */
            Warning ("Enumerator '%s' (value = %lu) implies type '%s'",
                     Ident,
                     (unsigned long)EnumVal,
                     GetBasicTypeName (NewType));
        }

        /* Warn if the value exceeds range of 'int' in standard mode */
        if (IS_Get (&Standard) != STD_CC65 && NewType->C != T_INT) {
            if (!IsSigned || EnumVal >= 0) {
                Warning ("ISO C restricts enumerator values to range of 'int'\n"
                         "\tEnumerator '%s' (value = %lu) is too large",
                         Ident,
                         (unsigned long)EnumVal);
            } else {
                Warning ("ISO C restricts enumerator values to range of 'int'\n"
                         "\tEnumerator '%s' (value = %ld) is too small",
                         Ident,
                         EnumVal);
            }
        }

        /* Add an entry of the enumerator to the symbol table */
        AddConstSym (Ident, NewType, SC_ENUMERATOR | SC_CONST, EnumVal);

        /* Use this type for following members */
        MemberType = NewType;

        /* Check for end of definition */
        if (CurTok.Tok != TOK_COMMA) {
            break;
        }
        NextToken ();
    }
    ConsumeRCurly ();

    /* Check if there have been any members. Error if none */
    if (NewType == 0) {
        Error ("Empty enum is invalid");
    }

    /* This evaluates the underlying type of the whole enum */
    MemberType = GetEnumeratorType (MinConstant, MaxConstant, 0);
    if (MemberType == 0) {
        /* It is very likely that the program is wrong */
        Error ("Enumeration values cannot be represented all as 'long'\n"
               "\tMin enumerator value = %ld, Max enumerator value = %lu",
               MinConstant, MaxConstant);

        /* Avoid more errors */
        MemberType = type_long;
    }

    FieldTab = GetSymTab ();

    /* Return a fictitious symbol if errors occurred during parsing */
    if (PrevErrorCount != ErrorCount) {
        Flags |= SC_FICTITIOUS;
    }

    return AddEnumSym (Name, SC_DEF | Flags, MemberType, FieldTab, DSFlags);
}



static int ParseFieldWidth (Declarator* D)
/* Parse an optional field width. Returns -1 if no field width is specified,
** otherwise the width of the field.
*/
{
    if (CurTok.Tok != TOK_COLON) {
        /* No bit-field declaration */
        return -1;
    }

    if (!IsClassInt (D->Type)) {
        /* Only integer types may be used for bit-fields */
        Error ("Bit-field has invalid type '%s', must be integral",
               GetBasicTypeName (D->Type));

        /* Avoid a diagnostic storm by giving the bit-field the widest valid
        ** signed type, and continuing to parse.
        */
        D->Type[0].C = T_INT;
    }

    /* We currently support integral types up to long */
    if (SizeOf (D->Type) > SizeOf (type_ulong)) {
        /* Only long-sized or smaller types may be used for bit-fields, for now */
        Error ("cc65 currently supports only long-sized and smaller bit-field types");

        /* Avoid a diagnostic storm */
        D->Type[0].C = T_INT;
    }

    /* Read the width */
    NextToken ();
    ExprDesc Expr = NoCodeConstAbsIntExpr (hie1);

    if (Expr.IVal < 0) {
        Error ("Negative width in bit-field");
        return -1;
    }
    if (Expr.IVal > (long)(SizeOf (D->Type) * CHAR_BITS)) {
        Error ("Width of bit-field exceeds its type");
        return -1;
    }
    if (Expr.IVal == 0 && D->Ident[0] != '\0') {
        Error ("Zero width for named bit-field");
        return -1;
    }

    /* Return the field width */
    return (int) Expr.IVal;
}



static unsigned PadWithBitField (unsigned StructSize, unsigned BitOffs)
/* Pad the current struct with an anonymous bit-field aligned to the next byte.
** Return how many bits are used to pad.
*/
{
    /* MSVC complains about unary negation of unsigned,
    ** so it has been rewritten as subtraction.
    */
    unsigned PaddingBits = (0 - BitOffs) % CHAR_BITS;

    /* We need an anonymous name */
    ident Ident;
    AnonName (Ident, "bit-field");

    /* Add an anonymous bit-field that aligns to the next
    ** byte.
    */
    AddBitField (Ident, type_uchar, StructSize, BitOffs, PaddingBits,
                 /*SignednessSpecified=*/1);

    return PaddingBits;
}



static unsigned AliasAnonStructFields (const Declarator* D, SymEntry* Anon)
/* Create alias fields from an anon union/struct in the current lexical level.
** The function returns the count of created aliases.
*/
{
    unsigned Count = 0;
    SymEntry* Field;
    SymEntry* Alias;

    /* Get the symbol table containing the fields. If it is empty, there has
    ** been an error before, so bail out.
    */
    SymTable* Tab = GetESUTagSym (D->Type)->V.S.SymTab;
    if (Tab == 0) {
        /* Incomplete definition - has been flagged before */
        return 0;
    }

    /* Get a pointer to the list of symbols. Then walk the list adding copies
    ** of the embedded struct to the current level.
    */
    Field = Tab->SymHead;
    while (Field) {

        /* Enter an alias of this symbol */
        if (!IsAnonName (Field->Name)) {
            Alias = AddLocalSym (Field->Name, Field->Type, SC_STRUCTFIELD|SC_ALIAS, 0);
            Alias->V.A.Field = Field;
            Alias->V.A.Offs  = Anon->V.Offs + Field->V.Offs;
            ++Count;
        }

        /* Currently, there can not be any attributes, but if there will be
        ** some in the future, we want to know this.
        */
        CHECK (Field->Attr == 0);

        /* Next entry */
        Field = Field->NextSym;
    }

    /* Return the count of created aliases */
    return Count;
}



static SymEntry* ParseUnionSpec (const char* Name, unsigned* DSFlags)
/* Parse a union specifier */
{

    unsigned  UnionSize;
    unsigned  FieldSize;
    int       FieldWidth;       /* Width in bits, -1 if not a bit-field */
    SymTable* FieldTab;
    SymEntry* UnionTagEntry;
    SymEntry* Field;
    unsigned  Flags = 0;
    unsigned  PrevErrorCount = ErrorCount;


    if (CurTok.Tok != TOK_LCURLY) {
        /* Just a forward declaration */
        return ForwardESU (Name, SC_UNION, DSFlags);
    }

    /* Add a forward declaration for the union tag in the current lexical level */
    UnionTagEntry = AddStructSym (Name, SC_UNION, 0, 0, DSFlags);

    UnionTagEntry->V.S.ACount = 0;

    /* Skip the curly brace */
    NextToken ();

    /* Enter a new lexical level for the struct */
    EnterStructLevel ();

    /* Parse union fields */
    UnionSize = 0;
    while (CurTok.Tok != TOK_RCURLY) {

        /* Get the type of the entry */
        DeclSpec Spec;
        int SignednessSpecified = 0;

        /* Check for a _Static_assert */
        if (CurTok.Tok == TOK_STATIC_ASSERT) {
            ParseStaticAssert ();
            continue;
        }

        InitDeclSpec (&Spec);
        ParseTypeSpec (&Spec, TS_DEFAULT_TYPE_NONE, &SignednessSpecified);

        /* Read fields with this type */
        while (1) {

            Declarator Decl;

            /* Get type and name of the struct field */
            ParseDecl (&Spec, &Decl, DM_ACCEPT_IDENT);

            /* Check for a bit-field declaration */
            FieldWidth = ParseFieldWidth (&Decl);

            /* Check for fields without names */
            if (Decl.Ident[0] == '\0') {
                if (FieldWidth < 0) {
                    /* In cc65 mode, we allow anonymous structs/unions within
                    ** a union.
                    */
                    SymEntry* TagEntry;
                    if (IS_Get (&Standard) >= STD_CC65          &&
                        IsClassStruct (Decl.Type)               &&
                        (TagEntry = GetESUTagSym (Decl.Type))   &&
                        SymHasAnonName (TagEntry)) {
                        /* This is an anonymous struct or union */
                        AnonFieldName (Decl.Ident, GetBasicTypeName (Decl.Type), UnionTagEntry->V.S.ACount);

                        /* Ignore CVR qualifiers */
                        if (IsQualConst (Decl.Type) || IsQualVolatile (Decl.Type) || IsQualRestrict (Decl.Type)) {
                            Warning ("Anonymous %s qualifiers are ignored", GetBasicTypeName (Decl.Type));
                            Decl.Type[0].C &= ~T_QUAL_CVR;
                        }
                    } else {
                        /* A non bit-field without a name is legal but useless */
                        Warning ("Declaration does not declare anything");

                        goto NextMember;
                    }
                } else if (FieldWidth > 0) {
                    /* A bit-field without a name will get an anonymous one */
                    AnonName (Decl.Ident, "bit-field");
                }
            } else if (IsIncompleteType (Decl.Type)) {
                Error ("Field '%s' has incomplete type '%s'",
                       Decl.Ident,
                       GetFullTypeName (Decl.Type));
            }

            /* Check for const types */
            if (IsQualConst (Decl.Type)) {
                Flags |= SC_HAVECONST;
            }

            /* Ignore zero sized bit fields in a union */
            if (FieldWidth == 0) {
                goto NextMember;
            }

            /* Handle sizes */
            FieldSize = SizeOf (Decl.Type);
            if (FieldSize > UnionSize) {
                UnionSize = FieldSize;
            }

            /* Add a field entry to the table */
            if (FieldWidth > 0) {
                /* For a union, allocate space for the type specified by the
                ** bit-field.
                */
                AddBitField (Decl.Ident, Decl.Type, 0, 0, FieldWidth,
                             SignednessSpecified);
            } else if (Decl.Ident[0] != '\0') {
                /* Add the new field to the table */
                Field = AddLocalSym (Decl.Ident, Decl.Type, SC_STRUCTFIELD, 0);

                /* Check the new field for certain kinds of members */
                if (IsClassStruct (Decl.Type)) {
                    SymEntry* TagEntry = GetESUTagSym (Decl.Type);

                    /* Alias the fields of the anonymous member on the current level */
                    if (IsAnonName (Decl.Ident)) {
                        Field->V.A.ANumber = UnionTagEntry->V.S.ACount++;
                        AliasAnonStructFields (&Decl, Field);
                    }

                    /* Check if the field itself has a flexible array member */
                    if (TagEntry && SymHasFlexibleArrayMember (TagEntry)) {
                        Field->Flags |= SC_HAVEFAM;
                        Flags        |= SC_HAVEFAM;
                    }

                    /* Check if the field itself has a const member */
                    if (TagEntry && SymHasConstMember (TagEntry)) {
                        Field->Flags |= SC_HAVECONST;
                        Flags        |= SC_HAVECONST;
                    }
                }
            }

NextMember: if (CurTok.Tok != TOK_COMMA) {
                break;
            }
            NextToken ();
        }
        ConsumeSemi ();
    }

    /* Skip the closing brace */
    NextToken ();

    /* Remember the symbol table and leave the struct level */
    FieldTab = GetFieldSymTab ();
    LeaveStructLevel ();

    /* Return a fictitious symbol if errors occurred during parsing */
    if (PrevErrorCount != ErrorCount) {
        Flags |= SC_FICTITIOUS;
    }

    /* Empty union is not supported now */
    if (UnionSize == 0) {
        Error ("Empty union type '%s' is not supported", Name);
    }

    /* Make a real entry from the forward decl and return it */
    return AddStructSym (Name, SC_UNION | SC_DEF | Flags, UnionSize, FieldTab, DSFlags);
}



static SymEntry* ParseStructSpec (const char* Name, unsigned* DSFlags)
/* Parse a struct specifier */
{

    unsigned  StructSize;
    int       FlexibleMember;
    unsigned  BitOffs;          /* Bit offset for bit-fields */
    int       FieldWidth;       /* Width in bits, -1 if not a bit-field */
    SymTable* FieldTab;
    SymEntry* StructTagEntry;
    SymEntry* Field;
    unsigned  Flags = 0;
    unsigned  PrevErrorCount = ErrorCount;


    if (CurTok.Tok != TOK_LCURLY) {
        /* Just a forward declaration */
        return ForwardESU (Name, SC_STRUCT, DSFlags);
    }

    /* Add a forward declaration for the struct tag in the current lexical level */
    StructTagEntry = AddStructSym (Name, SC_STRUCT, 0, 0, DSFlags);

    StructTagEntry->V.S.ACount = 0;

    /* Skip the curly brace */
    NextToken ();

    /* Enter a new lexical level for the struct */
    EnterStructLevel ();

    /* Parse struct fields */
    FlexibleMember = 0;
    StructSize     = 0;
    BitOffs        = 0;
    while (CurTok.Tok != TOK_RCURLY) {

        /* Get the type of the entry */
        DeclSpec Spec;
        int SignednessSpecified = 0;

        /* Check for a _Static_assert */
        if (CurTok.Tok == TOK_STATIC_ASSERT) {
            ParseStaticAssert ();
            continue;
        }

        InitDeclSpec (&Spec);
        ParseTypeSpec (&Spec, TS_DEFAULT_TYPE_NONE, &SignednessSpecified);

        /* Read fields with this type */
        while (1) {

            Declarator Decl;

            /* If we had a flexible array member before, no other fields can
            ** follow.
            */
            if (FlexibleMember) {
                Error ("Flexible array member must be last field");
                FlexibleMember = 0;     /* Avoid further errors */
            }

            /* Get type and name of the struct field */
            ParseDecl (&Spec, &Decl, DM_ACCEPT_IDENT);

            /* Check for a bit-field declaration */
            FieldWidth = ParseFieldWidth (&Decl);

            /* If this is not a bit field, or the bit field is too large for
            ** the remainder of the allocated unit, or we have a bit field
            ** with width zero, align the struct to the next member by adding
            ** a member with an anonymous name.
            */
            if (BitOffs > 0) {
                if (FieldWidth <= 0 ||
                    (BitOffs + FieldWidth) > CHAR_BITS * SizeOf (Decl.Type)) {
                    /* Add an anonymous bit-field that aligns to the next
                    ** byte.
                    */
                    unsigned PaddingBits = PadWithBitField (StructSize, BitOffs);

                    /* No bits left */
                    StructSize += (BitOffs + PaddingBits) / CHAR_BITS;
                    BitOffs = 0;
                }
            }

            /* Check for fields without names */
            if (Decl.Ident[0] == '\0') {
                if (FieldWidth < 0) {
                    /* In cc65 mode, we allow anonymous structs/unions within
                    ** a struct.
                    */
                    SymEntry* TagEntry;
                    if (IS_Get (&Standard) >= STD_CC65          &&
                        IsClassStruct (Decl.Type)               &&
                        (TagEntry = GetESUTagSym (Decl.Type))   &&
                        SymHasAnonName (TagEntry)) {
                        /* This is an anonymous struct or union */
                        AnonFieldName (Decl.Ident, GetBasicTypeName (Decl.Type), StructTagEntry->V.S.ACount);

                        /* Ignore CVR qualifiers */
                        if (IsQualConst (Decl.Type) || IsQualVolatile (Decl.Type) || IsQualRestrict (Decl.Type)) {
                            Warning ("Anonymous %s qualifiers are ignored", GetBasicTypeName (Decl.Type));
                            Decl.Type[0].C &= ~T_QUAL_CVR;
                        }
                    } else {
                        /* A non bit-field without a name is legal but useless */
                        Warning ("Declaration does not declare anything");

                        goto NextMember;
                    }
                } else if (FieldWidth > 0) {
                    /* A bit-field without a name will get an anonymous one */
                    AnonName (Decl.Ident, "bit-field");
                }
            } else {
                /* Check if this field is a flexible array member, and
                ** calculate the size of the field.
                */
                if (IsTypeArray (Decl.Type) && GetElementCount (Decl.Type) == UNSPECIFIED) {
                    /* Array with unspecified size */
                    if (StructSize == 0) {
                        Error ("Flexible array member cannot be first struct field");
                    }
                    FlexibleMember = 1;
                    Flags |= SC_HAVEFAM;

                    /* Assume zero for size calculations */
                    SetElementCount (Decl.Type, FLEXIBLE);
                }

                if (IsIncompleteType (Decl.Type)) {
                    Error ("Field '%s' has incomplete type '%s'",
                           Decl.Ident,
                           GetFullTypeName (Decl.Type));
                }
            }

            /* Check for const types */
            if (IsQualConst (Decl.Type)) {
                Flags |= SC_HAVECONST;
            }

            /* Apart from the above, a bit field with width 0 is not processed
            ** further.
            */
            if (FieldWidth == 0) {
                goto NextMember;
            }

            /* Add a field entry to the table */
            if (FieldWidth > 0) {
                /* Full bytes have already been added to the StructSize,
                ** which is passed to the offset of AddBitField.  BitOffs
                ** is always within a char, which simplifies handling the
                ** bit-field as a char type in expressions.
                */
                CHECK (BitOffs < CHAR_BITS);
                AddBitField (Decl.Ident, Decl.Type, StructSize, BitOffs,
                             FieldWidth, SignednessSpecified);
                BitOffs += FieldWidth;
                CHECK (BitOffs <= CHAR_BITS * SizeOf (Decl.Type));
                /* Add any full bytes to the struct size */
                StructSize += BitOffs / CHAR_BITS;
                BitOffs %= CHAR_BITS;
            } else if (Decl.Ident[0] != '\0') {
                /* Add the new field to the table */
                Field = AddLocalSym (Decl.Ident, Decl.Type, SC_STRUCTFIELD, StructSize);

                /* Check the new field for certain kinds of members */
                if (IsClassStruct (Decl.Type)) {
                    SymEntry* TagEntry = GetESUTagSym (Decl.Type);

                    /* Alias the fields of the anonymous member on the current level */
                    if (IsAnonName (Decl.Ident)) {
                        Field->V.A.ANumber = StructTagEntry->V.S.ACount++;
                        AliasAnonStructFields (&Decl, Field);
                    }

                    /* Check if the field itself has a flexible array member */
                    if (TagEntry && SymHasFlexibleArrayMember (TagEntry)) {
                        Field->Flags |= SC_HAVEFAM;
                        Flags        |= SC_HAVEFAM;
                        Error ("Invalid use of struct with flexible array member");
                    }

                    /* Check if the field itself has a const member */
                    if (TagEntry && SymHasConstMember (TagEntry)) {
                        Field->Flags |= SC_HAVECONST;
                        Flags        |= SC_HAVECONST;
                    }
                }

                if (!FlexibleMember) {
                    StructSize += SizeOf (Decl.Type);
                }
            }

NextMember: if (CurTok.Tok != TOK_COMMA) {
                break;
            }
            NextToken ();
        }
        ConsumeSemi ();
    }

    if (BitOffs > 0) {
        /* If we have bits from bit-fields left, pad the struct to next byte */
        unsigned PaddingBits = PadWithBitField (StructSize, BitOffs);

        /* No bits left */
        StructSize += (BitOffs + PaddingBits) / CHAR_BITS;
    }

    /* Skip the closing brace */
    NextToken ();

    /* Remember the symbol table and leave the struct level */
    FieldTab = GetFieldSymTab ();
    LeaveStructLevel ();

    /* Return a fictitious symbol if errors occurred during parsing */
    if (PrevErrorCount != ErrorCount) {
        Flags |= SC_FICTITIOUS;
    }

    /* Empty struct is not supported now */
    if (StructSize == 0) {
        Error ("Empty struct type '%s' is not supported", Name);
    }

    /* Make a real entry from the forward decl and return it */
    return AddStructSym (Name, SC_STRUCT | SC_DEF | Flags, StructSize, FieldTab, DSFlags);
}



static void ParseTypeSpec (DeclSpec* Spec, typespec_t TSFlags, int* SignednessSpecified)
/* Parse a type specifier.  Store whether one of "signed" or "unsigned" was
** specified, so bit-fields of unspecified signedness can be treated as
** unsigned; without special handling, it would be treated as signed.
*/
{
    ident       Ident;
    SymEntry*   TagEntry;
    TypeCode    Qualifiers = T_QUAL_NONE;

    if (SignednessSpecified != NULL) {
        *SignednessSpecified = 0;
    }

    /* Assume we have an explicit type */
    Spec->Flags &= ~DS_DEF_TYPE;

    /* Read storage specifiers and/or type qualifiers if we have any */
    OptionalSpecifiers (Spec, &Qualifiers, TSFlags);

    /* Look at the data type */
    switch (CurTok.Tok) {

        case TOK_VOID:
            NextToken ();
            Spec->Type[0].C = T_VOID;
            Spec->Type[0].A.U = 0;
            Spec->Type[1].C = T_END;
            break;

        case TOK_CHAR:
            NextToken ();
            Spec->Type[0].C = T_CHAR;
            Spec->Type[1].C = T_END;
            break;

        case TOK_LONG:
            NextToken ();
            if (CurTok.Tok == TOK_UNSIGNED) {
                if (SignednessSpecified != NULL) {
                    *SignednessSpecified = 1;
                }
                NextToken ();
                OptionalInt ();
                Spec->Type[0].C = T_ULONG;
                Spec->Type[1].C = T_END;
            } else {
                OptionalSigned (SignednessSpecified);
                OptionalInt ();
                Spec->Type[0].C = T_LONG;
                Spec->Type[1].C = T_END;
            }
            break;

        case TOK_SHORT:
            NextToken ();
            if (CurTok.Tok == TOK_UNSIGNED) {
                if (SignednessSpecified != NULL) {
                    *SignednessSpecified = 1;
                }
                NextToken ();
                OptionalInt ();
                Spec->Type[0].C = T_USHORT;
                Spec->Type[1].C = T_END;
            } else {
                OptionalSigned (SignednessSpecified);
                OptionalInt ();
                Spec->Type[0].C = T_SHORT;
                Spec->Type[1].C = T_END;
            }
            break;

        case TOK_INT:
            NextToken ();
            Spec->Type[0].C = T_INT;
            Spec->Type[1].C = T_END;
            break;

       case TOK_SIGNED:
            if (SignednessSpecified != NULL) {
                *SignednessSpecified = 1;
            }
            NextToken ();
            switch (CurTok.Tok) {

                case TOK_CHAR:
                    NextToken ();
                    Spec->Type[0].C = T_SCHAR;
                    Spec->Type[1].C = T_END;
                    break;

                case TOK_SHORT:
                    NextToken ();
                    OptionalInt ();
                    Spec->Type[0].C = T_SHORT;
                    Spec->Type[1].C = T_END;
                    break;

                case TOK_LONG:
                    NextToken ();
                    OptionalInt ();
                    Spec->Type[0].C = T_LONG;
                    Spec->Type[1].C = T_END;
                    break;

                case TOK_INT:
                    NextToken ();
                    /* FALL THROUGH */

                default:
                    Spec->Type[0].C = T_INT;
                    Spec->Type[1].C = T_END;
                    break;
            }
            break;

        case TOK_UNSIGNED:
            if (SignednessSpecified != NULL) {
                *SignednessSpecified = 1;
            }
            NextToken ();
            switch (CurTok.Tok) {

                case TOK_CHAR:
                    NextToken ();
                    Spec->Type[0].C = T_UCHAR;
                    Spec->Type[1].C = T_END;
                    break;

                case TOK_SHORT:
                    NextToken ();
                    OptionalInt ();
                    Spec->Type[0].C = T_USHORT;
                    Spec->Type[1].C = T_END;
                    break;

                case TOK_LONG:
                    NextToken ();
                    OptionalInt ();
                    Spec->Type[0].C = T_ULONG;
                    Spec->Type[1].C = T_END;
                    break;

                case TOK_INT:
                    NextToken ();
                    /* FALL THROUGH */

                default:
                    Spec->Type[0].C = T_UINT;
                    Spec->Type[1].C = T_END;
                    break;
            }
            break;

        case TOK_FLOAT:
            NextToken ();
            Spec->Type[0].C = T_FLOAT;
            Spec->Type[1].C = T_END;
            break;

        case TOK_DOUBLE:
            NextToken ();
            Spec->Type[0].C = T_DOUBLE;
            Spec->Type[1].C = T_END;
            break;

        case TOK_UNION:
            NextToken ();
            /* */
            if (CurTok.Tok == TOK_IDENT) {
                strcpy (Ident, CurTok.Ident);
                NextToken ();
            } else {
                AnonName (Ident, "union");
            }
            /* Remember we have an extra type decl */
            Spec->Flags |= DS_EXTRA_TYPE;
            /* Declare the union in the current scope */
            TagEntry = ParseUnionSpec (Ident, &Spec->Flags);
            /* Encode the union entry into the type */
            Spec->Type[0].C = T_UNION;
            SetESUTagSym (Spec->Type, TagEntry);
            Spec->Type[1].C = T_END;
            break;

        case TOK_STRUCT:
            NextToken ();
            /* */
            if (CurTok.Tok == TOK_IDENT) {
                strcpy (Ident, CurTok.Ident);
                NextToken ();
            } else {
                AnonName (Ident, "struct");
            }
            /* Remember we have an extra type decl */
            Spec->Flags |= DS_EXTRA_TYPE;
            /* Declare the struct in the current scope */
            TagEntry = ParseStructSpec (Ident, &Spec->Flags);
            /* Encode the struct entry into the type */
            Spec->Type[0].C = T_STRUCT;
            SetESUTagSym (Spec->Type, TagEntry);
            Spec->Type[1].C = T_END;
            break;

        case TOK_ENUM:
            NextToken ();
            /* Named enum */
            if (CurTok.Tok == TOK_IDENT) {
                strcpy (Ident, CurTok.Ident);
                NextToken ();
            } else {
                if (CurTok.Tok != TOK_LCURLY) {
                    Error ("Identifier expected for enum tag name");
                }
                AnonName (Ident, "enum");
            }
            /* Remember we have an extra type decl */
            Spec->Flags |= DS_EXTRA_TYPE;
            /* Parse the enum decl */
            TagEntry = ParseEnumSpec (Ident, &Spec->Flags);
            /* Encode the enum entry into the type */
            Spec->Type[0].C |= T_ENUM;
            SetESUTagSym (Spec->Type, TagEntry);
            Spec->Type[1].C = T_END;
            /* The signedness of enums is determined by the type, so say this is specified to avoid
            ** the int -> unsigned int handling for plain int bit-fields in AddBitField.
            */
            if (SignednessSpecified) {
                *SignednessSpecified = 1;
            }
            break;

        case TOK_IDENT:
            /* This could be a label */
            if (NextTok.Tok != TOK_COLON || GetLexicalLevel () == LEX_LEVEL_STRUCT) {
                TagEntry = FindSym (CurTok.Ident);
                if (TagEntry && SymIsTypeDef (TagEntry)) {
                    /* It's a typedef */
                    NextToken ();
                    TypeCopy (Spec->Type, TagEntry->Type);
                    /* If it's a typedef, we should actually use whether the signedness was
                    ** specified on the typedef, but that information has been lost.  Treat the
                    ** signedness as being specified to work around the ICE in #1267.
                    ** Unforunately, this will cause plain int bit-fields defined via typedefs
                    ** to be treated as signed rather than unsigned.
                    */
                    if (SignednessSpecified) {
                        *SignednessSpecified = 1;
                    }
                    break;
                }
            } else {
                /* This is a label. Use the default type flag to end the loop
                ** in DeclareLocals. The type code used here doesn't matter as
                ** long as it has no qualifiers.
                */
                Spec->Flags |= DS_DEF_TYPE;
                Spec->Type[0].C = T_INT;
                Spec->Type[1].C = T_END;
                break;
            }
            /* FALL THROUGH */

        default:
            if ((TSFlags & TS_MASK_DEFAULT_TYPE) == TS_DEFAULT_TYPE_NONE) {
                Spec->Flags |= DS_NO_TYPE;
                Spec->Type[0].C = T_INT;
                Spec->Type[1].C = T_END;
            } else {
                Spec->Flags |= DS_DEF_TYPE;
                Spec->Type[0].C = T_INT;
                Spec->Type[1].C = T_END;
            }
            break;
    }

    /* There may also be specifiers/qualifiers *after* the initial type */
    OptionalSpecifiers (Spec, &Qualifiers, TSFlags);
    Spec->Type[0].C |= Qualifiers;
}



static const Type* ParamTypeCvt (Type* T)
/* If T is an array or a function, convert it to a pointer else do nothing.
** Return the resulting type.
*/
{
    Type* Tmp = 0;

    if (IsTypeArray (T)) {
        Tmp = ArrayToPtr (T);
    } else if (IsTypeFunc (T)) {
        Tmp = NewPointerTo (T);
    }

    if (Tmp != 0) {
        /* Do several fixes on qualifiers */
        FixQualifiers (Tmp);

        /* Replace the type */
        TypeCopy (T, Tmp);
        TypeFree (Tmp);
    }

    return T;
}



static void ParseOldStyleParamList (FuncDesc* F)
/* Parse an old-style (K&R) parameter list */
{
    unsigned PrevErrorCount = ErrorCount;

    /* Parse params */
    while (CurTok.Tok != TOK_RPAREN) {

        /* List of identifiers expected */
        if (CurTok.Tok == TOK_IDENT) {

            /* Create a symbol table entry with type int */
            AddLocalSym (CurTok.Ident, type_int, SC_AUTO | SC_PARAM | SC_DEF | SC_DEFTYPE, 0);

            /* Count arguments */
            ++F->ParamCount;

            /* Skip the identifier */
            NextToken ();

        } else {
            /* Some fix point tokens that are used for error recovery */
            static const token_t TokenList[] = { TOK_COMMA, TOK_RPAREN, TOK_SEMI };

            /* Not a parameter name */
            Error ("Identifier expected for parameter name");

            /* Try some smart error recovery */
            SkipTokens (TokenList, sizeof(TokenList) / sizeof(TokenList[0]));
        }

        /* Check for more parameters */
        if (CurTok.Tok == TOK_COMMA) {
            NextToken ();
        } else {
            break;
        }
    }

    /* Skip right paren. We must explicitly check for one here, since some of
    ** the breaks above bail out without checking.
    */
    ConsumeRParen ();

    /* An optional list of type specifications follows */
    while (CurTok.Tok != TOK_LCURLY) {

        DeclSpec        Spec;

        /* Read the declaration specifier */
        ParseDeclSpec (&Spec, TS_DEFAULT_TYPE_NONE, SC_AUTO);

        /* We accept only auto and register as storage class specifiers, but
        ** we ignore all this, since we use auto anyway.
        */
        if ((Spec.StorageClass & SC_AUTO) == 0 &&
            (Spec.StorageClass & SC_REGISTER) == 0) {
            Error ("Illegal storage class");
        }

        /* Type must be specified */
        if ((Spec.Flags & DS_NO_TYPE) != 0) {
            Error ("Expected declaration specifiers");
            break;
        }

        /* Parse a comma separated variable list */
        while (1) {

            Declarator Decl;

            /* Read the parameter */
            ParseDecl (&Spec, &Decl, DM_NEED_IDENT);

            /* Warn about new local type declaration */
            if ((Spec.Flags & DS_NEW_TYPE_DECL) != 0) {
                Warning ("'%s' will be invisible out of this function",
                         GetFullTypeName (Spec.Type));
            }

            if (Decl.Ident[0] != '\0') {

                /* We have a name given. Search for the symbol */
                SymEntry* Param = FindLocalSym (Decl.Ident);
                if (Param) {
                    /* Check if we already changed the type for this
                    ** parameter.
                    */
                    if (Param->Flags & SC_DEFTYPE) {
                        /* Found it, change the default type to the one given */
                        SymChangeType (Param, ParamTypeCvt (Decl.Type));
                        /* Reset the "default type" flag */
                        Param->Flags &= ~SC_DEFTYPE;
                    } else {
                        /* Type has already been changed */
                        Error ("Redefinition for parameter '%s'", Param->Name);
                    }
                } else {
                    Error ("Unknown identifier: '%s'", Decl.Ident);
                }
            }

            if (CurTok.Tok == TOK_COMMA) {
                NextToken ();
            } else {
                break;
            }

        }

        /* Variable list must be semicolon terminated */
        ConsumeSemi ();
    }

    if (PrevErrorCount != ErrorCount) {
        /* Some fix point tokens that are used for error recovery */
        static const token_t TokenList[] = { TOK_COMMA, TOK_SEMI };

        /* Try some smart error recovery */
        SkipTokens (TokenList, sizeof(TokenList) / sizeof(TokenList[0]));
    }
}



static void ParseAnsiParamList (FuncDesc* F)
/* Parse a new-style (ANSI) parameter list */
{
    /* Parse params */
    while (CurTok.Tok != TOK_RPAREN) {

        DeclSpec    Spec;
        Declarator  Decl;
        SymEntry*   Param;

        /* Allow an ellipsis as last parameter */
        if (CurTok.Tok == TOK_ELLIPSIS) {
            NextToken ();
            F->Flags |= FD_VARIADIC;
            break;
        }

        /* Read the declaration specifier */
        ParseDeclSpec (&Spec, TS_DEFAULT_TYPE_NONE, SC_AUTO);

        /* We accept only auto and register as storage class specifiers */
        if ((Spec.StorageClass & SC_AUTO) == SC_AUTO) {
            Spec.StorageClass = SC_AUTO | SC_PARAM | SC_DEF;
        } else if ((Spec.StorageClass & SC_REGISTER) == SC_REGISTER) {
            Spec.StorageClass = SC_REGISTER | SC_STATIC | SC_PARAM | SC_DEF;
        } else {
            Error ("Illegal storage class");
            Spec.StorageClass = SC_AUTO | SC_PARAM | SC_DEF;
        }

        /* Type must be specified */
        if ((Spec.Flags & DS_NO_TYPE) != 0) {
            Error ("Type specifier missing");
        }

        /* Warn about new local type declaration */
        if ((Spec.Flags & DS_NEW_TYPE_DECL) != 0) {
            Warning ("'%s' will be invisible out of this function",
                     GetFullTypeName (Spec.Type));
        }

        /* Allow parameters without a name, but remember if we had some to
        ** eventually print an error message later.
        */
        ParseDecl (&Spec, &Decl, DM_ACCEPT_IDENT);
        if (Decl.Ident[0] == '\0') {

            /* Unnamed symbol. Generate a name that is not user accessible,
            ** then handle the symbol normal.
            */
            AnonName (Decl.Ident, "param");
            F->Flags |= FD_UNNAMED_PARAMS;

            /* Clear defined bit on nonames */
            Decl.StorageClass &= ~SC_DEF;
        }

        /* Parse attributes for this parameter */
        ParseAttribute (&Decl);

        /* Create a symbol table entry */
        Param = AddLocalSym (Decl.Ident, ParamTypeCvt (Decl.Type), Decl.StorageClass, 0);

        /* Add attributes if we have any */
        SymUseAttr (Param, &Decl);

        /* If the parameter is a struct or union, emit a warning */
        if (IsClassStruct (Decl.Type)) {
            if (IS_Get (&WarnStructParam)) {
                Warning ("Passing struct by value for parameter '%s'", Decl.Ident);
            }
        }

        /* Count arguments */
        ++F->ParamCount;

        /* Check for more parameters */
        if (CurTok.Tok == TOK_COMMA) {
            NextToken ();
        } else {
            break;
        }
    }

    /* Skip right paren. We must explicitly check for one here, since some of
    ** the breaks above bail out without checking.
    */
    ConsumeRParen ();
}



static FuncDesc* ParseFuncDecl (void)
/* Parse the argument list of a function with the enclosing parentheses */
{
    SymEntry* WrappedCall;
    unsigned int WrappedCallData;

    /* Create a new function descriptor */
    FuncDesc* F = NewFuncDesc ();

    /* Enter a new lexical level */
    EnterFunctionLevel ();

    /* Skip the opening paren */
    NextToken ();

    /* Check for several special parameter lists */
    if (CurTok.Tok == TOK_RPAREN) {
        /* Parameter list is empty (K&R-style) */
        F->Flags |= FD_EMPTY;
    } else if (CurTok.Tok == TOK_VOID && NextTok.Tok == TOK_RPAREN) {
        /* Parameter list declared as void */
        NextToken ();
        F->Flags |= FD_VOID_PARAM;
    } else if (CurTok.Tok == TOK_IDENT &&
               (NextTok.Tok == TOK_COMMA || NextTok.Tok == TOK_RPAREN)) {
        /* If the identifier is a typedef, we have a new-style parameter list;
        ** if it's some other identifier, it's an old-style parameter list.
        */
        SymEntry* Sym = FindSym (CurTok.Ident);
        if (Sym == 0 || !SymIsTypeDef (Sym)) {
            /* Old-style (K&R) function. */
            F->Flags |= FD_OLDSTYLE;
        }
    }

    /* Parse params */
    if ((F->Flags & FD_OLDSTYLE) == 0) {
        /* New-style function */
        ParseAnsiParamList (F);
    } else {
        /* Old-style function */
        ParseOldStyleParamList (F);
    }

    /* Remember the last function parameter. We need it later for several
    ** purposes, for example when passing stuff to fastcall functions. Since
    ** more symbols are added to the table, it is easier if we remember it
    ** now, since it is currently the last entry in the symbol table.
    */
    F->LastParam = GetSymTab()->SymTail;

    /* It is allowed to use incomplete types in function prototypes, so we
    ** won't always get to know the parameter sizes here and may do that later.
    */
    F->Flags |= FD_INCOMPLETE_PARAM;

    /* Leave the lexical level remembering the symbol tables */
    RememberFunctionLevel (F);

    /* Did we have a WrappedCall for this function? */
    GetWrappedCall((void **) &WrappedCall, &WrappedCallData);
    if (WrappedCall) {
        F->WrappedCall = WrappedCall;
        F->WrappedCallData = WrappedCallData;
    }

    /* Return the function descriptor */
    return F;
}



static void DirectDecl (const DeclSpec* Spec, Declarator* D, declmode_t Mode)
/* Recursively process direct declarators. Build a type array in reverse order. */
{
    /* Read optional function or pointer qualifiers that modify the identifier
    ** or token to the right. For convenience, we allow a calling convention
    ** also for pointers here. If it's a pointer-to-function, the qualifier
    ** later will be transfered to the function itself. If it's a pointer to
    ** something else, it will be flagged as an error.
    */
    TypeCode Qualifiers = OptionalQualifiers (T_QUAL_NONE, T_QUAL_ADDRSIZE | T_QUAL_CCONV);

    /* Pointer to something */
    if (CurTok.Tok == TOK_STAR) {

        /* Skip the star */
        NextToken ();

        /* Allow const, restrict, and volatile qualifiers */
        Qualifiers |= OptionalQualifiers (Qualifiers, T_QUAL_CVR);

        /* Parse the type that the pointer points to */
        DirectDecl (Spec, D, Mode);

        /* Add the type */
        AddTypeCodeToDeclarator (D, T_PTR | Qualifiers);
        return;
    }

    if (CurTok.Tok == TOK_LPAREN) {
        NextToken ();
        DirectDecl (Spec, D, Mode);
        ConsumeRParen ();
    } else {
        /* Things depend on Mode now:
        **  - Mode == DM_NEED_IDENT means:
        **      we *must* have a type and a variable identifer.
        **  - Mode == DM_NO_IDENT means:
        **      we must have a type but no variable identifer
        **      (if there is one, it's not read).
        **  - Mode == DM_ACCEPT_IDENT means:
        **      we *may* have an identifier. If there is an identifier,
        **      it is read, but it is no error, if there is none.
        */
        if (Mode == DM_NO_IDENT) {
            D->Ident[0] = '\0';
        } else if (CurTok.Tok == TOK_IDENT) {
            strcpy (D->Ident, CurTok.Ident);
            NextToken ();
        } else {
            if (Mode == DM_NEED_IDENT) {
                /* Some fix point tokens that are used for error recovery */
                static const token_t TokenList[] = { TOK_COMMA, TOK_SEMI, TOK_LCURLY, TOK_RCURLY };

                Error ("Identifier expected");

                /* Try some smart error recovery */
                SkipTokens (TokenList, sizeof(TokenList) / sizeof(TokenList[0]));

                /* Skip curly braces */
                if (CurTok.Tok == TOK_LCURLY) {
                    static const token_t CurlyToken[] = { TOK_RCURLY };
                    SkipTokens (CurlyToken, sizeof(CurlyToken) / sizeof(CurlyToken[0]));
                    NextToken ();
                } else if (CurTok.Tok == TOK_RCURLY) {
                    NextToken ();
                }
            }
            D->Ident[0] = '\0';
        }
    }

    while (CurTok.Tok == TOK_LBRACK || CurTok.Tok == TOK_LPAREN) {
        if (CurTok.Tok == TOK_LPAREN) {

            /* Function declarator */
            FuncDesc* F;
            SymEntry* PrevEntry;

            /* Parse the function declarator */
            F = ParseFuncDecl ();

            /* We cannot specify fastcall for variadic functions */
            if ((F->Flags & FD_VARIADIC) && (Qualifiers & T_QUAL_FASTCALL)) {
                Error ("Variadic functions cannot be __fastcall__");
                Qualifiers &= ~T_QUAL_FASTCALL;
            }

            /* Was there a previous entry? If so, copy WrappedCall info from it */
            PrevEntry = FindGlobalSym (D->Ident);
            if (PrevEntry && PrevEntry->Flags & SC_FUNC) {
                FuncDesc* D = GetFuncDesc (PrevEntry->Type);
                if (D->WrappedCall && !F->WrappedCall) {
                    F->WrappedCall = D->WrappedCall;
                    F->WrappedCallData = D->WrappedCallData;
                }
            }

            /* Add the function type. Be sure to bounds check the type buffer */
            NeedTypeSpace (D, 1);
            D->Type[D->Index].C = T_FUNC | Qualifiers;
            D->Type[D->Index].A.F = F;
            ++D->Index;

            /* Qualifiers now used */
            Qualifiers = T_QUAL_NONE;

        } else {
            /* Array declarator */
            long Size = UNSPECIFIED;

            /* We cannot have any qualifiers for an array */
            if (Qualifiers != T_QUAL_NONE) {
                Error ("Invalid qualifiers for array");
                Qualifiers = T_QUAL_NONE;
            }

            /* Skip the left bracket */
            NextToken ();

            /* Read the size if it is given */
            if (CurTok.Tok != TOK_RBRACK) {
                ExprDesc Expr = NoCodeConstAbsIntExpr (hie1);
                if (Expr.IVal <= 0) {
                    if (D->Ident[0] != '\0') {
                        Error ("Size of array '%s' is invalid", D->Ident);
                    } else {
                        Error ("Size of array is invalid");
                    }
                    Expr.IVal = 1;
                }
                Size = Expr.IVal;
            }

            /* Skip the right bracket */
            ConsumeRBrack ();

            /* Add the array type with the size to the type */
            NeedTypeSpace (D, 1);
            D->Type[D->Index].C = T_ARRAY;
            D->Type[D->Index].A.L = Size;
            ++D->Index;
        }
    }

    /* If we have remaining qualifiers, flag them as invalid */
    if (Qualifiers & T_QUAL_NEAR) {
        Error ("Invalid '__near__' qualifier");
    }
    if (Qualifiers & T_QUAL_FAR) {
        Error ("Invalid '__far__' qualifier");
    }
    if (Qualifiers & T_QUAL_FASTCALL) {
        Error ("Invalid '__fastcall__' qualifier");
    }
    if (Qualifiers & T_QUAL_CDECL) {
        Error ("Invalid '__cdecl__' qualifier");
    }
}



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



Type* ParseType (Type* T)
/* Parse a complete type specification */
{
    DeclSpec Spec;
    Declarator Decl;

    /* Get a type without a default */
    InitDeclSpec (&Spec);
    ParseTypeSpec (&Spec, TS_DEFAULT_TYPE_NONE, NULL);

    /* Parse additional declarators */
    ParseDecl (&Spec, &Decl, DM_NO_IDENT);

    /* Copy the type to the target buffer */
    TypeCopy (T, Decl.Type);

    /* Return a pointer to the target buffer */
    return T;
}



void ParseDecl (const DeclSpec* Spec, Declarator* D, declmode_t Mode)
/* Parse a variable, type or function declarator */
{
    /* Used to check if we have any errors during parsing this */
    unsigned PrevErrorCount = ErrorCount;

    /* Initialize the Declarator struct */
    InitDeclarator (D);

    /* Get additional derivation of the declarator and the identifier */
    DirectDecl (Spec, D, Mode);

    /* Add the base type */
    NeedTypeSpace (D, TypeLen (Spec->Type) + 1);        /* Bounds check */
    TypeCopy (D->Type + D->Index, Spec->Type);

    /* Use the storage class from the declspec */
    D->StorageClass = Spec->StorageClass;

    /* Do several fixes on qualifiers */
    FixQualifiers (D->Type);

    /* Check if the data type consists of any functions returning forbidden return
    ** types and remove qualifiers from the return types if they are not void.
    */
    FixFunctionReturnType (D->Type);

    /* Check recursively if the data type consists of arrays of forbidden types */
    CheckArrayElementType (D->Type);

    /* Parse attributes for this declarator */
    ParseAttribute (D);

    /* If we have a function, add a special storage class */
    if (IsTypeFunc (D->Type)) {

        D->StorageClass |= SC_FUNC;

    } else if (!IsTypeVoid (D->Type)) {
        /* Check the size of the generated type */
        unsigned Size = SizeOf (D->Type);

        if (Size >= 0x10000) {
            if (D->Ident[0] != '\0') {
                Error ("Size of '%s' is invalid (0x%06X)", D->Ident, Size);
            } else {
                Error ("Invalid size in declaration (0x%06X)", Size);
            }
        }
    }

    /* Check a few pre-C99 things */
    if ((Spec->Flags & DS_DEF_TYPE) != 0) {
        /* Check and warn about an implicit int return in the function */
        if (IsTypeFunc (D->Type) && IsRankInt (GetFuncReturnType (D->Type))) {
            /* Function has an implicit int return. Output a warning if we don't
            ** have the C89 standard enabled explicitly.
            */
            if (IS_Get (&Standard) >= STD_C99) {
                Warning ("Implicit 'int' return type is an obsolete feature");
            }
            GetFuncDesc (D->Type)->Flags |= FD_OLDSTYLE_INTRET;
        }

        /* For anthing that is not a function or typedef, check for an implicit
        ** int declaration.
        */
        if ((D->StorageClass & SC_FUNC) != SC_FUNC &&
            (D->StorageClass & SC_TYPEMASK) != SC_TYPEDEF) {
            /* If the standard was not set explicitly to C89, print a warning
            ** for variables with implicit int type.
            */
            if (IS_Get (&Standard) >= STD_C99) {
                Warning ("Implicit 'int' is an obsolete feature");
            }
        }
    }

    if (PrevErrorCount != ErrorCount) {
        /* Make the declaration fictitious if is is not parsed correctly */
        D->StorageClass |= SC_FICTITIOUS;

        if (Mode == DM_NEED_IDENT && D->Ident[0] == '\0') {
            /* Use a fictitious name for the identifier if it is missing */
            AnonName (D->Ident, "global");
        }
    }
}



void ParseDeclSpec (DeclSpec* Spec, typespec_t TSFlags, unsigned DefStorage)
/* Parse a declaration specification */
{
    /* Initialize the DeclSpec struct */
    InitDeclSpec (Spec);

    /* Assume we're using an explicit storage class */
    Spec->Flags &= ~DS_DEF_STORAGE;

    /* Parse the type specifiers */
    ParseTypeSpec (Spec, TSFlags | TS_STORAGE_CLASS_SPEC | TS_FUNCTION_SPEC, NULL);

    /* If no explicit storage class is given, use the default */
    if (Spec->StorageClass == 0) {
        Spec->Flags |= DS_DEF_STORAGE;
        Spec->StorageClass = DefStorage;
    }
}



void CheckEmptyDecl (const DeclSpec* Spec)
/* Called after an empty type declaration (that is, a type declaration without
** a variable). Checks if the declaration does really make sense and issues a
** warning if not.
*/
{
    if ((Spec->Flags & DS_EXTRA_TYPE) == 0) {
        Warning ("Useless declaration");
    }
}
