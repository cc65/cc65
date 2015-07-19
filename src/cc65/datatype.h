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
/*                                   Data                                    */
/*****************************************************************************/




/* Basic data types */
enum {
    T_END           = 0x000000,

    /* Basic types */
    T_TYPE_NONE     = 0x000000,
    T_TYPE_CHAR     = 0x000001,
    T_TYPE_SHORT    = 0x000002,
    T_TYPE_INT      = 0x000003,
    T_TYPE_LONG     = 0x000004,
    T_TYPE_LONGLONG = 0x000005,
    T_TYPE_ENUM     = 0x000006,
    T_TYPE_FLOAT    = 0x000007,
    T_TYPE_DOUBLE   = 0x000008,
    T_TYPE_VOID     = 0x000009,
    T_TYPE_STRUCT   = 0x00000A,
    T_TYPE_UNION    = 0x00000B,
    T_TYPE_ARRAY    = 0x00000C,
    T_TYPE_PTR      = 0x00000D,
    T_TYPE_FUNC     = 0x00000E,
    T_MASK_TYPE     = 0x00000F,

    /* Type classes */
    T_CLASS_NONE    = 0x000000,
    T_CLASS_INT     = 0x000010,
    T_CLASS_FLOAT   = 0x000020,
    T_CLASS_PTR     = 0x000030,
    T_CLASS_STRUCT  = 0x000040,
    T_CLASS_FUNC    = 0x000050,
    T_MASK_CLASS    = 0x000070,

    /* Type signedness */
    T_SIGN_NONE     = 0x000000,
    T_SIGN_UNSIGNED = 0x000080,
    T_SIGN_SIGNED   = 0x000100,
    T_MASK_SIGN     = 0x000180,

    /* Type size modifiers */
    T_SIZE_NONE     = 0x000000,
    T_SIZE_SHORT    = 0x000200,
    T_SIZE_LONG     = 0x000400,
    T_SIZE_LONGLONG = 0x000600,
    T_MASK_SIZE     = 0x000600,

    /* Type qualifiers */
    T_QUAL_NONE     = 0x000000,
    T_QUAL_CONST    = 0x000800,
    T_QUAL_VOLATILE = 0x001000,
    T_QUAL_RESTRICT = 0x002000,
    T_QUAL_NEAR     = 0x004000,
    T_QUAL_FAR      = 0x008000,
    T_QUAL_ADDRSIZE = T_QUAL_NEAR | T_QUAL_FAR,
    T_QUAL_FASTCALL = 0x010000,
    T_QUAL_CDECL    = 0x020000,
    T_QUAL_CCONV    = T_QUAL_FASTCALL | T_QUAL_CDECL,
    T_MASK_QUAL     = 0x03F800,

    /* Types */
    T_CHAR      = T_TYPE_CHAR     | T_CLASS_INT    | T_SIGN_UNSIGNED | T_SIZE_NONE,
    T_SCHAR     = T_TYPE_CHAR     | T_CLASS_INT    | T_SIGN_SIGNED   | T_SIZE_NONE,
    T_UCHAR     = T_TYPE_CHAR     | T_CLASS_INT    | T_SIGN_UNSIGNED | T_SIZE_NONE,
    T_SHORT     = T_TYPE_SHORT    | T_CLASS_INT    | T_SIGN_SIGNED   | T_SIZE_SHORT,
    T_USHORT    = T_TYPE_SHORT    | T_CLASS_INT    | T_SIGN_UNSIGNED | T_SIZE_SHORT,
    T_INT       = T_TYPE_INT      | T_CLASS_INT    | T_SIGN_SIGNED   | T_SIZE_NONE,
    T_UINT      = T_TYPE_INT      | T_CLASS_INT    | T_SIGN_UNSIGNED | T_SIZE_NONE,
    T_LONG      = T_TYPE_LONG     | T_CLASS_INT    | T_SIGN_SIGNED   | T_SIZE_LONG,
    T_ULONG     = T_TYPE_LONG     | T_CLASS_INT    | T_SIGN_UNSIGNED | T_SIZE_LONG,
    T_LONGLONG  = T_TYPE_LONGLONG | T_CLASS_INT    | T_SIGN_SIGNED   | T_SIZE_LONGLONG,
    T_ULONGLONG = T_TYPE_LONGLONG | T_CLASS_INT    | T_SIGN_UNSIGNED | T_SIZE_LONGLONG,
    T_ENUM      = T_TYPE_ENUM     | T_CLASS_INT    | T_SIGN_SIGNED   | T_SIZE_NONE,
    T_FLOAT     = T_TYPE_FLOAT    | T_CLASS_FLOAT  | T_SIGN_NONE     | T_SIZE_NONE,
    T_DOUBLE    = T_TYPE_DOUBLE   | T_CLASS_FLOAT  | T_SIGN_NONE     | T_SIZE_NONE,
    T_VOID      = T_TYPE_VOID     | T_CLASS_NONE   | T_SIGN_NONE     | T_SIZE_NONE,
    T_STRUCT    = T_TYPE_STRUCT   | T_CLASS_STRUCT | T_SIGN_NONE     | T_SIZE_NONE,
    T_UNION     = T_TYPE_UNION    | T_CLASS_STRUCT | T_SIGN_NONE     | T_SIZE_NONE,
    T_ARRAY     = T_TYPE_ARRAY    | T_CLASS_PTR    | T_SIGN_NONE     | T_SIZE_NONE,
    T_PTR       = T_TYPE_PTR      | T_CLASS_PTR    | T_SIGN_NONE     | T_SIZE_NONE,
    T_FUNC      = T_TYPE_FUNC     | T_CLASS_FUNC   | T_SIGN_NONE     | T_SIZE_NONE,

    /* Aliases */
    T_SIZE_T    = T_UINT,
};



/* Type code entry */
typedef unsigned long TypeCode;

/* Type entry */
typedef struct Type Type;
struct Type {
    TypeCode            C;      /* Code for this entry */
    union {
        void*           P;      /* Arbitrary attribute pointer */
        long            L;      /* Numeric attribute value */
        unsigned long   U;      /* Dito, unsigned */
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
extern Type type_schar[];
extern Type type_uchar[];
extern Type type_int[];
extern Type type_uint[];
extern Type type_long[];
extern Type type_ulong[];
extern Type type_void[];
extern Type type_size_t[];
extern Type type_float[];
extern Type type_double[];

/* Forward for the SymEntry struct */
struct SymEntry;



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

int SignExtendChar (int C);
/* Do correct sign extension of a character */

TypeCode GetDefaultChar (void);
/* Return the default char type (signed/unsigned) depending on the settings */

Type* GetCharArrayType (unsigned Len);
/* Return the type for a char array of the given length */

Type* GetImplicitFuncType (void);
/* Return a type string for an inplicitly declared function */

Type* PointerTo (const Type* T);
/* Return a type string that is "pointer to T". The type string is allocated
** on the heap and may be freed after use.
*/

void PrintType (FILE* F, const Type* T);
/* Output translation of type array. */

void PrintRawType (FILE* F, const Type* T);
/* Print a type string in raw format (for debugging) */

void PrintFuncSig (FILE* F, const char* Name, Type* T);
/* Print a function signature. */

int TypeHasAttr (const Type* T);
/* Return true if the given type has attribute data */

#if defined(HAVE_INLINE)
INLINE void CopyTypeAttr (const Type* Src, Type* Dest)
/* Copy attribute data from Src to Dest */
{
    Dest->A = Src->A;
}
#else
#  define CopyTypeAttr(Src, Dest)       ((Dest)->A = (Src)->A)
#endif

#if defined(HAVE_INLINE)
INLINE TypeCode UnqualifiedType (TypeCode T)
/* Return the unqalified type code */
{
    return (T & ~T_MASK_QUAL);
}
#else
#  define UnqualifiedType(T)    ((T) & ~T_MASK_QUAL)
#endif

unsigned SizeOf (const Type* T);
/* Compute size of object represented by type array. */

unsigned PSizeOf (const Type* T);
/* Compute size of pointer object. */

unsigned CheckedSizeOf (const Type* T);
/* Return the size of a data type. If the size is zero, emit an error and
** return some valid size instead (so the rest of the compiler doesn't have
** to work with invalid sizes).
*/
unsigned CheckedPSizeOf (const Type* T);
/* Return the size of a data type that is pointed to by a pointer. If the
** size is zero, emit an error and return some valid size instead (so the
** rest of the compiler doesn't have to work with invalid sizes).
*/

unsigned TypeOf (const Type* T);
/* Get the code generator base type of the object */

Type* Indirect (Type* T);
/* Do one indirection for the given type, that is, return the type where the
** given type points to.
*/

Type* ArrayToPtr (Type* T);
/* Convert an array to a pointer to it's first element */

#if defined(HAVE_INLINE)
INLINE TypeCode GetType (const Type* T)
/* Get the raw type */
{
    return (T->C & T_MASK_TYPE);
}
#else
#  define GetType(T)    ((T)->C & T_MASK_TYPE)
#endif

#if defined(HAVE_INLINE)
INLINE int IsTypeChar (const Type* T)
/* Return true if this is a character type */
{
    return (GetType (T) == T_TYPE_CHAR);
}
#else
#  define IsTypeChar(T)         (GetType (T) == T_TYPE_CHAR)
#endif

#if defined(HAVE_INLINE)
INLINE int IsTypeInt (const Type* T)
/* Return true if this is an int type (signed or unsigned) */
{
    return (GetType (T) == T_TYPE_INT);
}
#else
#  define IsTypeInt(T)          (GetType (T) == T_TYPE_INT)
#endif

#if defined(HAVE_INLINE)
INLINE int IsTypeLong (const Type* T)
/* Return true if this is a long type (signed or unsigned) */
{
    return (GetType (T) == T_TYPE_LONG);
}
#else
#  define IsTypeLong(T)         (GetType (T) == T_TYPE_LONG)
#endif

#if defined(HAVE_INLINE)
INLINE int IsTypeFloat (const Type* T)
/* Return true if this is a float type */
{
    return (GetType (T) == T_TYPE_FLOAT);
}
#else
#  define IsTypeFloat(T)        (GetType (T) == T_TYPE_FLOAT)
#endif

#if defined(HAVE_INLINE)
INLINE int IsTypeDouble (const Type* T)
/* Return true if this is a double type */
{
    return (GetType (T) == T_TYPE_DOUBLE);
}
#else
#  define IsTypeDouble(T)       (GetType (T) == T_TYPE_DOUBLE)
#endif

#if defined(HAVE_INLINE)
INLINE int IsTypePtr (const Type* T)
/* Return true if this is a pointer type */
{
    return (GetType (T) == T_TYPE_PTR);
}
#else
#  define IsTypePtr(T)          (GetType (T) == T_TYPE_PTR)
#endif

#if defined(HAVE_INLINE)
INLINE int IsTypeStruct (const Type* T)
/* Return true if this is a struct type */
{
    return (GetType (T) == T_TYPE_STRUCT);
}
#else
#  define IsTypeStruct(T)       (GetType (T) == T_TYPE_STRUCT)
#endif

#if defined(HAVE_INLINE)
INLINE int IsTypeUnion (const Type* T)
/* Return true if this is a union type */
{
    return (GetType (T) == T_TYPE_UNION);
}
#else
#  define IsTypeUnion(T)       (GetType (T) == T_TYPE_UNION)
#endif

#if defined(HAVE_INLINE)
INLINE int IsTypeArray (const Type* T)
/* Return true if this is an array type */
{
    return (GetType (T) == T_TYPE_ARRAY);
}
#else
#  define IsTypeArray(T)        (GetType (T) == T_TYPE_ARRAY)
#endif

#if defined(HAVE_INLINE)
INLINE int IsTypeVoid (const Type* T)
/* Return true if this is a void type */
{
    return (GetType (T) == T_TYPE_VOID);
}
#else
#  define IsTypeVoid(T)         (GetType (T) == T_TYPE_VOID)
#endif

#if defined(HAVE_INLINE)
INLINE int IsTypeFunc (const Type* T)
/* Return true if this is a function class */
{
    return (GetType (T) == T_TYPE_FUNC);
}
#else
#  define IsTypeFunc(T)         (GetType (T) == T_TYPE_FUNC)
#endif

#if defined(HAVE_INLINE)
INLINE int IsTypeFuncPtr (const Type* T)
/* Return true if this is a function pointer */
{
    return (IsTypePtr (T) && IsTypeFunc (T+1));
}
#else
#  define IsTypeFuncPtr(T)      (IsTypePtr (T) && IsTypeFunc (T+1))
#endif

#if defined(HAVE_INLINE)
INLINE TypeCode GetClass (const Type* T)
/* Get the class of a type string */
{
    return (T->C & T_MASK_CLASS);
}
#else
#  define GetClass(T)    ((T)->C & T_MASK_CLASS)
#endif

#if defined(HAVE_INLINE)
INLINE int IsClassInt (const Type* T)
/* Return true if this is an integer type */
{
    return (GetClass (T) == T_CLASS_INT);
}
#else
#  define IsClassInt(T)         (GetClass (T) == T_CLASS_INT)
#endif

#if defined(HAVE_INLINE)
INLINE int IsClassFloat (const Type* T)
/* Return true if this is a float type */
{
    return (GetClass (T) == T_CLASS_FLOAT);
}
#else
#  define IsClassFloat(T)       (GetClass (T) == T_CLASS_FLOAT)
#endif

#if defined(HAVE_INLINE)
INLINE int IsClassPtr (const Type* T)
/* Return true if this is a pointer type */
{
    return (GetClass (T) == T_CLASS_PTR);
}
#else
#  define IsClassPtr(T)         (GetClass (T) == T_CLASS_PTR)
#endif

#if defined(HAVE_INLINE)
INLINE int IsClassStruct (const Type* T)
/* Return true if this is a struct type */
{
    return (GetClass (T) == T_CLASS_STRUCT);
}
#else
#  define IsClassStruct(T)      (GetClass (T) == T_CLASS_STRUCT)
#endif

#if defined(HAVE_INLINE)
INLINE int IsClassFunc (const Type* T)
/* Return true if this is a function type */
{
    return (GetClass (T) == T_CLASS_FUNC);
}
#else
#  define IsClassFunc(T)        (GetClass (T) == T_CLASS_FUNC)
#endif

#if defined(HAVE_INLINE)
INLINE TypeCode GetSignedness (const Type* T)
/* Get the sign of a type */
{
    return (T->C & T_MASK_SIGN);
}
#else
#  define GetSignedness(T)      ((T)->C & T_MASK_SIGN)
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
INLINE int IsSignSigned (const Type* T)
/* Return true if this is a signed type */
{
    return (GetSignedness (T) == T_SIGN_SIGNED);
}
#else
#  define IsSignSigned(T)       (GetSignedness (T) == T_SIGN_SIGNED)
#endif

#if defined(HAVE_INLINE)
INLINE TypeCode GetQualifier (const Type* T)
/* Get the qualifier from the given type string */
{
    return (T->C & T_MASK_QUAL);
}
#else
#  define GetQualifier(T)      ((T)->C & T_MASK_QUAL)
#endif

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

int IsVariadicFunc (const Type* T) attribute ((const));
/* Return true if this is a function type or pointer to function type with
** variable parameter list
*/

#if defined(HAVE_INLINE)
INLINE TypeCode GetSizeModifier (const Type* T)
/* Get the size modifier of a type */
{
    return (T->C & T_MASK_SIZE);
}
#else
#  define GetSizeModifier(T)      ((T)->C & T_MASK_SIZE)
#endif

FuncDesc* GetFuncDesc (const Type* T) attribute ((const));
/* Get the FuncDesc pointer from a function or pointer-to-function type */

void SetFuncDesc (Type* T, FuncDesc* F);
/* Set the FuncDesc pointer in a function or pointer-to-function type */

Type* GetFuncReturn (Type* T) attribute ((const));
/* Return a pointer to the return type of a function or pointer-to-function type */

long GetElementCount (const Type* T);
/* Get the element count of the array specified in T (which must be of
** array type).
*/

void SetElementCount (Type* T, long Count);
/* Set the element count of the array specified in T (which must be of
** array type).
*/

Type* GetElementType (Type* T);
/* Return the element type of the given array type. */

Type* GetBaseElementType (Type* T);
/* Return the base element type of a given type. If T is not an array, this
** will return. Otherwise it will return the base element type, which means
** the element type that is not an array.
*/

struct SymEntry* GetSymEntry (const Type* T) attribute ((const));
/* Return a SymEntry pointer from a type */

void SetSymEntry (Type* T, struct SymEntry* S);
/* Set the SymEntry pointer for a type */

Type* IntPromotion (Type* T);
/* Apply the integer promotions to T and return the result. The returned type
** string may be T if there is no need to change it.
*/

Type* PtrConversion (Type* T);
/* If the type is a function, convert it to pointer to function. If the
** expression is an array, convert it to pointer to first element. Otherwise
** return T.
*/

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



/* End of datatype.h */

#endif
