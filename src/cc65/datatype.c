/*****************************************************************************/
/*                                                                           */
/*                                datatype.c                                 */
/*                                                                           */
/*               Type string handling for the cc65 C compiler                */
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

/* common */
#include "addrsize.h"
#include "check.h"
#include "mmodel.h"
#include "xmalloc.h"

/* cc65 */
#include "codegen.h"
#include "datatype.h"
#include "error.h"
#include "fp.h"
#include "funcdesc.h"
#include "global.h"
#include "symtab.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Predefined type strings */
Type type_schar[]       = { TYPE(T_SCHAR),  TYPE(T_END) };
Type type_uchar[]       = { TYPE(T_UCHAR),  TYPE(T_END) };
Type type_int[]         = { TYPE(T_INT),    TYPE(T_END) };
Type type_uint[]        = { TYPE(T_UINT),   TYPE(T_END) };
Type type_long[]        = { TYPE(T_LONG),   TYPE(T_END) };
Type type_ulong[]       = { TYPE(T_ULONG),  TYPE(T_END) };
Type type_void[]        = { TYPE(T_VOID),   TYPE(T_END) };
Type type_size_t[]      = { TYPE(T_SIZE_T), TYPE(T_END) };
Type type_float[]       = { TYPE(T_FLOAT),  TYPE(T_END) };
Type type_double[]      = { TYPE(T_DOUBLE), TYPE(T_END) };



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



const char* GetBasicTypeName (const Type* T)
/* Return a const name string of the basic type.
** Return "type" for unknown basic types.
*/
{
    switch (GetRawType (T)) {
    case T_TYPE_ENUM:       return "enum";
    case T_TYPE_FLOAT:      return "float";
    case T_TYPE_DOUBLE:     return "double";
    case T_TYPE_VOID:       return "void";
    case T_TYPE_STRUCT:     return "struct";
    case T_TYPE_UNION:      return "union";
    case T_TYPE_ARRAY:      return "array";
    case T_TYPE_PTR:        return "pointer";
    case T_TYPE_FUNC:       return "function";
    case T_TYPE_NONE:       /* FALLTHROUGH */
    default:                break;
    }
    if (IsClassInt (T)) {
        if (IsSignSigned (T)) {
            switch (GetRawType (T)) {
            case T_TYPE_CHAR:       return "signed char";
            case T_TYPE_SHORT:      return "short";
            case T_TYPE_INT:        return "int";
            case T_TYPE_LONG:       return "long";
            case T_TYPE_LONGLONG:   return "long long";
            default:
                return "signed integer";
            }
        } else if (IsSignUnsigned (T)) {
            switch (GetRawType (T)) {
            case T_TYPE_CHAR:       return "unsigned char";
            case T_TYPE_SHORT:      return "unsigned short";
            case T_TYPE_INT:        return "unsigned int";
            case T_TYPE_LONG:       return "unsigned long";
            case T_TYPE_LONGLONG:   return "unsigned long long";
            default:
                return "unsigned integer";
            }
        } else {
            switch (GetRawType (T)) {
            case T_TYPE_CHAR:       return "char";
            case T_TYPE_SHORT:      return "short";
            case T_TYPE_INT:        return "int";
            case T_TYPE_LONG:       return "long";
            case T_TYPE_LONGLONG:   return "long long";
            default:
                return "integer";
            }
        }
    }
    return "type";
}



unsigned TypeLen (const Type* T)
/* Return the length of the type string */
{
    const Type* Start = T;
    while (T->C != T_END) {
        ++T;
    }
    return T - Start;
}



Type* TypeCopy (Type* Dest, const Type* Src)
/* Copy a type string */
{
    Type* Orig = Dest;
    while (1) {
        *Dest = *Src;
        if (Src->C == T_END) {
            break;
        }
        Src++;
        Dest++;
    }
    return Orig;
}



Type* TypeDup (const Type* T)
/* Create a copy of the given type on the heap */
{
    unsigned Len = (TypeLen (T) + 1) * sizeof (Type);
    return memcpy (xmalloc (Len), T, Len);
}



Type* TypeAlloc (unsigned Len)
/* Allocate memory for a type string of length Len. Len *must* include the
** trailing T_END.
*/
{
    return xmalloc (Len * sizeof (Type));
}



void TypeFree (Type* T)
/* Free a type string */
{
    xfree (T);
}



int SignExtendChar (int C)
/* Do correct sign extension of a character */
{
    if (IS_Get (&SignedChars) && (C & 0x80) != 0) {
        return C | ~0xFF;
    } else {
        return C & 0xFF;
    }
}



TypeCode GetDefaultChar (void)
/* Return the default char type (signed/unsigned) depending on the settings */
{
    return IS_Get (&SignedChars)? T_SCHAR : T_UCHAR;
}



Type* GetCharArrayType (unsigned Len)
/* Return the type for a char array of the given length */
{
    /* Allocate memory for the type string */
    Type* T = TypeAlloc (3);    /* array/char/terminator */

    /* Fill the type string */
    T[0].C   = T_ARRAY;
    T[0].A.L = Len;             /* Array length is in the L attribute */
    T[1].C   = GetDefaultChar ();
    T[2].C   = T_END;

    /* Return the new type */
    return T;
}



Type* GetImplicitFuncType (void)
/* Return a type string for an inplicitly declared function */
{
    /* Get a new function descriptor */
    FuncDesc* F = NewFuncDesc ();

    /* Allocate memory for the type string */
    Type* T = TypeAlloc (3);    /* func/returns int/terminator */

    /* Prepare the function descriptor */
    F->Flags  = FD_EMPTY | FD_VARIADIC;
    F->SymTab = &EmptySymTab;
    F->TagTab = &EmptySymTab;

    /* Fill the type string */
    T[0].C   = T_FUNC | CodeAddrSizeQualifier ();
    T[0].A.P = F;
    T[1].C   = T_INT;
    T[2].C   = T_END;

    /* Return the new type */
    return T;
}



const Type* GetStructReplacementType (const Type* SType)
/* Get a replacement type for passing a struct/union in the primary register */
{
    const Type* NewType;
    /* If the size is less than or equal to that of a long, we will copy the
    ** struct using the primary register, otherwise we will use memcpy.
    */
    switch (SizeOf (SType)) {
        case 1:     NewType = type_uchar;   break;
        case 2:     NewType = type_uint;    break;
        case 3:     /* FALLTHROUGH */
        case 4:     NewType = type_ulong;   break;
        default:    NewType = SType;        break;
    }

    return NewType;
}



long GetIntegerTypeMin (const Type* Type)
/* Get the smallest possible value of the integer type.
** The type must have a known size.
*/
{
    if (SizeOf (Type) == 0) {
        Internal ("Incomplete type used in GetIntegerTypeMin");
    }

    if (IsSignSigned (Type)) {
        /* The smallest possible signed value of N-byte integer is -pow(2, 8*N-1) */
        return (long)((unsigned long)(-1L) << (CHAR_BITS * SizeOf (Type) - 1U));
    } else {
        return 0;
    }
}



unsigned long GetIntegerTypeMax (const Type* Type)
/* Get the largest possible value of the integer type.
** The type must have a known size.
*/
{
    if (SizeOf (Type) == 0) {
        Internal ("Incomplete type used in GetIntegerTypeMax");
    }

    if (IsSignSigned (Type)) {
        /* Min signed value of N-byte integer is pow(2, 8*N-1) - 1 */
        return (1UL << (CHAR_BITS * SizeOf (Type) - 1U)) - 1UL;
    } else {
        /* Max signed value of N-byte integer is pow(2, 8*N) - 1. However,
        ** workaround is needed as in ISO C it is UB if the shift count is
        ** equal to the bit width of the left operand type.
        */
        return (1UL << 1U << (CHAR_BITS * SizeOf (Type) - 1U)) - 1UL;
    }
}



static unsigned TypeOfBySize (const Type* Type)
/* Get the code generator replacement type of the object by its size */
{
    unsigned NewType;
    /* If the size is less than or equal to that of a a long, we will copy
    ** the struct using the primary register, otherwise we use memcpy.
    */
    switch (SizeOf (Type)) {
        case 1:     NewType = CF_CHAR;  break;
        case 2:     NewType = CF_INT;   break;
        case 3:     /* FALLTHROUGH */
        case 4:     NewType = CF_LONG;  break;
        default:    NewType = CF_NONE;  break;
    }

    return NewType;
}


Type* PointerTo (const Type* T)
/* Return a type string that is "pointer to T". The type string is allocated
** on the heap and may be freed after use.
*/
{
    /* Get the size of the type string including the terminator */
    unsigned Size = TypeLen (T) + 1;

    /* Allocate the new type string */
    Type* P = TypeAlloc (Size + 1);

    /* Create the return type... */
    P[0].C = T_PTR | (T[0].C & T_QUAL_ADDRSIZE);
    memcpy (P+1, T, Size * sizeof (Type));

    /* ...and return it */
    return P;
}



static TypeCode PrintTypeComp (FILE* F, TypeCode C, TypeCode Mask, const char* Name)
/* Check for a specific component of the type. If it is there, print the
** name and remove it. Return the type with the component removed.
*/
{
    if ((C & Mask) == Mask) {
        fprintf (F, "%s ", Name);
        C &= ~Mask;
    }
    return C;
}



void PrintType (FILE* F, const Type* T)
/* Output translation of type array. */
{
    /* Walk over the type string */
    while (T->C != T_END) {

        /* Get the type code */
        TypeCode C = T->C;

        /* Print any qualifiers */
        C = PrintTypeComp (F, C, T_QUAL_CONST, "const");
        C = PrintTypeComp (F, C, T_QUAL_VOLATILE, "volatile");
        C = PrintTypeComp (F, C, T_QUAL_RESTRICT, "restrict");
        C = PrintTypeComp (F, C, T_QUAL_NEAR, "__near__");
        C = PrintTypeComp (F, C, T_QUAL_FAR, "__far__");
        C = PrintTypeComp (F, C, T_QUAL_FASTCALL, "__fastcall__");
        C = PrintTypeComp (F, C, T_QUAL_CDECL, "__cdecl__");

        /* Signedness. Omit the signedness specifier for long and int */
        if ((C & T_MASK_TYPE) != T_TYPE_INT && (C & T_MASK_TYPE) != T_TYPE_LONG) {
            C = PrintTypeComp (F, C, T_SIGN_SIGNED, "signed");
        }
        C = PrintTypeComp (F, C, T_SIGN_UNSIGNED, "unsigned");

        /* Now check the real type */
        switch (C & T_MASK_TYPE) {
            case T_TYPE_CHAR:
                fprintf (F, "char");
                break;
            case T_TYPE_SHORT:
                fprintf (F, "short");
                break;
            case T_TYPE_INT:
                fprintf (F, "int");
                break;
            case T_TYPE_LONG:
                fprintf (F, "long");
                break;
            case T_TYPE_LONGLONG:
                fprintf (F, "long long");
                break;
            case T_TYPE_ENUM:
                fprintf (F, "enum");
                break;
            case T_TYPE_FLOAT:
                fprintf (F, "float");
                break;
            case T_TYPE_DOUBLE:
                fprintf (F, "double");
                break;
            case T_TYPE_VOID:
                fprintf (F, "void");
                break;
            case T_TYPE_STRUCT:
                fprintf (F, "struct %s", ((SymEntry*) T->A.P)->Name);
                break;
            case T_TYPE_UNION:
                fprintf (F, "union %s", ((SymEntry*) T->A.P)->Name);
                break;
            case T_TYPE_ARRAY:
                /* Recursive call */
                PrintType (F, T + 1);
                if (T->A.L == UNSPECIFIED) {
                    fprintf (F, " []");
                } else {
                    fprintf (F, " [%ld]", T->A.L);
                }
                return;
            case T_TYPE_PTR:
                /* Recursive call */
                PrintType (F, T + 1);
                fprintf (F, " *");
                return;
            case T_TYPE_FUNC:
                fprintf (F, "function returning ");
                break;
            default:
                fprintf (F, "unknown type: %04lX", T->C);
        }

        /* Next element */
        ++T;
    }
}



void PrintFuncSig (FILE* F, const char* Name, Type* T)
/* Print a function signature. */
{
    /* Get the function descriptor */
    const FuncDesc* D = GetFuncDesc (T);

    /* Print a comment with the function signature */
    PrintType (F, GetFuncReturn (T));
    if (IsQualNear (T)) {
        fprintf (F, " __near__");
    }
    if (IsQualFar (T)) {
        fprintf (F, " __far__");
    }
    if (IsQualFastcall (T)) {
        fprintf (F, " __fastcall__");
    }
    if (IsQualCDecl (T)) {
        fprintf (F, " __cdecl__");
    }
    fprintf (F, " %s (", Name);

    /* Parameters */
    if (D->Flags & FD_VOID_PARAM) {
        fprintf (F, "void");
    } else {
        unsigned I;
        SymEntry* E = D->SymTab->SymHead;
        for (I = 0; I < D->ParamCount; ++I) {
            if (I > 0) {
                fprintf (F, ", ");
            }
            if (SymIsRegVar (E)) {
                fprintf (F, "register ");
            }
            PrintType (F, E->Type);
            E = E->NextSym;
        }
    }

    /* End of parameter list */
    fprintf (F, ")");
}



void PrintRawType (FILE* F, const Type* T)
/* Print a type string in raw format (for debugging) */
{
    while (T->C != T_END) {
        fprintf (F, "%04lX ", T->C);
        ++T;
    }
    fprintf (F, "\n");
}



int TypeHasAttr (const Type* T)
/* Return true if the given type has attribute data */
{
    return IsClassStruct (T) || IsTypeArray (T) || IsClassFunc (T);
}



const Type* GetUnderlyingType (const Type* Type)
/* Get the underlying type of an enum or other integer class type */
{
    if (IsTypeEnum (Type)) {

        /* This should not happen, but just in case */
        if (Type->A.P == 0) {
            Internal ("Enum tag type error in GetUnderlyingTypeCode");
        }

        return ((SymEntry*)Type->A.P)->V.E.Type;
    }

    return Type;
}



TypeCode GetUnderlyingTypeCode (const Type* Type)
/* Get the type code of the unqualified underlying type of TCode.
** Return UnqualifiedType (TCode) if TCode is not scalar.
*/
{
    TypeCode Underlying = UnqualifiedType (Type->C);
    TypeCode TCode;

    /* We could also support other T_CLASS_INT types, but just enums for now */
    if (IsTypeEnum (Type)) {

        /* This should not happen, but just in case */
        if (Type->A.P == 0) {
            Internal ("Enum tag type error in GetUnderlyingTypeCode");
        }

        /* Inspect the underlying type of the enum */
        if (((SymEntry*)Type->A.P)->V.E.Type == 0) {
            /* Incomplete enum type is used */
            return Underlying;
        }
        TCode = UnqualifiedType (((SymEntry*)Type->A.P)->V.E.Type->C);

        /* Replace the type code with integer */
        Underlying = (TCode & ~T_MASK_TYPE);
        switch (TCode & T_MASK_SIZE) {
            case T_SIZE_INT:      Underlying |= T_TYPE_INT;      break;
            case T_SIZE_LONG:     Underlying |= T_TYPE_LONG;     break;
            case T_SIZE_SHORT:    Underlying |= T_TYPE_SHORT;    break;
            case T_SIZE_CHAR:     Underlying |= T_TYPE_CHAR;     break;
            case T_SIZE_LONGLONG: Underlying |= T_TYPE_LONGLONG; break;
            default:              Underlying |= T_TYPE_INT;      break;
        }
    }

    return Underlying;
}



unsigned SizeOf (const Type* T)
/* Compute size of object represented by type array. */
{
    switch (GetUnderlyingTypeCode (T)) {

        case T_VOID:
            /* A void variable is a cc65 extension.
            ** Get its size (in bytes).
            */
            return T->A.U;

        /* Beware: There's a chance that this triggers problems in other parts
           of the compiler. The solution is to fix the callers, because calling
           SizeOf() with a function type as argument is bad. */
        case T_FUNC:
            return 0;   /* Size of function is unknown */

        case T_SCHAR:
        case T_UCHAR:
            return SIZEOF_CHAR;

        case T_SHORT:
        case T_USHORT:
            return SIZEOF_SHORT;

        case T_INT:
        case T_UINT:
            return SIZEOF_INT;

        case T_PTR:
            return SIZEOF_PTR;

        case T_LONG:
        case T_ULONG:
            return SIZEOF_LONG;

        case T_LONGLONG:
        case T_ULONGLONG:
            return SIZEOF_LONGLONG;

        case T_FLOAT:
            return SIZEOF_FLOAT;

        case T_DOUBLE:
            return SIZEOF_DOUBLE;

        case T_STRUCT:
        case T_UNION:
            return ((SymEntry*) T->A.P)->V.S.Size;

        case T_ARRAY:
            if (T->A.L == UNSPECIFIED) {
                /* Array with unspecified size */
                return 0;
            } else {
                return T->A.U * SizeOf (T + 1);
            }

        case T_ENUM:
            /* Incomplete enum type */
            return 0;

        default:

            Internal ("Unknown type in SizeOf: %04lX", T->C);
            return 0;

    }
}



unsigned PSizeOf (const Type* T)
/* Compute size of pointer object. */
{
    /* We are expecting a pointer expression */
    CHECK (IsClassPtr (T));

    /* Skip the pointer or array token itself */
    return SizeOf (T + 1);
}



unsigned CheckedSizeOf (const Type* T)
/* Return the size of a data type. If the size is zero, emit an error and
** return some valid size instead (so the rest of the compiler doesn't have
** to work with invalid sizes).
*/
{
    unsigned Size = SizeOf (T);
    if (Size == 0) {
        Error ("Size of data type is unknown");
        Size = SIZEOF_CHAR;     /* Don't return zero */
    }
    return Size;
}



unsigned CheckedPSizeOf (const Type* T)
/* Return the size of a data type that is pointed to by a pointer. If the
** size is zero, emit an error and return some valid size instead (so the
** rest of the compiler doesn't have to work with invalid sizes).
*/
{
    unsigned Size = PSizeOf (T);
    if (Size == 0) {
        Error ("Size of data type is unknown");
        Size = SIZEOF_CHAR;     /* Don't return zero */
    }
    return Size;
}



unsigned TypeOf (const Type* T)
/* Get the code generator base type of the object */
{
    unsigned NewType;

    switch (GetUnderlyingTypeCode (T)) {

        case T_SCHAR:
            return CF_CHAR;

        case T_UCHAR:
            return CF_CHAR | CF_UNSIGNED;

        case T_SHORT:
        case T_INT:
            return CF_INT;

        case T_USHORT:
        case T_UINT:
        case T_PTR:
        case T_ARRAY:
            return CF_INT | CF_UNSIGNED;

        case T_LONG:
            return CF_LONG;

        case T_ULONG:
            return CF_LONG | CF_UNSIGNED;

        case T_FLOAT:
        case T_DOUBLE:
            /* These two are identical in the backend */
            return CF_FLOAT;

        case T_FUNC:
            /* Treat this as a function pointer */
            return CF_INT | CF_UNSIGNED;

        case T_STRUCT:
        case T_UNION:
            NewType = TypeOfBySize (T);
            if (NewType != CF_NONE) {
                return NewType;
            }
            /* Address of ... */
            return CF_INT | CF_UNSIGNED;

        default:
            Error ("Illegal type %04lX", T->C);
            return CF_INT;
    }
}



unsigned FuncTypeOf (const Type* T)
/* Get the code generator flag for calling the function */
{
    if (GetUnderlyingTypeCode (T) == T_FUNC) {
        return (((FuncDesc*) T->A.P)->Flags & FD_VARIADIC) ? 0 : CF_FIXARGC;
    } else {
        Error ("Illegal function type %04lX", T->C);
        return 0;
    }
}



Type* Indirect (Type* T)
/* Do one indirection for the given type, that is, return the type where the
** given type points to.
*/
{
    /* We are expecting a pointer expression */
    CHECK (IsClassPtr (T));

    /* Skip the pointer or array token itself */
    return T + 1;
}



Type* ArrayToPtr (Type* T)
/* Convert an array to a pointer to it's first element */
{
    /* Return pointer to first element */
    return PointerTo (GetElementType (T));
}



int IsVariadicFunc (const Type* T)
/* Return true if this is a function type or pointer to function type with
** variable parameter list
*/
{
    FuncDesc* F = GetFuncDesc (T);
    return (F->Flags & FD_VARIADIC) != 0;
}



FuncDesc* GetFuncDesc (const Type* T)
/* Get the FuncDesc pointer from a function or pointer-to-function type */
{
    if (UnqualifiedType (T->C) == T_PTR) {
        /* Pointer to function */
        ++T;
    }

    /* Be sure it's a function type */
    CHECK (IsClassFunc (T));

    /* Get the function descriptor from the type attributes */
    return T->A.P;
}



void SetFuncDesc (Type* T, FuncDesc* F)
/* Set the FuncDesc pointer in a function or pointer-to-function type */
{
    if (UnqualifiedType (T->C) == T_PTR) {
        /* Pointer to function */
        ++T;
    }

    /* Be sure it's a function type */
    CHECK (IsClassFunc (T));

    /* Set the function descriptor */
    T->A.P = F;
}



Type* GetFuncReturn (Type* T)
/* Return a pointer to the return type of a function or pointer-to-function type */
{
    if (UnqualifiedType (T->C) == T_PTR) {
        /* Pointer to function */
        ++T;
    }

    /* Be sure it's a function type */
    CHECK (IsClassFunc (T));

    /* Return a pointer to the return type */
    return T + 1;
}



long GetElementCount (const Type* T)
/* Get the element count of the array specified in T (which must be of
** array type).
*/
{
    CHECK (IsTypeArray (T));
    return T->A.L;
}



void SetElementCount (Type* T, long Count)
/* Set the element count of the array specified in T (which must be of
** array type).
*/
{
    CHECK (IsTypeArray (T));
    T->A.L = Count;
}



Type* GetElementType (Type* T)
/* Return the element type of the given array type. */
{
    CHECK (IsTypeArray (T));
    return T + 1;
}



Type* GetBaseElementType (Type* T)
/* Return the base element type of a given type. If T is not an array, this
** will return. Otherwise it will return the base element type, which means
** the element type that is not an array.
*/
{
    while (IsTypeArray (T)) {
        ++T;
    }
    return T;
}



SymEntry* GetSymEntry (const Type* T)
/* Return a SymEntry pointer from a type */
{
    /* Only enums, structs or unions have a SymEntry attribute */
    CHECK (IsClassStruct (T) || IsTypeEnum (T));

    /* Return the attribute */
    return T->A.P;
}



void SetSymEntry (Type* T, SymEntry* S)
/* Set the SymEntry pointer for a type */
{
    /* Only enums, structs or unions have a SymEntry attribute */
    CHECK (IsClassStruct (T) || IsTypeEnum (T));

    /* Set the attribute */
    T->A.P = S;
}



Type* IntPromotion (Type* T)
/* Apply the integer promotions to T and return the result. The returned type
** string may be T if there is no need to change it.
*/
{
    /* We must have an int to apply int promotions */
    PRECONDITION (IsClassInt (T));

    /* An integer can represent all values from either signed or unsigned char,
    ** so convert chars to int and leave all other types alone.
    */
    if (IsTypeChar (T)) {
        return type_int;
    } else {
        return T;
    }
}



Type* PtrConversion (Type* T)
/* If the type is a function, convert it to pointer to function. If the
** expression is an array, convert it to pointer to first element. Otherwise
** return T.
*/
{
    if (IsTypeFunc (T)) {
        return PointerTo (T);
    } else if (IsTypeArray (T)) {
        return ArrayToPtr (T);
    } else {
        return T;
    }
}



TypeCode AddrSizeQualifier (unsigned AddrSize)
/* Return T_QUAL_NEAR or T_QUAL_FAR depending on the address size */
{
    switch (AddrSize) {

        case ADDR_SIZE_ABS:
            return T_QUAL_NEAR;

        case ADDR_SIZE_FAR:
            return T_QUAL_FAR;

        default:
            Error ("Invalid address size");
            return T_QUAL_NEAR;

    }
}
