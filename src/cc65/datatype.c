/*****************************************************************************/
/*                                                                           */
/*				  datatype.c				     */
/*                                                                           */
/*		 Type string handling for the cc65 C compiler		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998     Ullrich von Bassewitz                                        */
/*              Wacholderweg 14                                              */
/*              D-70597 Stuttgart                                            */
/* EMail:       uz@musoftware.de                                             */
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

#include "check.h"
#include "codegen.h"
#include "datatype.h"
#include "error.h"
#include "funcdesc.h"
#include "global.h"
#include "mem.h"
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
type type_pschar []	= { T_PTR, T_CHAR, T_END };
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
    return SignedChars? T_CHAR : T_UCHAR;
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



void PrintType (FILE* F, const type* tarray)
/* Output translation of type array. */
{
    const type* p;

    for (p = tarray; *p != T_END; ++p) {
	if (*p & T_UNSIGNED) {
	    fprintf (F, "unsigned ");
	}
	switch (*p) {
    	    case T_VOID:
	  	fprintf (F, "void\n");
	  	break;
	    case T_CHAR:
	    case T_UCHAR:
	  	fprintf (F, "char\n");
	  	break;
	    case T_INT:
	    case T_UINT:
	  	fprintf (F, "int\n");
	  	break;
	    case T_SHORT:
	    case T_USHORT:
	    	fprintf (F, "short\n");
	    	break;
	    case T_LONG:
	    case T_ULONG:
	     	fprintf (F, "long\n");
	     	break;
	    case T_FLOAT:
	     	fprintf (F, "float\n");
	     	break;
	    case T_DOUBLE:
	     	fprintf (F, "double\n");
	     	break;
	    case T_PTR:
	     	fprintf (F, "pointer to ");
	     	break;
	    case T_ARRAY:
       	       	fprintf (F, "array[%lu] of ", Decode (p + 1));
	     	p += DECODE_SIZE;
	     	break;
	    case T_STRUCT:
	     	fprintf (F, "struct %s\n", ((SymEntry*) Decode (p + 1))->Name);
	     	p += DECODE_SIZE;
	     	break;
	    case T_UNION:
	     	fprintf (F, "union %s\n", ((SymEntry*) Decode (p + 1))->Name);
	     	p += DECODE_SIZE;
	     	break;
	    case T_FUNC:
	     	fprintf (F, "function returning ");
	     	p += DECODE_SIZE;
	     	break;
	    default:
	     	fprintf (F, "unknown type: %04X\n", *p);
	}
    }
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



unsigned SizeOf (const type* tarray)
/* Compute size of object represented by type array. */
{
    SymEntry* Entry;

    switch (*tarray) {

	case T_VOID:
	    return 0;

	case T_CHAR:
	case T_UCHAR:
	    return 1;

    	case T_INT:
	case T_UINT:
       	case T_SHORT:
    	case T_USHORT:
	case T_PTR:
        case T_ENUM:
	    return 2;

        case T_LONG:
    	case T_ULONG:
	    return 4;

	case T_ARRAY:
	    return (Decode (tarray + 1) * SizeOf (tarray + DECODE_SIZE + 1));

	case T_STRUCT:
	case T_UNION:
       	    Entry = DecodePtr (tarray+1);
       	    return Entry->V.S.Size;

    	default:
	    Internal ("Unknown type: %04X", *tarray);
	    return 0;

    }
}



unsigned PSizeOf (const type* tptr)
/* Compute size of pointer object. */
{
    /* We are expecting a pointer expression */
    CHECK (*tptr & T_POINTER);

    /* Skip the pointer or array token itself */
    if (*tptr == T_ARRAY) {
       	return SizeOf (tptr + DECODE_SIZE + 1);
    } else {
      	return SizeOf (tptr + 1);
    }
}



unsigned TypeOf (const type* Type)
/* Get the code generator base type of the object */
{
    FuncDesc* F;

    switch (*Type) {

    	case T_CHAR:
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



type* Indirect (type* Type)
/* Do one indirection for the given type, that is, return the type where the
 * given type points to.
 */
{
    /* We are expecting a pointer expression */
    CHECK (Type[0] & T_POINTER);

    /* Skip the pointer or array token itself */
    if (Type[0] == T_ARRAY) {
       	return Type + DECODE_SIZE + 1;
    } else {
      	return Type + 1;
    }
}



int IsVoid (const type* Type)
/* Return true if this is a void type */
{
    return (Type[0] == T_VOID && Type[1] == T_END);
}



int IsPtr (const type* Type)
/* Return true if this is a pointer type */
{
    return (Type[0] & T_POINTER) != 0;
}



int IsChar (const type* Type)
/* Return true if this is a character type */
{
    return (Type[0] == T_CHAR || Type[0] == T_UCHAR) && Type[1] == T_END;
}



int IsInt (const type* Type)
/* Return true if this is an integer type */
{
    return (Type[0] & T_INTEGER) != 0;
}



int IsLong (const type* Type)
/* Return true if this is a long type (signed or unsigned) */
{
    return (Type[0] & T_LONG) == T_LONG;
}



int IsUnsigned (const type* Type)
/* Return true if this is an unsigned type */
{
    return (Type[0] & T_UNSIGNED) != 0;
}



int IsStruct (const type* Type)
/* Return true if this is a struct type */
{
    return (Type[0] == T_STRUCT || Type[0] == T_UNION);
}



int IsFunc (const type* Type)
/* Return true if this is a function type */
{
    return (Type[0] == T_FUNC);
}



int IsFastCallFunc (const type* Type)
/* Return true if this is a function type with __fastcall__ calling conventions */
{
    FuncDesc* F;
    CHECK (*Type == T_FUNC);
    F = DecodePtr (Type+1);
    return (F->Flags & FD_FASTCALL) != 0;
}



int IsFuncPtr (const type* Type)
/* Return true if this is a function pointer */
{
    return (Type[0] == T_PTR && Type[1] == T_FUNC);
}



int IsArray (const type* Type)
/* Return true if this is an array type */
{
    return (Type[0] == T_ARRAY);
}



struct FuncDesc* GetFuncDesc (const type* Type)
/* Get the FuncDesc pointer from a function or pointer-to-function type */
{
    if (Type[0] == T_PTR) {
	/* Pointer to function */
	++Type;
    }

    /* Be sure it's a function type */
    CHECK (Type[0] == T_FUNC);

    /* Decode the function descriptor and return it */
    return DecodePtr (Type+1);
}



