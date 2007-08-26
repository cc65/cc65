/*****************************************************************************/
/*                                                                           */
/*				  datatype.h				     */
/*                                                                           */
/*		 Type string handling for the cc65 C compiler		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2006 Ullrich von Bassewitz                                       */
/*               Römerstrasse 52                                             */
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



#ifndef DATATYPE_H
#define DATATYPE_H



#include <stdio.h>

/* common */
#include "attrib.h"
#include "inline.h"

/* cc65 */
#include "funcdesc.h"



/*****************************************************************************/
/*     	      	    	       	     Data   				     */
/*****************************************************************************/




/* Basic data types */
enum {
    T_END	    = 0x0000,

    /* Basic types */
    T_TYPE_NONE	    = 0x0000,
    T_TYPE_CHAR	    = 0x0001,
    T_TYPE_SHORT    = 0x0002,
    T_TYPE_INT 	    = 0x0003,
    T_TYPE_LONG	    = 0x0004,
    T_TYPE_LONGLONG = 0x0005,
    T_TYPE_ENUM	    = 0x0006,
    T_TYPE_FLOAT    = 0x0007,
    T_TYPE_DOUBLE   = 0x0008,
    T_TYPE_VOID     = 0x0009,
    T_TYPE_STRUCT   = 0x000A,
    T_TYPE_UNION    = 0x000B,
    T_TYPE_ARRAY    = 0x000C,
    T_TYPE_PTR      = 0x000D,
    T_TYPE_FUNC     = 0x000E,
    T_MASK_TYPE	    = 0x001F,

    /* Type classes */
    T_CLASS_NONE    = 0x0000,
    T_CLASS_INT	    = 0x0020,
    T_CLASS_FLOAT   = 0x0040,
    T_CLASS_PTR	    = 0x0060,
    T_CLASS_STRUCT  = 0x0080,
    T_CLASS_FUNC    = 0x00A0,
    T_MASK_CLASS    = 0x00E0,

    /* Type signedness */
    T_SIGN_NONE	    = 0x0000,
    T_SIGN_UNSIGNED = 0x0100,
    T_SIGN_SIGNED   = 0x0200,
    T_MASK_SIGN     = 0x0300,

    /* Type size modifiers */
    T_SIZE_NONE	    = 0x0000,
    T_SIZE_SHORT    = 0x0400,
    T_SIZE_LONG     = 0x0800,
    T_SIZE_LONGLONG = 0x0C00,
    T_MASK_SIZE	    = 0x0C00,

    /* Type qualifiers */
    T_QUAL_NONE     = 0x0000,
    T_QUAL_CONST    = 0x1000,
    T_QUAL_VOLATILE = 0x2000,
    T_QUAL_RESTRICT = 0x4000,
    T_MASK_QUAL	    = 0x7000,

    /* Types */
    T_CHAR     	= T_TYPE_CHAR     | T_CLASS_INT    | T_SIGN_UNSIGNED | T_SIZE_NONE,
    T_SCHAR    	= T_TYPE_CHAR     | T_CLASS_INT    | T_SIGN_SIGNED   | T_SIZE_NONE,
    T_UCHAR    	= T_TYPE_CHAR     | T_CLASS_INT    | T_SIGN_UNSIGNED | T_SIZE_NONE,
    T_SHORT    	= T_TYPE_SHORT    | T_CLASS_INT    | T_SIGN_SIGNED   | T_SIZE_SHORT,
    T_USHORT    = T_TYPE_SHORT    | T_CLASS_INT    | T_SIGN_UNSIGNED | T_SIZE_SHORT,
    T_INT      	= T_TYPE_INT      | T_CLASS_INT    | T_SIGN_SIGNED   | T_SIZE_NONE,
    T_UINT     	= T_TYPE_INT      | T_CLASS_INT    | T_SIGN_UNSIGNED | T_SIZE_NONE,
    T_LONG     	= T_TYPE_LONG     | T_CLASS_INT    | T_SIGN_SIGNED   | T_SIZE_LONG,
    T_ULONG    	= T_TYPE_LONG     | T_CLASS_INT    | T_SIGN_UNSIGNED | T_SIZE_LONG,
    T_LONGLONG 	= T_TYPE_LONGLONG | T_CLASS_INT    | T_SIGN_SIGNED   | T_SIZE_LONGLONG,
    T_ULONGLONG	= T_TYPE_LONGLONG | T_CLASS_INT    | T_SIGN_UNSIGNED | T_SIZE_LONGLONG,
    T_ENUM     	= T_TYPE_ENUM     | T_CLASS_INT    | T_SIGN_SIGNED   | T_SIZE_NONE,
    T_FLOAT    	= T_TYPE_FLOAT    | T_CLASS_FLOAT  | T_SIGN_NONE     | T_SIZE_NONE,
    T_DOUBLE   	= T_TYPE_DOUBLE   | T_CLASS_FLOAT  | T_SIGN_NONE     | T_SIZE_NONE,
    T_VOID     	= T_TYPE_VOID     | T_CLASS_NONE   | T_SIGN_NONE     | T_SIZE_NONE,
    T_STRUCT    = T_TYPE_STRUCT   | T_CLASS_STRUCT | T_SIGN_NONE     | T_SIZE_NONE,
    T_UNION     = T_TYPE_UNION    | T_CLASS_STRUCT | T_SIGN_NONE     | T_SIZE_NONE,
    T_ARRAY    	= T_TYPE_ARRAY    | T_CLASS_PTR    | T_SIGN_NONE     | T_SIZE_NONE,
    T_PTR      	= T_TYPE_PTR      | T_CLASS_PTR    | T_SIGN_NONE     | T_SIZE_NONE,
    T_FUNC     	= T_TYPE_FUNC     | T_CLASS_FUNC   | T_SIGN_NONE     | T_SIZE_NONE,

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
#define MAXTYPELEN   	30

/* Special encodings for element counts of an array */
#define UNSPECIFIED     -1L     /* Element count was not specified */
#define FLEXIBLE        0L      /* Flexible array struct member */

/* Sizes */
#define SIZEOF_CHAR     1
#define SIZEOF_SHORT    2
#define SIZEOF_INT      2
#define SIZEOF_LONG     4
#define SIZEOF_LONGLONG 8
#define SIZEOF_FLOAT    4
#define SIZEOF_DOUBLE   4
#define SIZEOF_PTR      2

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
/*     	      	      	       	     Code				     */
/*****************************************************************************/



unsigned TypeLen (const Type* T);
/* Return the length of the type string */

Type* TypeCpy (Type* Dest, const Type* Src);
/* Copy a type string */

Type* TypeDup (const Type* T);
/* Create a copy of the given type on the heap */

Type* TypeAlloc (unsigned Len);
/* Allocate memory for a type string of length Len. Len *must* include the
 * trailing T_END.
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
 * on the heap and may be freed after use.
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
 * return some valid size instead (so the rest of the compiler doesn't have
 * to work with invalid sizes).
 */
unsigned CheckedPSizeOf (const Type* T);
/* Return the size of a data type that is pointed to by a pointer. If the
 * size is zero, emit an error and return some valid size instead (so the
 * rest of the compiler doesn't have to work with invalid sizes).
 */

unsigned TypeOf (const Type* T);
/* Get the code generator base type of the object */

Type* Indirect (Type* T);
/* Do one indirection for the given type, that is, return the type where the
 * given type points to.
 */

Type* ArrayToPtr (const Type* T);
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

TypeCode GetQualifier (const Type* T) attribute ((const));
/* Get the qualifier from the given type string */           

#if defined(HAVE_INLINE)
INLINE int IsQualConst (const Type* T)
/* Return true if the given type has a const memory image */
{
    return (GetQualifier (T) & T_QUAL_CONST) != 0;
}
#else
#  define IsQualConst(T)        ((GetQualifier (T) & T_QUAL_CONST) != 0)
#endif

#if defined(HAVE_INLINE)
INLINE int IsQualVolatile (const Type* T)
/* Return true if the given type has a volatile type qualifier */
{
    return (GetQualifier (T) & T_QUAL_VOLATILE) != 0;
}
#else
#  define IsQualVolatile(T)     ((GetQualifier (T) & T_QUAL_VOLATILE) != 0)
#endif

#if defined(HAVE_INLINE)
INLINE int IsQualRestrict (const Type* T)
/* Return true if the given type has a restrict qualifier */
{
    return (GetQualifier (T) & T_QUAL_RESTRICT) != 0;
}
#else
#  define IsQualRestrict(T)     ((GetQualifier (T) & T_QUAL_RESTRICT) != 0)
#endif

int IsFastCallFunc (const Type* T) attribute ((const));
/* Return true if this is a function type or pointer to function with
 * __fastcall__ calling conventions
 */

int IsVariadicFunc (const Type* T) attribute ((const));
/* Return true if this is a function type or pointer to function type with
 * variable parameter list
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
 * array type).
 */

void SetElementCount (Type* T, long Count);
/* Set the element count of the array specified in T (which must be of
 * array type).
 */

Type* GetElementType (Type* T);
/* Return the element type of the given array type. */

struct SymEntry* GetSymEntry (const Type* T) attribute ((const));
/* Return a SymEntry pointer from a type */

void SetSymEntry (Type* T, struct SymEntry* S);
/* Set the SymEntry pointer for a type */

Type* IntPromotion (Type* T);
/* Apply the integer promotions to T and return the result. The returned type
 * string may be T if there is no need to change it.
 */

Type* PtrConversion (Type* T);
/* If the type is a function, convert it to pointer to function. If the
 * expression is an array, convert it to pointer to first element. Otherwise
 * return T.
 */



/* End of datatype.h */

#endif



