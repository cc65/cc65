/*****************************************************************************/
/*                                                                           */
/*				  datatype.c				     */
/*                                                                           */
/*		 Type string handling for the cc65 C compiler		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2000 Ullrich von Bassewitz                                       */
/*               Wacholderweg 14                                             */
/*               D-70597 Stuttgart                                           */
/* EMail:        uz@musoftware.de                                            */
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
#include "check.h"
#include "xmalloc.h"

/* cc65 */
#include "codegen.h"
#include "datatype.h"
#include "error.h"
#include "funcdesc.h"
#include "global.h"
#include "util.h"
#include "symtab.h"



/*****************************************************************************/
/*		    	 	     Data	    			     */
/*****************************************************************************/



/* Predefined type strings */
type type_int []	= { T_INT,	T_END };
type type_uint []   	= { T_UINT,	T_END };
type type_long []   	= { T_LONG,	T_END };
type type_ulong []  	= { T_ULONG,	T_END };
type type_void []   	= { T_VOID,	T_END };
type type_pschar []	= { T_PTR, T_SCHAR, T_END };
type type_puchar []	= { T_PTR, T_UCHAR, T_END };



/*****************************************************************************/
/*		 		     Code				     */
/*****************************************************************************/



unsigned TypeLen (const type* T)
/* Return the length of the type string */
{
    const type* Start = T;
    while (*T) {
	++T;
    }
    return T - Start;
}



int TypeCmp (const type* T1, const type* T2)
/* Compare two type strings */
{
    int A, B, D;
    do {
	A = *T1++;
	B = *T2++;
	D = A - B;
    } while (D == 0 && A != 0);
    return D;
}



type* TypeCpy (type* Dest, const type* Src)
/* Copy a type string */
{
    type T;
    type* Orig = Dest;
    do {
	T = *Src++;
	*Dest++ = T;
    } while (T);
    return Orig;
}



type* TypeCat (type* Dest, const type* Src)
/* Append Src */
{
    TypeCpy (Dest + TypeLen (Dest), Src);
    return Dest;
}



type* TypeDup (const type* T)
/* Create a copy of the given type on the heap */
{
    unsigned Len = (TypeLen (T) + 1) * sizeof (type);
    return memcpy (xmalloc (Len), T, Len);
}



type* TypeAlloc (unsigned Len)
/* Allocate memory for a type string of length Len. Len *must* include the
 * trailing T_END.
 */
{
    return xmalloc (Len * sizeof (type));
}



void TypeFree (type* T)
/* Free a type string */
{
    xfree (T);
}



type GetDefaultChar (void)
/* Return the default char type (signed/unsigned) depending on the settings */
{
    return SignedChars? T_SCHAR : T_UCHAR;
}



type* GetCharArrayType (unsigned Len)
/* Return the type for a char array of the given length */
{
    /* Allocate memory for the type string */
    type* T = TypeAlloc (1 + DECODE_SIZE + 2);

    /* Fill the type string */
    T [0] 	      = T_ARRAY;
    T [DECODE_SIZE+1] = GetDefaultChar();
    T [DECODE_SIZE+2] = T_END;

    /* Encode the length in the type string */
    Encode (T+1, Len);

    /* Return the new type */
    return T;
}



type* GetImplicitFuncType (void)
/* Return a type string for an inplicitly declared function */
{
    /* Get a new function descriptor */
    FuncDesc* F = NewFuncDesc ();

    /* Allocate memory for the type string */
    type* T = TypeAlloc (1 + DECODE_SIZE + 2);

    /* Prepare the function descriptor */
    F->Flags  = FD_IMPLICIT | FD_EMPTY | FD_ELLIPSIS;
    F->SymTab = &EmptySymTab;
    F->TagTab = &EmptySymTab;

    /* Fill the type string */
    T [0]    	      = T_FUNC;
    T [DECODE_SIZE+1] = T_INT;
    T [DECODE_SIZE+2] = T_END;

    /* Encode the function descriptor into the type string */
    EncodePtr (T+1, F);

    /* Return the new type */
    return T;
}



static type PrintTypeComp (FILE* F, type T, type Mask, const char* Name)
/* Check for a specific component of the type. If it is there, print the
 * name and remove it. Return the type with the component removed.
 */
{
    if ((T & Mask) == Mask) {
	fprintf (F, "%s ", Name);
	T &= ~Mask;
    }
    return T;
}



void PrintType (FILE* F, const type* Type)
/* Output translation of type array. */
{
    /* If the first field has const and/or volatile qualifiers, print and
     * remove them.
     */
    type T = *Type++;
    T = PrintTypeComp (F, T, T_QUAL_CONST, "const");
    T = PrintTypeComp (F, T, T_QUAL_VOLATILE, "volatile");

    /* Walk over the complete string */
    do {

     	/* Check for the sizes */
       	T = PrintTypeComp (F, T, T_SIZE_SHORT, "short");
	T = PrintTypeComp (F, T, T_SIZE_LONG, "long");
	T = PrintTypeComp (F, T, T_SIZE_LONGLONG, "long long");

	/* Signedness */
	T = PrintTypeComp (F, T, T_SIGN_SIGNED, "signed");
	T = PrintTypeComp (F, T, T_SIGN_UNSIGNED, "unsigned");

	/* Now check the real type */
     	switch (T & T_MASK_TYPE) {
	    case T_TYPE_CHAR:
	  	fprintf (F, "char\n");
	  	break;
	    case T_TYPE_INT:
	  	fprintf (F, "int\n");
	  	break;
	    case T_TYPE_FLOAT:
	     	fprintf (F, "float\n");
	     	break;
	    case T_TYPE_DOUBLE:
	     	fprintf (F, "double\n");
	     	break;
     	    case T_TYPE_VOID:
     	  	fprintf (F, "void\n");
	  	break;
	    case T_TYPE_STRUCT:
	     	fprintf (F, "struct %s\n", ((SymEntry*) DecodePtr (Type))->Name);
	     	Type += DECODE_SIZE;
	     	break;
	    case T_TYPE_UNION:
	     	fprintf (F, "union %s\n", ((SymEntry*) DecodePtr (Type))->Name);
	     	Type += DECODE_SIZE;
	     	break;
	    case T_TYPE_ARRAY:
       	       	fprintf (F, "array[%lu] of ", Decode (Type));
	     	Type += DECODE_SIZE;
	     	break;
	    case T_TYPE_PTR:
	     	fprintf (F, "pointer to ");
	     	break;
	    case T_TYPE_FUNC:
	     	fprintf (F, "function returning ");
	     	Type += DECODE_SIZE;
	     	break;
	    default:
	     	fprintf (F, "unknown type: %04X\n", T);
	}

	/* Get the next type element */
	T = *Type++;

    } while (T != T_END);
}



void PrintRawType (FILE* F, const type* Type)
/* Print a type string in raw format (for debugging) */
{
    while (*Type != T_END) {
       	fprintf (F, "%04X ", *Type++);
    }
    fprintf (F, "\n");
}



void Encode (type* Type, unsigned long Val)
/* Encode p[0] and p[1] so that neither p[0] nore p[1] is zero */
{
    int I;
    for (I = 0; I < DECODE_SIZE; ++I) {
	*Type++ = ((type) Val) | 0x8000;
	Val >>= 15;
    }
}



void EncodePtr (type* Type, void* P)
/* Encode a pointer into a type array */
{
    Encode (Type, (unsigned long) P);
}



unsigned long Decode (const type* Type)
/* Decode */
{
    int I;
    unsigned long Val = 0;
    for (I = DECODE_SIZE-1; I >= 0; I--) {
	Val <<= 15;
	Val |= (Type[I] & 0x7FFF);
    }
    return Val;
}



void* DecodePtr (const type* Type)
/* Decode a pointer from a type array */
{
    return (void*) Decode (Type);
}



int HasEncode (const type* Type)
/* Return true if the given type has encoded data */
{
    return IsStruct (Type) || IsArray (Type) || IsFunc (Type);
}



void CopyEncode (const type* Source, type* Target)
/* Copy encoded data from Source to Target */
{
    memcpy (Target, Source, DECODE_SIZE * sizeof (type));
}



unsigned SizeOf (const type* T)
/* Compute size of object represented by type array. */
{
    SymEntry* Entry;

    switch (*T) {

	case T_VOID:
	    Error (ERR_ILLEGAL_SIZE);
	    return 0;

	case T_SCHAR:
	case T_UCHAR:
	    return 1;

       	case T_SHORT:
    	case T_USHORT:
    	case T_INT:
	case T_UINT:
	case T_PTR:
	    return 2;

        case T_LONG:
    	case T_ULONG:
	    return 4;

	case T_LONGLONG:
	case T_ULONGLONG:
	    return 8;

        case T_ENUM:
	    return 2;

	case T_FLOAT:
	case T_DOUBLE:
	    return 4;

	case T_STRUCT:
	case T_UNION:
       	    Entry = DecodePtr (T+1);
       	    return Entry->V.S.Size;

	case T_ARRAY:
	    return (Decode (T+ 1) * SizeOf (T + DECODE_SIZE + 1));

    	default:
	    Internal ("Unknown type in SizeOf: %04X", *T);
	    return 0;

    }
}



unsigned PSizeOf (const type* T)
/* Compute size of pointer object. */
{
    /* We are expecting a pointer expression */
    CHECK ((*T & T_CLASS_PTR) != 0);

    /* Skip the pointer or array token itself */
    if (*T == T_ARRAY) {
       	return SizeOf (T + DECODE_SIZE + 1);
    } else {
      	return SizeOf (T + 1);
    }
}



unsigned TypeOf (const type* Type)
/* Get the code generator base type of the object */
{
    FuncDesc* F;

    switch (*Type) {

	case T_SCHAR:
    	    return CF_CHAR;

    	case T_UCHAR:
    	    return CF_CHAR | CF_UNSIGNED;

    	case T_SHORT:
	case T_INT:
        case T_ENUM:
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

        case T_FUNC:
	    F = DecodePtr (Type+1);
	    return (F->Flags & FD_ELLIPSIS)? 0 : CF_FIXARGC;

        case T_STRUCT:
        case T_UNION:
       	    /* Address of ... */
       	    return CF_INT | CF_UNSIGNED;

       	default:
       	    Error (ERR_ILLEGAL_TYPE);
       	    return CF_INT;
    }
}



type* Indirect (type* T)
/* Do one indirection for the given type, that is, return the type where the
 * given type points to.
 */
{
    /* We are expecting a pointer expression */
    CHECK ((*T & T_MASK_CLASS) == T_CLASS_PTR);

    /* Skip the pointer or array token itself */
    if (*T == T_ARRAY) {
       	return T + DECODE_SIZE + 1;
    } else {
      	return T + 1;
    }
}



int IsTypeVoid (const type* T)
/* Return true if this is a void type */
{
    return (T[0] == T_VOID && T[1] == T_END);
}



int IsPtr (const type* T)
/* Return true if this is a pointer type */
{
    return (T[0] & T_MASK_CLASS) == T_CLASS_PTR;
}



int IsChar (const type* T)
/* Return true if this is a character type */
{
    return (T[0] & T_MASK_TYPE) == T_TYPE_CHAR && T[1] == T_END;
}



int IsInt (const type* T)
/* Return true if this is an integer type */
{
    return (T[0] & T_MASK_CLASS) == T_CLASS_INT;
}



int IsLong (const type* T)
/* Return true if this is a long type (signed or unsigned) */
{
    return (T[0] & T_MASK_SIZE) == T_SIZE_LONG;
}



int IsUnsigned (const type* T)
/* Return true if this is an unsigned type */
{
    return (T[0] & T_MASK_SIGN) == T_SIGN_UNSIGNED;
}



int IsStruct (const type* T)
/* Return true if this is a struct type */
{
    return (T[0] & T_MASK_CLASS) == T_CLASS_STRUCT;
}



int IsFunc (const type* T)
/* Return true if this is a function type */
{
    return (T[0] == T_FUNC);
}



int IsFastCallFunc (const type* T)
/* Return true if this is a function type with __fastcall__ calling conventions */
{
    FuncDesc* F;
    CHECK (T[0] == T_FUNC);
    F = DecodePtr (T+1);
    return (F->Flags & FD_FASTCALL) != 0;
}



int IsFuncPtr (const type* T)
/* Return true if this is a function pointer */
{
    return (T[0] == T_PTR && T[1] == T_FUNC);
}



int IsArray (const type* T)
/* Return true if this is an array type */
{
    return (T[0] == T_ARRAY);
}



struct FuncDesc* GetFuncDesc (const type* T)
/* Get the FuncDesc pointer from a function or pointer-to-function type */
{
    if (T[0] == T_PTR) {
	/* Pointer to function */
	++T;
    }

    /* Be sure it's a function type */
    CHECK (T[0] == T_FUNC);

    /* Decode the function descriptor and return it */
    return DecodePtr (T+1);
}




