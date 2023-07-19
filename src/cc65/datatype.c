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
#include "ident.h"
#include "symtab.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Predefined type strings */
const Type type_char[]      = { TYPE(T_CHAR),   TYPE(T_END) };
const Type type_schar[]     = { TYPE(T_SCHAR),  TYPE(T_END) };
const Type type_uchar[]     = { TYPE(T_UCHAR),  TYPE(T_END) };
const Type type_int[]       = { TYPE(T_INT),    TYPE(T_END) };
const Type type_uint[]      = { TYPE(T_UINT),   TYPE(T_END) };
const Type type_long[]      = { TYPE(T_LONG),   TYPE(T_END) };
const Type type_ulong[]     = { TYPE(T_ULONG),  TYPE(T_END) };
const Type type_bool[]      = { TYPE(T_INT),    TYPE(T_END) };
const Type type_void[]      = { TYPE(T_VOID),   TYPE(T_END) };
const Type type_size_t[]    = { TYPE(T_SIZE_T), TYPE(T_END) };
const Type type_float[]     = { TYPE(T_FLOAT),  TYPE(T_END) };
const Type type_double[]    = { TYPE(T_DOUBLE), TYPE(T_END) };

/* More predefined type strings */
const Type type_char_p[]    = { TYPE(T_PTR),    TYPE(T_CHAR),   TYPE(T_END) };
const Type type_c_char_p[]  = { TYPE(T_PTR),    TYPE(T_C_CHAR), TYPE(T_END) };
const Type type_void_p[]    = { TYPE(T_PTR),    TYPE(T_VOID),   TYPE(T_END) };
const Type type_c_void_p[]  = { TYPE(T_PTR),    TYPE(T_C_VOID), TYPE(T_END) };



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



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



/*****************************************************************************/
/*                           Type info extraction                            */
/*****************************************************************************/



int SignExtendChar (int C)
/* Do correct sign extension of a character to an int */
{
    if (IS_Get (&SignedChars) && (C & 0x80) != 0) {
        return C | ~0xFF;
    } else {
        return C & 0xFF;
    }
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



unsigned BitSizeOf (const Type* T)
/* Return the size (in bit-width) of a data type */
{
    return IsTypeBitField (T) ? T->A.B.Width : CHAR_BITS * SizeOf (T);
}



unsigned SizeOf (const Type* T)
/* Compute size (in bytes) of object represented by type array */
{
    switch (GetUnqualTypeCode (T)) {

        case T_VOID:
            /* A void variable is a cc65 extension.
            ** Get its size (in bytes).
            */
            return T->A.U;

        /* Beware: There's a chance that this triggers problems in other parts
        ** of the compiler. The solution is to fix the callers, because calling
        ** SizeOf() with a function type as argument is bad.
        */
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
            return T->A.S->V.S.Size;

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
/* Compute size (in bytes) of pointee object */
{
    /* We are expecting a pointer expression */
    CHECK (IsClassPtr (T));

    /* Skip the pointer or array token itself */
    return SizeOf (T + 1);
}



unsigned CheckedBitSizeOf (const Type* T)
/* Return the size (in bit-width) of a data type. If the size is zero, emit an
** error and return some valid size instead (so the rest of the compiler
** doesn't have to work with invalid sizes).
*/
{
    return IsTypeBitField (T) ? T->A.B.Width : CHAR_BITS * CheckedSizeOf (T);
}



unsigned CheckedSizeOf (const Type* T)
/* Return the size (in bytes) of a data type. If the size is zero, emit an
** error and return some valid size instead (so the rest of the compiler
** doesn't have to work with invalid sizes).
*/
{
    unsigned Size = SizeOf (T);
    if (Size == 0) {
        if (HasUnknownSize (T + 1)) {
            Error ("Size of type '%s' is unknown", GetFullTypeName (T));
        } else {
            Error ("Size of type '%s' is 0", GetFullTypeName (T));
        }
        Size = SIZEOF_CHAR;     /* Don't return zero */
    }
    return Size;
}



unsigned CheckedPSizeOf (const Type* T)
/* Return the size (in bytes) of a data type that is pointed to by a pointer.
** If the size is zero, emit an error and return some valid size instead (so
** the rest of the compiler doesn't have to work with invalid sizes).
*/
{
    unsigned Size = PSizeOf (T);
    if (Size == 0) {
        if (HasUnknownSize (T + 1)) {
            Error ("Pointer to type '%s' of unknown size", GetFullTypeName (T + 1));
        } else {
            Error ("Pointer to type '%s' of 0 size", GetFullTypeName (T + 1));
        }
        Size = SIZEOF_CHAR;     /* Don't return zero */
    }
    return Size;
}



static unsigned GetMinimalTypeSizeByBitWidth (unsigned BitWidth)
/* Return the size of the smallest integer type that may have BitWidth bits */
{
    /* Since all integer types supported in cc65 for bit-fields have sizes that
    ** are powers of 2, we can just use this bit-twiddling trick.
    */
    unsigned V = (int)(BitWidth - 1U) / (int)CHAR_BITS;
    V |= V >> 1;
    V |= V >> 2;
    V |= V >> 4;
    V |= V >> 8;
    V |= V >> 16;

    /* Return the result size */
    return V + 1U;
}



TypeCode GetUnqualTypeCode (const Type* Type)
/* Get the type code of the unqualified underlying type of Type.
** Return GetUnqualRawTypeCode (Type) if Type is not scalar.
*/
{
    TypeCode Underlying = GetUnqualRawTypeCode (Type);

    if (IsDeclTypeChar (Type)) {

        return IS_Get (&SignedChars) ? T_SCHAR : T_UCHAR;

    } else if (IsTypeEnum (Type)) {
        TypeCode TCode;

        /* This should not happen, but just in case */
        if (Type->A.S == 0) {
            Internal ("Enum tag type error in GetUnqualTypeCode");
        }

        /* Inspect the underlying type of the enum */
        if (Type->A.S->V.E.Type == 0) {
            /* Incomplete enum type is used */
            return Underlying;
        }
        TCode = GetUnqualRawTypeCode (Type->A.S->V.E.Type);

        /* Replace the type code with integer */
        Underlying = (TCode & ~T_MASK_RANK);
        switch (TCode & T_MASK_SIZE) {
            case T_SIZE_INT:      Underlying |= T_RANK_INT;      break;
            case T_SIZE_LONG:     Underlying |= T_RANK_LONG;     break;
            case T_SIZE_SHORT:    Underlying |= T_RANK_SHORT;    break;
            case T_SIZE_CHAR:     Underlying |= T_RANK_CHAR;     break;
            case T_SIZE_LONGLONG: Underlying |= T_RANK_LONGLONG; break;
            default:              Underlying |= T_RANK_INT;      break;
        }
    } else if (IsTypeBitField (Type)) {
        /* We consider the smallest type that can represent all values of the
        ** bit-field, instead of the type used in the declaration, the truly
        ** underlying of the bit-field.
        */
        switch (GetMinimalTypeSizeByBitWidth (Type->A.B.Width)) {
            case SIZEOF_CHAR:     Underlying = T_CHAR;      break;
            case SIZEOF_INT:      Underlying = T_INT;       break;
            case SIZEOF_LONG:     Underlying = T_LONG;      break;
            case SIZEOF_LONGLONG: Underlying = T_LONGLONG;  break;
            default:              Underlying = T_INT;       break;
        }
        Underlying &= ~T_MASK_SIGN;
        Underlying |= Type->C & T_MASK_SIGN;
    }

    return Underlying;
}



/*****************************************************************************/
/*                             Type manipulation                             */
/*****************************************************************************/



Type* GetImplicitFuncType (void)
/* Return a type string for an implicitly declared function */
{
    /* Get a new function descriptor */
    FuncDesc* F = NewFuncDesc ();

    /* Allocate memory for the type string */
    Type* T = TypeAlloc (3);    /* func/returns int/terminator */

    /* Prepare the function descriptor */
    F->Flags  = FD_EMPTY;
    F->SymTab = &EmptySymTab;
    F->TagTab = &EmptySymTab;

    /* Fill the type string */
    T[0].C   = T_FUNC | CodeAddrSizeQualifier ();
    T[0].A.F = F;
    T[1].C   = T_INT;
    T[2].C   = T_END;

    /* Return the new type */
    return T;
}



Type* GetCharArrayType (unsigned Len)
/* Return the type for a char array of the given length */
{
    /* Allocate memory for the type string */
    Type* T = TypeAlloc (3);    /* array/char/terminator */

    /* Fill the type string */
    T[0].C   = T_ARRAY;
    T[0].A.L = Len;             /* Array length is in the L attribute */
    T[1].C   = T_CHAR;
    T[2].C   = T_END;

    /* Return the new type */
    return T;
}



Type* NewPointerTo (const Type* T)
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



Type* NewBitFieldOf (const Type* T, unsigned BitOffs, unsigned BitWidth)
/* Return a type string that is "T : BitWidth" aligned on BitOffs. The type
** string is allocated on the heap and may be freed after use.
*/
{
    Type* P;

    /* The type specifier must be integeral */
    CHECK (IsClassInt (T));

    /* Allocate the new type string */
    P = TypeAlloc (3);

    /* Create the return type... */
    P[0].C = IsSignSigned (T) ? T_SBITFIELD : T_UBITFIELD;
    P[0].C |= (T[0].C & T_QUAL_ADDRSIZE);
    P[0].A.B.Offs  = BitOffs;
    P[0].A.B.Width = BitWidth;

    /* Get the declaration type */
    memcpy (&P[1], GetUnderlyingType (T), sizeof (P[1]));

    /* Get done... */
    P[2].C = T_END;

    /* ...and return it */
    return P;
}



const Type* AddressOf (const Type* T)
/* Return a type string that is "address of T". The type string is allocated
** on the heap and may be freed after use.
*/
{
    /* Get the size of the type string including the terminator */
    unsigned Size = TypeLen (T) + 1;

    /* Allocate the new type string */
    Type* P = TypeAlloc (Size + 1);

    /* Create the return type... */
    P[0].C = T_PTR | (T[0].C & T_QUAL_ADDRSIZE) | T_QUAL_CONST;
    memcpy (P+1, T, Size * sizeof (Type));

    /* ...and return it */
    return P;
}



const Type* Indirect (const Type* T)
/* Do one indirection for the given type, that is, return the type where the
** given type points to.
*/
{
    /* We are expecting a pointer expression */
    CHECK (IsClassPtr (T));

    /* Skip the pointer or array token itself */
    return T + 1;
}



Type* ArrayToPtr (const Type* T)
/* Convert an array to a pointer to it's first element */
{
    /* Return pointer to first element */
    return NewPointerTo (GetElementType (T));
}



const Type* PtrConversion (const Type* T)
/* If the type is a function, convert it to pointer to function. If the
** expression is an array, convert it to pointer to first element. Otherwise
** return T.
*/
{
    if (IsTypeFunc (T)) {
        return AddressOf (T);
    } else if (IsTypeArray (T)) {
        return AddressOf (GetElementType (T));
    } else {
        return T;
    }
}



const Type* StdConversion (const Type* T)
/* If the type is a function, convert it to pointer to function. If the
** expression is an array, convert it to pointer to first element. If the
** type is an integer, do integeral promotion. Otherwise return T.
*/
{
    if (IsTypeFunc (T)) {
        return AddressOf (T);
    } else if (IsTypeArray (T)) {
        return AddressOf (GetElementType (T));
    } else if (IsClassInt (T)) {
        return IntPromotion (T);
    } else {
        return T;
    }
}



const Type* IntPromotion (const Type* T)
/* Apply the integer promotions to T and return the result. The returned type
** string may be T if there is no need to change it.
*/
{
    /* We must have an int to apply int promotions */
    PRECONDITION (IsClassInt (T));

    /* https://port70.net/~nsz/c/c89/c89-draft.html#3.2.1.1
    ** A char, a short int, or an int bit-field, or their signed or unsigned varieties, or
    ** an object that has enumeration type, may be used in an expression wherever an int or
    ** unsigned int may be used. If an int can represent all values of the original type,
    ** the value is converted to an int; otherwise it is converted to an unsigned int.
    ** These are called the integral promotions.
    */

    if (IsTypeBitField (T)) {
        /* As we now support long bit-fields, we need modified rules for them:
        ** - If an int can represent all values of the bit-field, the bit-field is converted
        **   to an int;
        ** - Otherwise, if an unsigned int can represent all values of the bit-field, the
        **   bit-field is converted to an unsigned int;
        ** - Otherwise, the bit-field will have its declared integer type.
        ** These rules are borrowed from C++ and seem to be consistent with GCC/Clang's.
        */
        if (T->A.B.Width > INT_BITS) {
            return IsSignUnsigned (T) ? type_ulong : type_long;
        }
        return T->A.B.Width == INT_BITS && IsSignUnsigned (T) ? type_uint : type_int;
    } else if (IsRankChar (T)) {
        /* An integer can represent all values from either signed or unsigned char, so convert
        ** chars to int.
        */
        return type_int;
    } else if (IsRankShort (T)) {
        /* An integer cannot represent all values from unsigned short, so convert unsigned short
        ** to unsigned int.
        */
        return IsSignUnsigned (T) ? type_uint : type_int;
    } else if (!IsIncompleteESUType (T)) {
        /* The type is a complete type not smaller than int, so leave it alone. */
        return T;
    } else {
        /* Otherwise, this is an incomplete enum, and there is expceted to be an error already.
        ** Assume int to avoid further errors.
        */
        return type_int;
    }
}



const Type* ArithmeticConvert (const Type* lhst, const Type* rhst)
/* Perform the usual arithmetic conversions for binary operators. */
{
    /* https://port70.net/~nsz/c/c89/c89-draft.html#3.2.1.5
    ** Many binary operators that expect operands of arithmetic type cause conversions and yield
    ** result types in a similar way. The purpose is to yield a common type, which is also the type
    ** of the result. This pattern is called the usual arithmetic conversions.
    */

    /* There are additional rules for floating point types that we don't bother with, since
    ** floating point types are not (yet) supported.
    ** The integral promotions are performed on both operands.
    */
    lhst = IntPromotion (lhst);
    rhst = IntPromotion (rhst);

    /* If either operand has type unsigned long int, the other operand is converted to
    ** unsigned long int.
    */
    if ((IsRankLong (lhst) && IsSignUnsigned (lhst)) ||
        (IsRankLong (rhst) && IsSignUnsigned (rhst))) {
        return type_ulong;
    }

    /* Otherwise, if one operand has type long int and the other has type unsigned int,
    ** if a long int can represent all values of an unsigned int, the operand of type unsigned int
    ** is converted to long int ; if a long int cannot represent all the values of an unsigned int,
    ** both operands are converted to unsigned long int.
    */
    if ((IsRankLong (lhst) && IsRankInt (rhst) && IsSignUnsigned (rhst)) ||
        (IsRankLong (rhst) && IsRankInt (lhst) && IsSignUnsigned (lhst))) {
        /* long can represent all unsigneds, so we are in the first sub-case. */
        return type_long;
    }

    /* Otherwise, if either operand has type long int, the other operand is converted to long int.
    */
    if (IsRankLong (lhst) || IsRankLong (rhst)) {
        return type_long;
    }

    /* Otherwise, if either operand has type unsigned int, the other operand is converted to
    ** unsigned int.
    */
    if ((IsRankInt (lhst) && IsSignUnsigned (lhst)) ||
        (IsRankInt (rhst) && IsSignUnsigned (rhst))) {
        return type_uint;
    }

    /* Otherwise, both operands have type int. */
    CHECK (IsRankInt (lhst));
    CHECK (IsSignSigned (lhst));
    CHECK (IsRankInt (rhst));
    CHECK (IsSignSigned (rhst));
    return type_int;
}



const Type* GetSignedType (const Type* T)
/* Get signed counterpart of the integral type */
{
    switch (GetUnqualTypeCode (T) & T_MASK_RANK) {
        case T_RANK_CHAR:
            return type_schar;

        case T_RANK_INT:
        case T_RANK_SHORT:
            return type_int;

        case T_RANK_LONG:
            return type_long;

        default:
            Internal ("Unknown type code: %lX", GetUnqualTypeCode (T));
            return T;
    }
}



const Type* GetUnsignedType (const Type* T)
/* Get unsigned counterpart of the integral type */
{
    switch (GetUnqualTypeCode (T) & T_MASK_RANK) {
        case T_RANK_CHAR:
            return type_uchar;

        case T_RANK_INT:
        case T_RANK_SHORT:
            return type_uint;

        case T_RANK_LONG:
            return type_ulong;

        default:
            Internal ("Unknown type code: %lX", GetUnqualTypeCode (T));
            return T;
    }
}



const Type* GetUnderlyingType (const Type* Type)
/* Get the underlying type of an enum or other integer class type */
{
    if (IsDeclTypeChar (Type)) {
        return IS_Get (&SignedChars) ? type_schar : type_uchar;
    } else if (IsTypeEnum (Type)) {
        /* This should not happen, but just in case */
        if (Type->A.S == 0) {
            Internal ("Enum tag type error in GetUnderlyingType");
        }

        /* If incomplete enum type is used, just return its raw type */
        if (Type->A.S->V.E.Type != 0) {
            return Type->A.S->V.E.Type;
        }
    } else if (IsTypeBitField (Type)) {
        /* We consider the smallest type that can represent all values of the
        ** bit-field, instead of the type used in the declaration, the truly
        ** underlying of the bit-field.
        */
        switch (GetMinimalTypeSizeByBitWidth (Type->A.B.Width)) {
            case SIZEOF_CHAR: Type = IsSignSigned (Type) ? type_schar : type_uchar; break;
            case SIZEOF_INT:  Type = IsSignSigned (Type) ? type_int   : type_uint;  break;
            case SIZEOF_LONG: Type = IsSignSigned (Type) ? type_long  : type_ulong; break;
            default:          Type = IsSignSigned (Type) ? type_int   : type_uint;  break;
        }
    }

    return Type;
}



const Type* GetStructReplacementType (const Type* SType)
/* Get a replacement type for passing a struct/union by value in the primary */
{
    const Type* NewType;
    /* If the size is less than or equal to that of a long, we will copy the
    ** struct using the primary register, otherwise we will use memcpy.
    */
    switch (SizeOf (SType)) {
        case 1:     NewType = type_uchar;   break;
        case 2:     NewType = type_uint;    break;
        case 4:     NewType = type_ulong;   break;
        default:    NewType = SType;        break;
    }

    return NewType;
}



const Type* GetBitFieldChunkType (const Type* Type)
/* Get the type needed to operate on the byte chunk containing the bit-field */
{
    unsigned ChunkSize;
    if ((Type->A.B.Width - 1U) / CHAR_BITS ==
        (Type->A.B.Offs + Type->A.B.Width - 1U) / CHAR_BITS) {
        /* T bit-field fits within its underlying type */
        return GetUnderlyingType (Type);
    }

    ChunkSize = GetMinimalTypeSizeByBitWidth (Type->A.B.Offs + Type->A.B.Width);
    if (ChunkSize < SizeOf (Type + 1)) {
        /* The end of the bit-field is offset by some bits so that it requires
        ** more bytes to be accessed as a whole than its underlying type does.
        ** Note: In cc65 the bit offset is always less than CHAR_BITS.
        */
        switch (ChunkSize) {
            case SIZEOF_CHAR: return IsSignSigned (Type) ? type_schar : type_uchar;
            case SIZEOF_INT:  return IsSignSigned (Type) ? type_int   : type_uint;
            case SIZEOF_LONG: return IsSignSigned (Type) ? type_long  : type_ulong;
            default:          return IsSignSigned (Type) ? type_int   : type_uint;
        }
    }

    /* We can always use the declarartion integer type as the chunk type.
    ** Note: A bit-field will not occupy bits located in bytes more than that
    ** of its declaration type in cc65. So this is OK.
    */
    return Type + 1;
}



/*****************************************************************************/
/*                              Type Predicates                              */
/*****************************************************************************/



int IsTypeFragBitField (const Type* T)
/* Return true if this is a bit-field that shares byte space with other fields */
{
    return IsTypeBitField (T) &&
           (T->A.B.Offs != 0 || T->A.B.Width != CHAR_BITS * SizeOf (T));
}



int IsObjectType (const Type* T)
/* Return true if this is a fully described object type */
{
    return !IsTypeFunc (T) && !IsIncompleteType (T);
}



int IsIncompleteType (const Type* T)
/* Return true if this is an object type lacking size info */
{
    if (IsTypeArray (T)) {
        return GetElementCount (T) == UNSPECIFIED || IsIncompleteType (T + 1);
    }
    return IsTypeVoid (T) || IsIncompleteESUType (T);
}



int IsArithmeticType (const Type* T)
/* Return true if this is an integer or floating type */
{
    return IsClassInt (T) || IsClassFloat (T);
}



int IsBasicType (const Type* T)
/* Return true if this is a character, integer or floating type */
{
    return IsDeclRankChar (T) || IsClassInt (T) || IsClassFloat (T);
}



int IsScalarType (const Type* T)
/* Return true if this is an arithmetic or pointer type */
{
    return IsArithmeticType (T) || IsTypePtr (T);
}



int IsDerivedType (const Type* T)
/* Return true if this is an array, struct, union, function or pointer type */
{
    return IsTypeArray (T) || IsClassStruct (T) || IsClassFunc (T) || IsTypePtr (T);
}



int IsAggregateType (const Type* T)
/* Return true if this is an array or struct type */
{
    return IsTypeArray (T) || IsTypeStruct (T);
}



int IsRelationType (const Type* T)
/* Return true if this is an arithmetic, array or pointer type */
{
    return IsArithmeticType (T) || IsClassPtr (T);
}



int IsCastType (const Type* T)
/* Return true if this type can be used for casting */
{
    return IsScalarType (T) || IsTypeVoid (T);
}



int IsESUType (const Type* T)
/* Return true if this is an enum/struct/union type */
{
    return IsClassStruct (T) || IsTypeEnum (T);
}



int IsIncompleteESUType (const Type* T)
/* Return true if this is an incomplete ESU type */
{
    SymEntry* TagSym = GetESUTagSym (T);

    return TagSym != 0 && !SymIsDef (TagSym);
}



int IsEmptiableObjectType (const Type* T)
/* Return true if this is a struct/union/void type that can have zero size */
{
    return IsClassStruct (T) || IsTypeVoid (T);
}



int HasUnknownSize (const Type* T)
/* Return true if this is an incomplete ESU type or an array of unknown size */
{
    if (IsTypeArray (T)) {
        return GetElementCount (T) == UNSPECIFIED || HasUnknownSize (T + 1);
    }
    return IsIncompleteESUType (T);
}



int TypeHasAttrData (const Type* T)
/* Return true if the given type has attribute data */
{
    return IsClassStruct (T) || IsTypeArray (T) || IsClassFunc (T);
}



/*****************************************************************************/
/*                             Qualifier helpers                             */
/*****************************************************************************/



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



/*****************************************************************************/
/*                           Function type helpers                           */
/*****************************************************************************/



int IsVariadicFunc (const Type* T)
/* Return true if this is a function type or pointer to function type with
** variable parameter list.
** Check fails if the type is not a function or a pointer to function.
*/
{
    return (GetFuncDesc (T)->Flags & FD_VARIADIC) != 0;
}



int IsFastcallFunc (const Type* T)
/* Return true if this is a function type or pointer to function type by
** __fastcall__ calling convention.
** Check fails if the type is not a function or a pointer to function.
*/
{
    if (GetUnqualRawTypeCode (T) == T_PTR) {
        /* Pointer to function */
        ++T;
    }
    return !IsVariadicFunc (T) && (AutoCDecl ? IsQualFastcall (T) : !IsQualCDecl (T));
}



FuncDesc* GetFuncDesc (const Type* T)
/* Get the FuncDesc pointer from a function or pointer-to-function type */
{
    if (GetUnqualRawTypeCode (T) == T_PTR) {
        /* Pointer to function */
        ++T;
    }

    /* Be sure it's a function type */
    CHECK (IsClassFunc (T));

    /* Get the function descriptor from the type attributes */
    return T->A.F;
}



void SetFuncDesc (Type* T, FuncDesc* F)
/* Set the FuncDesc pointer in a function or pointer-to-function type */
{
    if (GetUnqualRawTypeCode (T) == T_PTR) {
        /* Pointer to function */
        ++T;
    }

    /* Be sure it's a function type */
    CHECK (IsClassFunc (T));

    /* Set the function descriptor */
    T->A.F = F;
}



const Type* GetFuncReturnType (const Type* T)
/* Return a pointer to the return type of a function or pointer-to-function type */
{
    if (GetUnqualRawTypeCode (T) == T_PTR) {
        /* Pointer to function */
        ++T;
    }

    /* Be sure it's a function type */
    CHECK (IsClassFunc (T));

    /* Return a pointer to the return type */
    return T + 1;
}



Type* GetFuncReturnTypeModifiable (Type* T)
/* Return a non-const pointer to the return type of a function or pointer-to-function type */
{
    if (GetUnqualRawTypeCode (T) == T_PTR) {
        /* Pointer to function */
        ++T;
    }

    /* Be sure it's a function type */
    CHECK (IsClassFunc (T));

    /* Return a pointer to the return type */
    return T + 1;
}



const FuncDesc* GetFuncDefinitionDesc (const Type* T)
/* Get the function descriptor of the function definition */
{
    const FuncDesc* D;

    /* Be sure it's a function type */
    CHECK (IsClassFunc (T));

    D = GetFuncDesc (T);
    return D->FuncDef != 0 ? D->FuncDef : D;
}



/*****************************************************************************/
/*                            Array type helpers                             */
/*****************************************************************************/



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



const Type* GetElementType (const Type* T)
/* Return the element type of the given array type */
{
    CHECK (IsTypeArray (T));
    return T + 1;
}



Type* GetElementTypeModifiable (Type* T)
/* Return the element type of the given array type */
{
    CHECK (IsTypeArray (T));
    return T + 1;
}



const Type* GetBaseElementType (const Type* T)
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



/*****************************************************************************/
/*                             ESU types helpers                             */
/*****************************************************************************/



struct SymEntry* GetESUTagSym (const Type* T)
/* Get the tag symbol entry of the enum/struct/union type.
** Return 0 if it is not an enum/struct/union.
*/
{
    if ((IsClassStruct (T) || IsTypeEnum (T))) {
        return T->A.S;
    }
    return 0;
}



void SetESUTagSym (Type* T, struct SymEntry* S)
/* Set the tag symbol entry of the enum/struct/union type */
{
    /* Only enums, structs or unions have a SymEntry attribute */
    CHECK (IsClassStruct (T) || IsTypeEnum (T));

    /* Set the attribute */
    T->A.S = S;
}



/*****************************************************************************/
/*                                  Helpers                                  */
/*****************************************************************************/



const char* GetBasicTypeName (const Type* T)
/* Return a const name string of the basic type.
** Return "type" for unknown basic types.
*/
{
    switch (GetRawTypeRank (T)) {
    case T_RANK_ENUM:       return "enum";
    case T_RANK_BITFIELD:   return "bit-field";
    case T_RANK_FLOAT:      return "float";
    case T_RANK_DOUBLE:     return "double";
    case T_RANK_VOID:       return "void";
    case T_RANK_STRUCT:     return "struct";
    case T_RANK_UNION:      return "union";
    case T_RANK_ARRAY:      return "array";
    case T_RANK_PTR:        return "pointer";
    case T_RANK_FUNC:       return "function";
    case T_RANK_NONE:       /* FALLTHROUGH */
    default:                break;
    }
    if (IsClassInt (T)) {
        if (IsRawSignSigned (T)) {
            switch (GetRawTypeRank (T)) {
            case T_RANK_CHAR:       return "signed char";
            case T_RANK_SHORT:      return "short";
            case T_RANK_INT:        return "int";
            case T_RANK_LONG:       return "long";
            case T_RANK_LONGLONG:   return "long long";
            default:
                return "signed integer";
            }
        } else if (IsRawSignUnsigned (T)) {
            switch (GetRawTypeRank (T)) {
            case T_RANK_CHAR:       return "unsigned char";
            case T_RANK_SHORT:      return "unsigned short";
            case T_RANK_INT:        return "unsigned int";
            case T_RANK_LONG:       return "unsigned long";
            case T_RANK_LONGLONG:   return "unsigned long long";
            default:
                return "unsigned integer";
            }
        } else {
            switch (GetRawTypeRank (T)) {
            case T_RANK_CHAR:       return "char";
            case T_RANK_SHORT:      return "short";
            case T_RANK_INT:        return "int";
            case T_RANK_LONG:       return "long";
            case T_RANK_LONGLONG:   return "long long";
            default:
                return "integer";
            }
        }
    }
    return "type";
}



static const char* GetTagSymName (const Type* T)
/* Return a name string of the type or the symbol name if it is an ESU type.
** Note: This may use a static buffer that could be overwritten by other calls.
*/
{
    static char TypeName [IDENTSIZE + 16];
    SymEntry* Sym;

    Sym = GetESUTagSym (T);
    if (Sym == 0) {
        return GetBasicTypeName (T);
    }
    sprintf (TypeName, "%s %s", GetBasicTypeName (T),
             Sym->Name[0] != '\0' ? Sym->Name : "<unknown>");

    return TypeName;
}



const char* GetFullTypeName (const Type* T)
/* Return the full name string of the given type */
{
    struct StrBuf* Buf = NewDiagnosticStrBuf ();
    GetFullTypeNameBuf (Buf, T);

    return SB_GetConstBuf (Buf);
}



static struct StrBuf* GetFullTypeNameWestEast (struct StrBuf* West, struct StrBuf* East, const Type* T)
/* Return the name string of the given type split into a western part and an
** eastern part.
*/
{
    struct StrBuf Buf = AUTO_STRBUF_INITIALIZER;

    if (IsTypeArray (T)) {

        long Count = GetElementCount (T);
        if (!SB_IsEmpty (East)) {
            if (Count > 0) {
                SB_Printf (&Buf, "[%ld]", Count);
            } else {
                SB_Printf (&Buf, "[]");
            }
            SB_Append (East, &Buf);
            SB_Terminate (East);

        } else {
            if (Count > 0) {
                SB_Printf (East, "[%ld]", Count);
            } else {
                SB_Printf (East, "[]");
            }

            if (!SB_IsEmpty (West)) {
                /* Add parentheses to West */
                SB_Printf (&Buf, "(%s)", SB_GetConstBuf (West));
                SB_Copy (West, &Buf);
                SB_Terminate (West);
            }
        }

        /* Get element type */
        GetFullTypeNameWestEast (West, East, T + 1);

    } else if (IsTypeFunc (T)) {

        FuncDesc* D             = GetFuncDesc (T);
        struct StrBuf ParamList = AUTO_STRBUF_INITIALIZER;

        /* First argument */
        SymEntry* Param = D->SymTab->SymHead;
        unsigned I;
        for (I = 0; I < D->ParamCount; ++I) {
            CHECK (Param != 0 && (Param->Flags & SC_PARAM) != 0);
            if (I > 0) {
                SB_AppendStr (&ParamList, ", ");
            }
            SB_AppendStr (&ParamList, SB_GetConstBuf (GetFullTypeNameBuf (&Buf, Param->Type)));
            SB_Clear (&Buf);
            /* Next argument */
            Param = Param->NextSym;
        }
        if ((D->Flags & FD_VARIADIC) == 0) {
            if (D->ParamCount == 0 && (D->Flags & FD_EMPTY) == 0) {
                SB_AppendStr (&ParamList, "void");
            }
        } else {
            if (D->ParamCount > 0) {
                SB_AppendStr (&ParamList, ", ...");
            } else {
                SB_AppendStr (&ParamList, "...");
            }
        }
        SB_Terminate (&ParamList);

        /* Join the existing West and East together */
        if (!SB_IsEmpty (East)) {
            SB_Append (West, East);
            SB_Terminate (West);
            SB_Clear (East);
        }

        if (SB_IsEmpty (West)) {
            /* Just use the param list */
            SB_Printf (West, "(%s)", SB_GetConstBuf (&ParamList));
        } else {
            /* Append the param list to the existing West */
            SB_Printf (&Buf, "(%s)(%s)", SB_GetConstBuf (West), SB_GetConstBuf (&ParamList));
            SB_Printf (West, "%s", SB_GetConstBuf (&Buf));
        }
        SB_Done (&ParamList);

        /* Return type */
        GetFullTypeNameWestEast (West, East, T + 1);

    } else if (IsTypePtr (T)) {

        int QualCount = 0;

        SB_Printf (&Buf, "*");

        /* Add qualifiers */
        if ((GetQualifier (T) & ~T_QUAL_NEAR) != T_QUAL_NONE) {
            QualCount = GetQualifierTypeCodeNameBuf (&Buf, T->C, T_QUAL_NEAR);
        }

        if (!SB_IsEmpty (West)) {
            if (QualCount > 0) {
                SB_AppendChar (&Buf, ' ');
            }
            SB_Append (&Buf, West);
        }

        SB_Copy (West, &Buf);
        SB_Terminate (West);

        /* Get indirection type */
        GetFullTypeNameWestEast (West, East, T + 1);

    } else {

        /* Add qualifiers */
        if ((GetQualifier (T) & ~T_QUAL_NEAR) != 0) {
            if (GetQualifierTypeCodeNameBuf (&Buf, T->C, T_QUAL_NEAR) > 0) {
                SB_AppendChar (&Buf, ' ');
            }
        }

        if (!IsTypeBitField (T)) {
            SB_AppendStr (&Buf, GetTagSymName (T));
        } else {
            SB_AppendStr (&Buf, GetBasicTypeName (T + 1));
        }

        if (!SB_IsEmpty (West)) {
            SB_AppendChar (&Buf, ' ');
            SB_Append (&Buf, West);
        }

        SB_Copy (West, &Buf);
        SB_Terminate (West);
    }

    SB_Done (&Buf);
    return West;
}



struct StrBuf* GetFullTypeNameBuf (struct StrBuf* S, const Type* T)
/* Return the full name string of the given type */
{
    struct StrBuf East = AUTO_STRBUF_INITIALIZER;
    GetFullTypeNameWestEast (S, &East, T);

    /* Join West and East */
    SB_Append (S, &East);
    SB_Terminate (S);
    SB_Done (&East);

    return S;
}



int GetQualifierTypeCodeNameBuf (struct StrBuf* S, TypeCode Qual, TypeCode IgnoredQual)
/* Return the names of the qualifiers of the type.
** Qualifiers to be ignored can be specified with the IgnoredQual flags.
** Return the count of added qualifier names.
*/
{
    int Count = 0;

    Qual &= T_MASK_QUAL & ~IgnoredQual;
    if (Qual & T_QUAL_CONST) {
        if (!SB_IsEmpty (S)) {
            SB_AppendChar (S, ' ');
        }
        SB_AppendStr (S, "const");
        ++Count;
    }
    if (Qual & T_QUAL_VOLATILE) {
        if (Count > 0) {
            SB_AppendChar (S, ' ');
        }
        SB_AppendStr (S, "volatile");
        ++Count;
    }
    if (Qual & T_QUAL_RESTRICT) {
        if (Count > 0) {
            SB_AppendChar (S, ' ');
        }
        SB_AppendStr (S, "restrict");
        ++Count;
    }
    if (Qual & T_QUAL_NEAR) {
        if (Count > 0) {
            SB_AppendChar (S, ' ');
        }
        SB_AppendStr (S, "__near__");
        ++Count;
    }
    if (Qual & T_QUAL_FAR) {
        SB_AppendStr (S, "__far__");
        ++Count;
    }
    if (Qual & T_QUAL_FASTCALL) {
        if (Count > 0) {
            SB_AppendChar (S, ' ');
        }
        SB_AppendStr (S, "__fastcall__");
        ++Count;
    }
    if (Qual & T_QUAL_CDECL) {
        if (Count > 0) {
            SB_AppendChar (S, ' ');
        }
        SB_AppendStr (S, "__cdecl__");
        ++Count;
    }

    if (Count > 0) {
        SB_Terminate (S);
    }

    return Count;
}



void PrintType (FILE* F, const Type* T)
/* Print fulle name of the type */
{
    StrBuf Buf = AUTO_STRBUF_INITIALIZER;
    fprintf (F, "%s", SB_GetConstBuf (GetFullTypeNameBuf (&Buf, T)));
    SB_Done (&Buf);
}



void PrintFuncSig (FILE* F, const char* Name, const Type* T)
/* Print a function signature */
{
    StrBuf Buf       = AUTO_STRBUF_INITIALIZER;
    StrBuf ParamList = AUTO_STRBUF_INITIALIZER;
    StrBuf East      = AUTO_STRBUF_INITIALIZER;
    StrBuf West      = AUTO_STRBUF_INITIALIZER;

    /* Get the function descriptor used in definition */
    const FuncDesc* D = GetFuncDefinitionDesc (T);

    /* Get the parameter list string. Start from the first parameter */
    SymEntry* Param = D->SymTab->SymHead;
    unsigned I;
    for (I = 0; I < D->ParamCount; ++I) {
        CHECK (Param != 0 && (Param->Flags & SC_PARAM) != 0);
        if (I > 0) {
            SB_AppendStr (&ParamList, ", ");
        }
        if (SymIsRegVar (Param)) {
            SB_AppendStr (&ParamList, "register ");
        }
        if (!SymHasAnonName (Param)) {
            SB_AppendStr (&Buf, Param->Name);
        }
        SB_AppendStr (&ParamList, SB_GetConstBuf (GetFullTypeNameBuf (&Buf, Param->Type)));
        SB_Clear (&Buf);
        /* Next argument */
        Param = Param->NextSym;
    }
    if ((D->Flags & FD_VARIADIC) == 0) {
        if (D->ParamCount == 0 && (D->Flags & FD_EMPTY) == 0) {
            SB_AppendStr (&ParamList, "void");
        }
    } else {
        if (D->ParamCount > 0) {
            SB_AppendStr (&ParamList, ", ...");
        } else {
            SB_AppendStr (&ParamList, "...");
        }
    }
    SB_Terminate (&ParamList);

    /* Get the function qualifiers */
    if (GetQualifierTypeCodeNameBuf (&Buf, T->C, T_QUAL_NONE) > 0) {
        /* Append a space between the qualifiers and the name */
        SB_AppendChar (&Buf, ' ');
    }
    SB_Terminate (&Buf);

    /* Get the signature string without the return type */
    SB_Printf (&West, "%s%s (%s)", SB_GetConstBuf (&Buf), Name, SB_GetConstBuf (&ParamList));
    SB_Done (&Buf);
    SB_Done (&ParamList);

    /* Complete with the return type */
    GetFullTypeNameWestEast (&West, &East, GetFuncReturnType (T));
    SB_Append (&West, &East);
    SB_Terminate (&West);

    /* Output */
    fprintf (F, "%s", SB_GetConstBuf (&West));
    SB_Done (&East);
    SB_Done (&West);
}



void PrintRawType (FILE* F, const Type* T)
/* Print a type string in raw hex format (for debugging) */
{
    while (T->C != T_END) {
        fprintf (F, "%04lX ", T->C);
        ++T;
    }
    fprintf (F, "\n");
}
