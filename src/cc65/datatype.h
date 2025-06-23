/*****************************************************************************/
/*                                                                           */
/*                                datatype.h                                 */
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



#ifndef DATATYPE_H
#define DATATYPE_H



#include <stdio.h>

/* common */
#include "attrib.h"
#include "inline.h"
#include "mmodel.h"

/* cc65 */
#include "funcdesc.h"



/*****************************************************************************/
/*                           Forward declarations                            */
/*****************************************************************************/



struct StrBuf;
struct SymEntry;



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Basic data types */
enum {
    T_END           = 0x000000,

    /* Basic type ranks */
    T_RANK_NONE     = 0x000000,
    T_RANK_CHAR     = 0x000001,
    T_RANK_SHORT    = 0x000002,
    T_RANK_INT      = 0x000003,
    T_RANK_LONG     = 0x000004,
    T_RANK_LONGLONG = 0x000005,
    T_RANK_ENUM     = 0x000008,
    T_RANK_BITFIELD = 0x000009,
    T_RANK_FLOAT    = 0x00000A,
    T_RANK_DOUBLE   = 0x00000B,
    T_RANK_VOID     = 0x000010,
    T_RANK_STRUCT   = 0x000011,
    T_RANK_UNION    = 0x000012,
    T_RANK_ARRAY    = 0x000018,
    T_RANK_PTR      = 0x000019,
    T_RANK_FUNC     = 0x00001A,
    T_MASK_RANK     = 0x00001F,

    /* Type classes */
    T_CLASS_NONE    = 0x000000,
    T_CLASS_INT     = 0x000020,
    T_CLASS_FLOAT   = 0x000040,
    T_CLASS_PTR     = 0x000060,
    T_CLASS_STRUCT  = 0x000080,
    T_CLASS_FUNC    = 0x0000A0,
    T_MASK_CLASS    = 0x0000E0,

    /* Type signedness */
    T_SIGN_NONE     = 0x000000,
    T_SIGN_UNSIGNED = 0x000100,
    T_SIGN_SIGNED   = 0x000200,
    T_MASK_SIGN     = 0x000300,

    /* Type size modifiers */
    T_SIZE_NONE     = 0x000000,
    T_SIZE_CHAR     = 0x001000,
    T_SIZE_SHORT    = 0x002000,
    T_SIZE_INT      = 0x003000,
    T_SIZE_LONG     = 0x004000,
    T_SIZE_LONGLONG = 0x005000,
    T_MASK_SIZE     = 0x00F000,

    /* Type qualifiers */
    T_QUAL_NONE     = 0x000000,
    T_QUAL_CONST    = 0x010000,
    T_QUAL_VOLATILE = 0x020000,
    T_QUAL_RESTRICT = 0x040000,
    T_QUAL_CVR      = T_QUAL_CONST | T_QUAL_VOLATILE | T_QUAL_RESTRICT,
    T_QUAL_NEAR     = 0x080000,
    T_QUAL_FAR      = 0x100000,
    T_QUAL_ADDRSIZE = T_QUAL_NEAR | T_QUAL_FAR,
    T_QUAL_FASTCALL = 0x200000,
    T_QUAL_CDECL    = 0x400000,
    T_QUAL_CCONV    = T_QUAL_FASTCALL | T_QUAL_CDECL,
    T_MASK_QUAL     = 0x7F0000,

    /* Types */
    T_CHAR      = T_RANK_CHAR     | T_CLASS_INT    | T_SIGN_NONE     | T_SIZE_CHAR,
    T_SCHAR     = T_RANK_CHAR     | T_CLASS_INT    | T_SIGN_SIGNED   | T_SIZE_CHAR,
    T_UCHAR     = T_RANK_CHAR     | T_CLASS_INT    | T_SIGN_UNSIGNED | T_SIZE_CHAR,
    T_SHORT     = T_RANK_SHORT    | T_CLASS_INT    | T_SIGN_SIGNED   | T_SIZE_SHORT,
    T_USHORT    = T_RANK_SHORT    | T_CLASS_INT    | T_SIGN_UNSIGNED | T_SIZE_SHORT,
    T_INT       = T_RANK_INT      | T_CLASS_INT    | T_SIGN_SIGNED   | T_SIZE_INT,
    T_UINT      = T_RANK_INT      | T_CLASS_INT    | T_SIGN_UNSIGNED | T_SIZE_INT,
    T_LONG      = T_RANK_LONG     | T_CLASS_INT    | T_SIGN_SIGNED   | T_SIZE_LONG,
    T_ULONG     = T_RANK_LONG     | T_CLASS_INT    | T_SIGN_UNSIGNED | T_SIZE_LONG,
    T_LONGLONG  = T_RANK_LONGLONG | T_CLASS_INT    | T_SIGN_SIGNED   | T_SIZE_LONGLONG,
    T_ULONGLONG = T_RANK_LONGLONG | T_CLASS_INT    | T_SIGN_UNSIGNED | T_SIZE_LONGLONG,
    T_ENUM      = T_RANK_ENUM     | T_CLASS_INT    | T_SIGN_NONE     | T_SIZE_NONE,
    T_SBITFIELD = T_RANK_BITFIELD | T_CLASS_INT    | T_SIGN_SIGNED   | T_SIZE_NONE,
    T_UBITFIELD = T_RANK_BITFIELD | T_CLASS_INT    | T_SIGN_UNSIGNED | T_SIZE_NONE,
    T_FLOAT     = T_RANK_FLOAT    | T_CLASS_FLOAT  | T_SIGN_NONE     | T_SIZE_NONE,
    T_DOUBLE    = T_RANK_DOUBLE   | T_CLASS_FLOAT  | T_SIGN_NONE     | T_SIZE_NONE,
    T_VOID      = T_RANK_VOID     | T_CLASS_NONE   | T_SIGN_NONE     | T_SIZE_NONE,
    T_STRUCT    = T_RANK_STRUCT   | T_CLASS_STRUCT | T_SIGN_NONE     | T_SIZE_NONE,
    T_UNION     = T_RANK_UNION    | T_CLASS_STRUCT | T_SIGN_NONE     | T_SIZE_NONE,
    T_ARRAY     = T_RANK_ARRAY    | T_CLASS_PTR    | T_SIGN_NONE     | T_SIZE_NONE,
    T_PTR       = T_RANK_PTR      | T_CLASS_PTR    | T_SIGN_NONE     | T_SIZE_NONE,
    T_FUNC      = T_RANK_FUNC     | T_CLASS_FUNC   | T_SIGN_NONE     | T_SIZE_NONE,

    /* More types for convenience */
    T_C_CHAR    = T_CHAR | T_QUAL_CONST,
    T_C_VOID    = T_VOID | T_QUAL_CONST,

    /* Aliases */
    T_SIZE_T    = T_UINT,
};



/* Type code entry */
typedef unsigned long TypeCode;

/* Type entry */
typedef struct Type Type;
struct Type {
    TypeCode             C;     /* Code for this entry */
    union {
        struct FuncDesc* F;     /* Function description pointer */
        struct SymEntry* S;     /* Enum/struct/union tag symbol entry pointer */
        long             L;     /* Numeric attribute value */
        unsigned long    U;     /* Dito, unsigned */
        struct {
            unsigned     Offs;  /* Bit offset into storage unit */
            unsigned     Width; /* Width in bits */
        } B;                    /* Data for bit fields */
    } A;                        /* Type attribute if necessary */
};

/* A macro that expands to a full initializer for struct Type */
#define TYPE(T)         { (T), { 0 } }

/* Maximum length of a type string */
#define MAXTYPELEN      30

/* Special encodings for element counts of an array */
#define UNSPECIFIED     -1L     /* Element count was not specified */
#define FLEXIBLE        0L      /* Flexible array struct member */

/* Sizes. Floating point sizes come from fp.h */
#define SIZEOF_CHAR     1U
#define SIZEOF_SHORT    2U
#define SIZEOF_INT      2U
#define SIZEOF_LONG     4U
#define SIZEOF_LONGLONG 8U
#define SIZEOF_FLOAT    (FP_F_Size())
#define SIZEOF_DOUBLE   (FP_D_Size())
#define SIZEOF_PTR      SIZEOF_INT

/* Bit sizes */
#define CHAR_BITS       (8 * SIZEOF_CHAR)
#define SHORT_BITS      (8 * SIZEOF_SHORT)
#define INT_BITS        (8 * SIZEOF_INT)
#define LONG_BITS       (8 * SIZEOF_LONG)
#define LONGLONG_BITS   (8 * SIZEOF_LONGLONG)
#define FLOAT_BITS      (8 * SIZEOF_FLOAT)
#define DOUBLE_BITS     (8 * SIZEOF_DOUBLE)
#define PTR_BITS        (8 * SIZEOF_PTR)

/* Predefined type strings */
extern const Type type_char[];
extern const Type type_schar[];
extern const Type type_uchar[];
extern const Type type_short[];
extern const Type type_ushort[];
extern const Type type_int[];
extern const Type type_uint[];
extern const Type type_long[];
extern const Type type_ulong[];
extern const Type type_bool[];
extern const Type type_void[];
extern const Type type_size_t[];
extern const Type type_float[];
extern const Type type_double[];

/* More predefined type strings */
extern const Type type_char_p[];
extern const Type type_c_char_p[];
extern const Type type_void_p[];
extern const Type type_c_void_p[];



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



unsigned TypeLen (const Type* T);
/* Return the length of the type string */

Type* TypeCopy (Type* Dest, const Type* Src);
/* Copy a type string */

Type* TypeDup (const Type* T);
/* Create a copy of the given type on the heap */

Type* TypeAlloc (unsigned Len);
/* Allocate memory for a type string of length Len. Len *must* include the
** trailing T_END.
*/

void TypeFree (Type* T);
/* Free a type string */

#if defined(HAVE_INLINE)
INLINE void CopyTypeAttr (const Type* Src, Type* Dest)
/* Copy attribute data from Src to Dest */
{
    Dest->A = Src->A;
}
#else
#  define CopyTypeAttr(Src, Dest)       ((Dest)->A = (Src)->A)
#endif



/*****************************************************************************/
/*                           Type info extraction                            */
/*****************************************************************************/



int SignExtendChar (int C);
/* Do correct sign extension of a character to an int */

long GetIntegerTypeMin (const Type* Type);
/* Get the smallest possible value of the integer type.
** The type must have a known size.
*/

unsigned long GetIntegerTypeMax (const Type* Type);
/* Get the largest possible value of the integer type.
** The type must have a known size.
*/

unsigned BitSizeOf (const Type* T);
/* Return the size (in bit-width) of a data type */

unsigned SizeOf (const Type* T);
/* Compute size (in bytes) of object represented by type array */

unsigned PSizeOf (const Type* T);
/* Compute size (in bytes) of pointee object */

unsigned CheckedBitSizeOf (const Type* T);
/* Return the size (in bit-width) of a data type. If the size is zero, emit an
** error and return some valid size instead (so the rest of the compiler
** doesn't have to work with invalid sizes).
*/

unsigned CheckedSizeOf (const Type* T);
/* Return the size (in bytes) of a data type. If the size is zero, emit an
** error and return some valid size instead (so the rest of the compiler
** doesn't have to work with invalid sizes).
*/

unsigned CheckedPSizeOf (const Type* T);
/* Return the size (in bytes) of a data type that is pointed to by a pointer.
** If the size is zero, emit an error and return some valid size instead (so
** the rest of the compiler doesn't have to work with invalid sizes).
*/

#if defined(HAVE_INLINE)
INLINE TypeCode GetQualifier (const Type* T)
/* Get the qualifier from the given type. This doesn't have a "raw" version
** since an underlying type can never be qualified.
*/
{
    return (T->C & T_MASK_QUAL);
}
#else
#  define GetQualifier(T)       ((T)->C & T_MASK_QUAL)
#endif

TypeCode GetUnderlyingTypeCode (const Type* Type);
/* Get the type code of the unqualified underlying type of Type.
** Return GetUnqualRawTypeCode (Type) if Type is not scalar.
*/

#if defined(HAVE_INLINE)
INLINE TypeCode GetUnqualRawTypeCode (const Type* T)
/* Return the unqualified raw type code */
{
    return (T->C & ~T_MASK_QUAL);
}
#else
#  define GetUnqualRawTypeCode(T)   ((T)->C & ~T_MASK_QUAL)
#endif

#if defined(HAVE_INLINE)
INLINE TypeCode GetTypeClass (const Type* T)
/* Get the class of a type. This doesn't have a "raw" version since an
** underlying type can never be in a different class.
*/
{
    return (T->C & T_MASK_CLASS);
}
#else
#  define GetTypeClass(T)       ((T)->C & T_MASK_CLASS)
#endif

#if defined(HAVE_INLINE)
INLINE TypeCode GetTypeRank (const Type* T)
/* Get the type rank of a type */
{
    return (GetUnderlyingTypeCode (T) & T_MASK_RANK);
}
#else
#  define GetTypeRank(T)        (GetUnderlyingTypeCode (T) & T_MASK_RANK)
#endif

#if defined(HAVE_INLINE)
INLINE TypeCode GetSignedness (const Type* T)
/* Get the signedness of a type */
{
    return (GetUnderlyingTypeCode (T) & T_MASK_SIGN);
}
#else
#  define GetSignedness(T)      (GetUnderlyingTypeCode (T) & T_MASK_SIGN)
#endif

#if defined(HAVE_INLINE)
INLINE TypeCode GetSizeModifier (const Type* T)
/* Get the size modifier of a type */
{
    return (GetUnderlyingTypeCode (T) & T_MASK_SIZE);
}
#else
#  define GetSizeModifier(T)    (GetUnderlyingTypeCode (T) & T_MASK_SIZE)
#endif

#if defined(HAVE_INLINE)
INLINE TypeCode GetRawTypeRank (const Type* T)
/* Get the raw type rank of a type */
{
    return (T->C & T_MASK_RANK);
}
#else
#  define GetRawTypeRank(T)     ((T)->C & T_MASK_RANK)
#endif

#if defined(HAVE_INLINE)
INLINE TypeCode GetRawSignedness (const Type* T)
/* Get the raw signedness of a type */
{
    return (T->C & T_MASK_SIGN);
}
#else
#  define GetRawSignedness(T)   ((T)->C & T_MASK_SIGN)
#endif

#if defined(HAVE_INLINE)
INLINE TypeCode GetRawSizeModifier (const Type* T)
/* Get the raw size modifier of a type */
{
    return (T->C & T_MASK_SIZE);
}
#else
#  define GetRawSizeModifier(T) ((T)->C & T_MASK_SIZE)
#endif



/*****************************************************************************/
/*                             Type manipulation                             */
/*****************************************************************************/



Type* GetImplicitFuncType (void);
/* Return a type string for an implicitly declared function */

Type* GetCharArrayType (unsigned Len);
/* Return the type for a char array of the given length */

Type* NewPointerTo (const Type* T);
/* Return a type string that is "pointer to T". The type string is allocated
** on the heap and may be freed after use.
*/

Type* NewBitFieldOf (const Type* T, unsigned BitOffs, unsigned BitWidth);
/* Return a type string that is "unqualified T : BitWidth" aligned on BitOffs.
** The type string is allocated on the heap and may be freed after use.
*/

const Type* AddressOf (const Type* T);
/* Return a type string that is "address of T". The type string is allocated
** on the heap and may be freed after use.
*/

const Type* Indirect (const Type* T);
/* Do one indirection for the given type, that is, return the type where the
** given type points to.
*/

Type* ArrayToPtr (const Type* T);
/* Convert an array to a pointer to it's first element */

const Type* PtrConversion (const Type* T);
/* If the type is a function, convert it to pointer to function. If the
** expression is an array, convert it to pointer to first element. Otherwise
** return T.
*/

const Type* StdConversion (const Type* T);
/* If the type is a function, convert it to pointer to function. If the
** expression is an array, convert it to pointer to first element. If the
** type is an integer, do integeral promotion. Otherwise return T.
*/

const Type* IntPromotion (const Type* T);
/* Apply the integer promotions to T and return the result. The returned type
** string may be T if there is no need to change it.
*/

const Type* ArithmeticConvert (const Type* lhst, const Type* rhst);
/* Perform the usual arithmetic conversions for binary operators. */

const Type* GetSignedType (const Type* T);
/* Get signed counterpart of the integral type */

const Type* GetUnsignedType (const Type* T);
/* Get unsigned counterpart of the integral type */

const Type* GetUnderlyingType (const Type* Type);
/* Get the underlying type of an enum or other integer class type */

const Type* GetStructReplacementType (const Type* SType);
/* Get a replacement type for passing a struct/union by value in the primary */

const Type* GetBitFieldDeclType (const Type* Type);
/* Get the original integer type used to declare the bit-field */

const Type* GetBitFieldChunkType (const Type* Type);
/* Get the type needed to operate on the byte chunk containing the bit-field */



/*****************************************************************************/
/*                              Type Predicates                              */
/*****************************************************************************/



#if defined(HAVE_INLINE)
INLINE int IsRankChar (const Type* T)
/* Return true if this is a character type */
{
    return (GetTypeRank (T) == T_RANK_CHAR);
}
#else
#  define IsRankChar(T)         (GetTypeRank (T) == T_RANK_CHAR)
#endif

#if defined(HAVE_INLINE)
INLINE int IsRankShort (const Type* T)
/* Return true if this is a short type (signed or unsigned) */
{
    return (GetTypeRank (T) == T_RANK_SHORT);
}
#else
#  define IsRankShort(T)        (GetTypeRank (T) == T_RANK_SHORT)
#endif

#if defined(HAVE_INLINE)
INLINE int IsRankInt (const Type* T)
/* Return true if this is an int type (signed or unsigned) */
{
    return (GetTypeRank (T) == T_RANK_INT);
}
#else
#  define IsRankInt(T)          (GetTypeRank (T) == T_RANK_INT)
#endif

#if defined(HAVE_INLINE)
INLINE int IsRankLong (const Type* T)
/* Return true if this is a long int type (signed or unsigned) */
{
    return (GetTypeRank (T) == T_RANK_LONG);
}
#else
#  define IsRankLong(T)         (GetTypeRank (T) == T_RANK_LONG)
#endif

#if defined(HAVE_INLINE)
INLINE int IsDeclTypeChar (const Type* T)
/* Return true if this is declared as a char type (without signed/unsigned).
** This function is to exclude enums whose underlying type is char.
*/
{
    return (GetUnqualRawTypeCode (T) == T_CHAR);
}
#else
#  define IsDeclTypeChar(T)     (GetUnqualRawTypeCode (T) == T_CHAR)
#endif

#if defined(HAVE_INLINE)
INLINE int IsDeclRankChar (const Type* T)
/* Return true if this is declared as a character type (including signed/unsigned).
** This function is to exclude enums whose underlying types are character types.
*/
{
    return (GetRawTypeRank (T) == T_RANK_CHAR);
}
#else
#  define IsDeclRankChar(T)     (GetRawTypeRank (T) == T_RANK_CHAR)
#endif

#if defined(HAVE_INLINE)
INLINE int IsTypeFloat (const Type* T)
/* Return true if this is a float type */
{
    return (GetRawTypeRank (T) == T_RANK_FLOAT);
}
#else
#  define IsTypeFloat(T)        (GetRawTypeRank (T) == T_RANK_FLOAT)
#endif

#if defined(HAVE_INLINE)
INLINE int IsTypeDouble (const Type* T)
/* Return true if this is a double type */
{
    return (GetRawTypeRank (T) == T_RANK_DOUBLE);
}
#else
#  define IsTypeDouble(T)       (GetRawTypeRank (T) == T_RANK_DOUBLE)
#endif

#if defined(HAVE_INLINE)
INLINE int IsTypePtr (const Type* T)
/* Return true if this is a pointer type */
{
    return (GetRawTypeRank (T) == T_RANK_PTR);
}
#else
#  define IsTypePtr(T)          (GetRawTypeRank (T) == T_RANK_PTR)
#endif

#if defined(HAVE_INLINE)
INLINE int IsTypeEnum (const Type* T)
/* Return true if this is an enum type */
{
    return (GetRawTypeRank (T) == T_RANK_ENUM);
}
#else
#  define IsTypeEnum(T)         (GetRawTypeRank (T) == T_RANK_ENUM)
#endif

#if defined(HAVE_INLINE)
INLINE int IsTypeSignedBitField (const Type* T)
/* Return true if this is a signed bit-field */
{
    return (GetUnqualRawTypeCode (T) == T_SBITFIELD);
}
#else
#  define IsTypeSignedBitField(T)   (GetUnqualRawTypeCode (T) == T_SBITFIELD)
#endif

#if defined(HAVE_INLINE)
INLINE int IsTypeUnsignedBitField (const Type* T)
/* Return true if this is an unsigned bit-field */
{
    return (GetUnqualRawTypeCode (T) == T_UBITFIELD);
}
#else
#  define IsTypeUnsignedBitField(T) (GetUnqualRawTypeCode (T) == T_UBITFIELD)
#endif

#if defined(HAVE_INLINE)
INLINE int IsTypeBitField (const Type* T)
/* Return true if this is a bit-field (either signed or unsigned) */
{
    return IsTypeSignedBitField (T) || IsTypeUnsignedBitField (T);
}
#else
#  define IsTypeBitField(T)     (IsTypeSignedBitField (T) || IsTypeUnsignedBitField (T))
#endif

int IsTypeFragBitField (const Type* T);
/* Return true if this is a bit-field that shares byte space with other fields */

#if defined(HAVE_INLINE)
INLINE int IsTypeStruct (const Type* T)
/* Return true if this is a struct type */
{
    return (GetRawTypeRank (T) == T_RANK_STRUCT);
}
#else
#  define IsTypeStruct(T)       (GetRawTypeRank (T) == T_RANK_STRUCT)
#endif

#if defined(HAVE_INLINE)
INLINE int IsTypeUnion (const Type* T)
/* Return true if this is a union type */
{
    return (GetRawTypeRank (T) == T_RANK_UNION);
}
#else
#  define IsTypeUnion(T)        (GetRawTypeRank (T) == T_RANK_UNION)
#endif

#if defined(HAVE_INLINE)
INLINE int IsTypeArray (const Type* T)
/* Return true if this is an array type */
{
    return (GetRawTypeRank (T) == T_RANK_ARRAY);
}
#else
#  define IsTypeArray(T)        (GetRawTypeRank (T) == T_RANK_ARRAY)
#endif

#if defined(HAVE_INLINE)
INLINE int IsTypeVoid (const Type* T)
/* Return true if this is a void type */
{
    return (GetRawTypeRank (T) == T_RANK_VOID);
}
#else
#  define IsTypeVoid(T)         (GetRawTypeRank (T) == T_RANK_VOID)
#endif

#if defined(HAVE_INLINE)
INLINE int IsTypeFunc (const Type* T)
/* Return true if this is a function type */
{
    return (GetRawTypeRank (T) == T_RANK_FUNC);
}
#else
#  define IsTypeFunc(T)         (GetRawTypeRank (T) == T_RANK_FUNC)
#endif

#if defined(HAVE_INLINE)
INLINE int IsTypeFuncPtr (const Type* T)
/* Return true if this is a function pointer type */
{
    return (IsTypePtr (T) && IsTypeFunc (T+1));
}
#else
#  define IsTypeFuncPtr(T)      (IsTypePtr (T) && IsTypeFunc (T+1))
#endif

#if defined(HAVE_INLINE)
INLINE int IsTypeFuncLike (const Type* T)
/* Return true if this is a function or a function pointer */
{
    return IsTypeFunc (T) || IsTypeFuncPtr (T);
}
#else
int IsTypeFuncLike (const Type* T);
/* Return true if this is a function or a function pointer */
#endif

#if defined(HAVE_INLINE)
INLINE int IsClassInt (const Type* T)
/* Return true if this is an integer type */
{
    return (GetTypeClass (T) == T_CLASS_INT);
}
#else
#  define IsClassInt(T)         (GetTypeClass (T) == T_CLASS_INT)
#endif

#if defined(HAVE_INLINE)
INLINE int IsClassFloat (const Type* T)
/* Return true if this is a floating type */
{
    return (GetTypeClass (T) == T_CLASS_FLOAT);
}
#else
#  define IsClassFloat(T)       (GetTypeClass (T) == T_CLASS_FLOAT)
#endif

#if defined(HAVE_INLINE)
INLINE int IsClassPtr (const Type* T)
/* Return true if this is a pointer or array type */
{
    return (GetTypeClass (T) == T_CLASS_PTR);
}
#else
#  define IsClassPtr(T)         (GetTypeClass (T) == T_CLASS_PTR)
#endif

#if defined(HAVE_INLINE)
INLINE int IsClassStruct (const Type* T)
/* Return true if this is a struct or union type */
{
    return (GetTypeClass (T) == T_CLASS_STRUCT);
}
#else
#  define IsClassStruct(T)      (GetTypeClass (T) == T_CLASS_STRUCT)
#endif

#if defined(HAVE_INLINE)
INLINE int IsClassFunc (const Type* T)
/* Return true if this is a function type */
{
    return (GetTypeClass (T) == T_CLASS_FUNC);
}
#else
#  define IsClassFunc(T)        (GetTypeClass (T) == T_CLASS_FUNC)
#endif

int IsObjectType (const Type* T);
/* Return true if this is a fully described object type */

int IsIncompleteType (const Type* T);
/* Return true if this is an object type lacking size info */

int IsArithmeticType (const Type* T);
/* Return true if this is an integer or floating type */

int IsBasicType (const Type* T);
/* Return true if this is a char, integer or floating type */

int IsScalarType (const Type* T);
/* Return true if this is an arithmetic or pointer type */

int IsDerivedType (const Type* T);
/* Return true if this is an array, struct, union, function or pointer type */

int IsAggregateType (const Type* T);
/* Return true if this is an array or struct type */

int IsDerivedDeclaratorType (const Type* T);
/* Return true if this is an array, function or pointer type */

int IsRelationType (const Type* T);
/* Return true if this is an arithmetic, array or pointer type */

int IsCastType (const Type* T);
/* Return true if this type can be used for casting */

int IsESUType (const Type* T);
/* Return true if this is an enum/struct/union type */

int IsIncompleteESUType (const Type* T);
/* Return true if this is an incomplete ESU type */

int IsAnonESUType (const Type* T);
/* Return true if this is an anonymous ESU type */

int IsAnonStructClass (const Type* T);
/* Return true if this is an anonymous struct or union type */

int IsPassByRefType (const Type* T);
/* Return true if this is a large struct/union type that doesn't fit in the
** primary. This returns false for the void value extension type since it is
** not passable at all.
*/

int IsEmptiableObjectType (const Type* T);
/* Return true if this is a struct/union/void type that can have zero size */

int HasUnknownSize (const Type* T);
/* Return true if this is an incomplete ESU type or an array of unknown size */

int TypeHasAttrData (const Type* T);
/* Return true if the given type has attribute data */

#if defined(HAVE_INLINE)
INLINE int IsRawSignUnsigned (const Type* T)
/* Return true if this is an unsigned raw type */
{
    return (GetRawSignedness (T) == T_SIGN_UNSIGNED);
}
#else
#  define IsRawSignUnsigned(T)  (GetRawSignedness (T) == T_SIGN_UNSIGNED)
#endif

#if defined(HAVE_INLINE)
INLINE int IsSignUnsigned (const Type* T)
/* Return true if this is an unsigned type */
{
    return (GetSignedness (T) == T_SIGN_UNSIGNED);
}
#else
#  define IsSignUnsigned(T)     (GetSignedness (T) == T_SIGN_UNSIGNED)
#endif

#if defined(HAVE_INLINE)
INLINE int IsRawSignSigned (const Type* T)
/* Return true if this is a signed raw type */
{
    return (GetRawSignedness (T) == T_SIGN_SIGNED);
}
#else
#  define IsRawSignSigned(T)    (GetRawSignedness (T) == T_SIGN_SIGNED)
#endif

#if defined(HAVE_INLINE)
INLINE int IsSignSigned (const Type* T)
/* Return true if this is a signed type */
{
    return (GetSignedness (T) == T_SIGN_SIGNED);
}
#else
#  define IsSignSigned(T)       (GetSignedness (T) == T_SIGN_SIGNED)
#endif



/*****************************************************************************/
/*                             Qualifier helpers                             */
/*****************************************************************************/



#if defined(HAVE_INLINE)
INLINE int IsQualConst (const Type* T)
/* Return true if the given type has a const memory image */
{
    return (T->C & T_QUAL_CONST) != 0;
}
#else
#  define IsQualConst(T)        (((T)->C & T_QUAL_CONST) != 0)
#endif

#if defined(HAVE_INLINE)
INLINE int IsQualVolatile (const Type* T)
/* Return true if the given type has a volatile type qualifier */
{
    return (T->C & T_QUAL_VOLATILE) != 0;
}
#else
#  define IsQualVolatile(T)     (((T)->C & T_QUAL_VOLATILE) != 0)
#endif

#if defined(HAVE_INLINE)
INLINE int IsQualRestrict (const Type* T)
/* Return true if the given type has a restrict qualifier */
{
    return (T->C & T_QUAL_RESTRICT) != 0;
}
#else
#  define IsQualRestrict(T)     (((T)->C & T_QUAL_RESTRICT) != 0)
#endif

#if defined(HAVE_INLINE)
INLINE int IsQualNear (const Type* T)
/* Return true if the given type has a near qualifier */
{
    return (T->C & T_QUAL_NEAR) != 0;
}
#else
#  define IsQualNear(T)         (((T)->C & T_QUAL_NEAR) != 0)
#endif

#if defined(HAVE_INLINE)
INLINE int IsQualFar (const Type* T)
/* Return true if the given type has a far qualifier */
{
    return (T->C & T_QUAL_FAR) != 0;
}
#else
#  define IsQualFar(T)          (((T)->C & T_QUAL_FAR) != 0)
#endif

#if defined(HAVE_INLINE)
INLINE int IsQualFastcall (const Type* T)
/* Return true if the given type has a fastcall qualifier */
{
    return (T->C & T_QUAL_FASTCALL) != 0;
}
#else
#  define IsQualFastcall(T)     (((T)->C & T_QUAL_FASTCALL) != 0)
#endif

#if defined(HAVE_INLINE)
INLINE int IsQualCDecl (const Type* T)
/* Return true if the given type has a cdecl qualifier */
{
    return (T->C & T_QUAL_CDECL) != 0;
}
#else
#  define IsQualCDecl(T)        (((T)->C & T_QUAL_CDECL) != 0)
#endif

#if defined(HAVE_INLINE)
INLINE int IsQualCConv (const Type* T)
/* Return true if the given type has a calling convention qualifier */
{
    return (T->C & T_QUAL_CCONV) != 0;
}
#else
#  define IsQualCConv(T)        (((T)->C & T_QUAL_CCONV) != 0)
#endif

TypeCode AddrSizeQualifier (unsigned AddrSize);
/* Return T_QUAL_NEAR or T_QUAL_FAR depending on the address size */

#if defined(HAVE_INLINE)
INLINE TypeCode CodeAddrSizeQualifier (void)
/* Return T_QUAL_NEAR or T_QUAL_FAR depending on the code address size */
{
    return AddrSizeQualifier (CodeAddrSize);
}
#else
#  define CodeAddrSizeQualifier()      (AddrSizeQualifier (CodeAddrSize))
#endif

#if defined(HAVE_INLINE)
INLINE TypeCode DataAddrSizeQualifier (void)
/* Return T_QUAL_NEAR or T_QUAL_FAR depending on the data address size */
{
    return AddrSizeQualifier (DataAddrSize);
}
#else
#  define DataAddrSizeQualifier()      (AddrSizeQualifier (DataAddrSize))
#endif



/*****************************************************************************/
/*                           Function type helpers                           */
/*****************************************************************************/



int IsVariadicFunc (const Type* T) attribute ((const));
/* Return true if this is a function type or pointer to function type with
** variable parameter list.
** Check fails if the type is not a function or a pointer to function.
*/

int IsFastcallFunc (const Type* T) attribute ((const));
/* Return true if this is a function type or pointer to function type with
** __fastcall__ calling convention.
** Check fails if the type is not a function or a pointer to function.
*/

FuncDesc* GetFuncDesc (const Type* T) attribute ((const));
/* Get the FuncDesc pointer from a function or pointer-to-function type */

void SetFuncDesc (Type* T, FuncDesc* F);
/* Set the FuncDesc pointer in a function or pointer-to-function type */

const Type* GetFuncReturnType (const Type* T) attribute ((const));
/* Return a pointer to the return type of a function or pointer-to-function type */

Type* GetFuncReturnTypeModifiable (Type* T) attribute ((const));
/* Return a non-const pointer to the return type of a function or pointer-to-function type */

const FuncDesc* GetFuncDefinitionDesc (const Type* T) attribute ((const));
/* Get the function descriptor of the function definition */



/*****************************************************************************/
/*                            Array type helpers                             */
/*****************************************************************************/



long GetElementCount (const Type* T);
/* Get the element count of the array specified in T (which must be of
** array type).
*/

void SetElementCount (Type* T, long Count);
/* Set the element count of the array specified in T (which must be of
** array type).
*/

const Type* GetElementType (const Type* T);
/* Return the element type of the given array type */

Type* GetElementTypeModifiable (Type* T);
/* Return the element type of the given array type */

const Type* GetBaseElementType (const Type* T);
/* Return the base element type of a given type. If T is not an array, this
** will return. Otherwise it will return the base element type, which means
** the element type that is not an array.
*/



/*****************************************************************************/
/*                             ESU types helpers                             */
/*****************************************************************************/



struct SymEntry* GetESUTagSym (const Type* T) attribute ((const));
/* Get the tag symbol entry of the enum/struct/union type.
** Return 0 if it is not an enum/struct/union.
*/

void SetESUTagSym (Type* T, struct SymEntry* S);
/* Set the tag symbol entry of the enum/struct/union type */



/*****************************************************************************/
/*                                  Helpers                                  */
/*****************************************************************************/



const char* GetBasicTypeName (const Type* T);
/* Return a const name string of the basic type.
** Return "<type>" for unknown basic types.
*/

const char* GetFullTypeName (const Type* T);
/* Return the full name string of the given type */

struct StrBuf* GetFullTypeNameBuf (struct StrBuf* S, const Type* T);
/* Return the full name string of the given type */

int GetQualifierTypeCodeNameBuf (struct StrBuf* S, TypeCode Qual, TypeCode IgnoredQual);
/* Return the names of the qualifiers of the type.
** Qualifiers to be ignored can be specified with the IgnoredQual flags.
** Return the count of added qualifier names.
*/

void PrintType (FILE* F, const Type* T);
/* Print fulle name of the type */

void PrintFuncSig (FILE* F, const char* Name, const Type* T);
/* Print a function signature */

void PrintRawType (FILE* F, const Type* T);
/* Print a type string in raw hex format (for debugging) */



/* End of datatype.h */

#endif
