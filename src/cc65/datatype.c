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
/*		    	  	     Data	    			     */
/*****************************************************************************/



/* Predefined type strings */
type type_uchar []     	= { T_UCHAR,	T_END };
type type_int []	= { T_INT,	T_END };
type type_uint []   	= { T_UINT,	T_END };
type type_long []   	= { T_LONG,	T_END };
type type_ulong []  	= { T_ULONG,	T_END };
type type_void []   	= { T_VOID,	T_END };
type type_size_t []	= { T_UINT,     T_END };



/*****************************************************************************/
/*		 		     Code				     */
/*****************************************************************************/



unsigned TypeLen (const type* T)
/* Return the length of the type string */
{
    const type* Start = T;
    while (*T != T_END) {
	++T;
    }
    return T - Start;
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
    return (type*) memcpy (xmalloc (Len), T, Len);
}



type* TypeAlloc (unsigned Len)
/* Allocate memory for a type string of length Len. Len *must* include the
 * trailing T_END.
 */
{
    return (type*) xmalloc (Len * sizeof (type));
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
    F->Flags  = FD_IMPLICIT | FD_EMPTY | FD_VARIADIC;
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



type* PointerTo (const type* T)
/* Return a type string that is "pointer to T". The type string is allocated
 * on the heap and may be freed after use.
 */
{
    /* Get the size of the type string including the terminator */
    unsigned Size = TypeLen (T) + 1;

    /* Allocate the new type string */
    type* P = TypeAlloc	(Size + 1);

    /* Create the return type... */
    P[0] = T_PTR;
    memcpy (P+1, T, Size * sizeof (type));

    /* ...and return it */
    return P;
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
    type T;
    unsigned long Size;

    /* Walk over the complete string */
    while ((T = *Type++) != T_END) {

	/* Print any qualifiers */
    	T = PrintTypeComp (F, T, T_QUAL_CONST, "const");
	T = PrintTypeComp (F, T, T_QUAL_VOLATILE, "volatile");

    	/* Signedness. Omit the signedness specifier for long and int */
	if ((T & T_MASK_TYPE) != T_TYPE_INT && (T & T_MASK_TYPE) != T_TYPE_LONG) {
	    T = PrintTypeComp (F, T, T_SIGN_SIGNED, "signed");
	}
    	T = PrintTypeComp (F, T, T_SIGN_UNSIGNED, "unsigned");

    	/* Now check the real type */
     	switch (T & T_MASK_TYPE) {
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
    	     	fprintf (F, "struct %s", ((SymEntry*) DecodePtr (Type))->Name);
       	     	Type += DECODE_SIZE;
	     	break;
	    case T_TYPE_UNION:
	     	fprintf (F, "union %s", ((SymEntry*) DecodePtr (Type))->Name);
	     	Type += DECODE_SIZE;
	     	break;
	    case T_TYPE_ARRAY:
		/* Recursive call */
		PrintType (F, Type + DECODE_SIZE);
		Size = Decode (Type);
		if (Size == 0) {
		    fprintf (F, "[]");
		} else {
		    fprintf (F, "[%lu]", Size);
		}
	     	return;
	    case T_TYPE_PTR:
 		/* Recursive call */
		PrintType (F, Type);
		fprintf (F, "*");
	     	return;
	    case T_TYPE_FUNC:
	     	fprintf (F, "function returning ");
	     	Type += DECODE_SIZE;
	     	break;
	    default:
	     	fprintf (F, "unknown type: %04X", T);
	}

    }
}



void PrintFuncSig (FILE* F, const char* Name, type* Type)
/* Print a function signature. */
{
    /* Get the function descriptor */
    const FuncDesc* D = GetFuncDesc (Type);

    /* Print a comment with the function signature */
    PrintType (F, GetFuncReturn (Type));
    if (D->Flags & FD_FASTCALL) {
     	fprintf (F, " __fastcall__");
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
 	    PrintType (F, E->Type);
 	    E = E->NextSym;
 	}
    }

    /* End of parameter list */
    fprintf (F, ")");
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
    return IsClassStruct (Type) || IsTypeArray (Type) || IsTypeFunc (Type);
}



void CopyEncode (const type* Source, type* Target)
/* Copy encoded data from Source to Target */
{
    memcpy (Target, Source, DECODE_SIZE * sizeof (type));
}



type UnqualifiedType (type T)
/* Return the unqalified type */
{
    return (T & ~T_MASK_QUAL);
}



unsigned SizeOf (const type* T)
/* Compute size of object represented by type array. */
{
    SymEntry* Entry;

    switch (UnqualifiedType (T[0])) {

    	case T_VOID:
    	    Error ("Variable has unknown size");
	    return 1;	/* Return something that makes sense */

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
       	    Entry = (SymEntry*) DecodePtr (T+1);
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
    if (IsTypeArray (T)) {
       	return SizeOf (T + DECODE_SIZE + 1);
    } else {
      	return SizeOf (T + 1);
    }
}



unsigned TypeOf (const type* T)
/* Get the code generator base type of the object */
{
    FuncDesc* F;

    switch (UnqualifiedType (T[0])) {

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
	    F = (FuncDesc*) DecodePtr (T+1);
	    return (F->Flags & FD_VARIADIC)? 0 : CF_FIXARGC;

        case T_STRUCT:
        case T_UNION:
       	    /* Address of ... */
       	    return CF_INT | CF_UNSIGNED;

       	default:
       	    Error ("Illegal type");
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
    if (IsTypeArray (T)) {
       	return T + DECODE_SIZE + 1;
    } else {
      	return T + 1;
    }
}



int IsTypeChar (const type* T)
/* Return true if this is a character type */
{
    return (T[0] & T_MASK_TYPE) == T_TYPE_CHAR;
}



int IsTypeInt (const type* T)
/* Return true if this is an int type (signed or unsigned) */
{
    return (T[0] & T_MASK_TYPE) == T_TYPE_INT;
}



int IsTypeLong (const type* T)
/* Return true if this is a long type (signed or unsigned) */
{
    return (T[0] & T_MASK_TYPE) == T_TYPE_LONG;
}



int IsTypeFloat (const type* T)
/* Return true if this is a float type */
{
    return (T[0] & T_MASK_TYPE) == T_TYPE_FLOAT;
}



int IsTypeDouble (const type* T)
/* Return true if this is a double type */
{
    return (T[0] & T_MASK_TYPE) == T_TYPE_DOUBLE;
}



int IsTypePtr (const type* T)
/* Return true if this is a pointer type */
{
    return ((T[0] & T_MASK_TYPE) == T_TYPE_PTR);
}



int IsTypeArray (const type* T)
/* Return true if this is an array type */
{
    return ((T[0] & T_MASK_TYPE) == T_TYPE_ARRAY);
}



int IsTypeVoid (const type* T)
/* Return true if this is a void type */
{
    return (T[0] & T_MASK_TYPE) == T_TYPE_VOID;
}



int IsTypeFunc (const type* T)
/* Return true if this is a function class */
{
    return ((T[0] & T_MASK_TYPE) == T_TYPE_FUNC);
}



int IsClassInt (const type* T)
/* Return true if this is an integer type */
{
    return (T[0] & T_MASK_CLASS) == T_CLASS_INT;
}



int IsClassFloat (const type* T)
/* Return true if this is a float type */
{
    return (T[0] & T_MASK_CLASS) == T_CLASS_FLOAT;
}



int IsClassPtr (const type* T)
/* Return true if this is a pointer type */
{
    return (T[0] & T_MASK_CLASS) == T_CLASS_PTR;
}



int IsClassStruct (const type* T)
/* Return true if this is a struct type */
{
    return (T[0] & T_MASK_CLASS) == T_CLASS_STRUCT;
}



int IsSignUnsigned (const type* T)
/* Return true if this is an unsigned type */
{
    return (T[0] & T_MASK_SIGN) == T_SIGN_UNSIGNED;
}



int IsQualConst (const type* T)
/* Return true if the given type has a const memory image */
{
    return (GetQualifier (T) & T_QUAL_CONST) != 0;
}



int IsQualVolatile (const type* T)
/* Return true if the given type has a volatile type qualifier */
{
    return (GetQualifier (T) & T_QUAL_VOLATILE) != 0;
}



int IsFastCallFunc (const type* T)
/* Return true if this is a function type or pointer to function with
 * __fastcall__ calling conventions
 */
{
    FuncDesc* F	= GetFuncDesc (T);
    return (F->Flags & FD_FASTCALL) != 0;
}



int IsVariadicFunc (const type* T)
/* Return true if this is a function type or pointer to function type with
 * variable parameter list
 */
{
    FuncDesc* F = GetFuncDesc (T);
    return (F->Flags & FD_VARIADIC) != 0;
}



int IsTypeFuncPtr (const type* T)
/* Return true if this is a function pointer */
{
    return ((T[0] & T_MASK_TYPE) == T_TYPE_PTR && (T[1] & T_MASK_TYPE) == T_TYPE_FUNC);
}



type GetType (const type* T)
/* Get the raw type */
{
    PRECONDITION (T[0] != T_END);
    return (T[0] & T_MASK_TYPE);
}



type GetClass (const type* T)
/* Get the class of a type string */
{
    PRECONDITION (T[0] != T_END);
    return (T[0] & T_MASK_CLASS);
}



type GetSignedness (const type* T)
/* Get the sign of a type */
{
    PRECONDITION (T[0] != T_END);
    return (T[0] & T_MASK_SIGN);
}



type GetSizeModifier (const type* T)
/* Get the size modifier of a type */
{
    PRECONDITION (T[0] != T_END);
    return (T[0] & T_MASK_SIZE);
}



type GetQualifier (const type* T)
/* Get the qualifier from the given type string */
{
    /* If this is an array, look at the element type, otherwise look at the
     * type itself.
     */
    if (IsTypeArray (T)) {
    	T += DECODE_SIZE + 1;
    }
    return (T[0] & T_QUAL_CONST);
}



FuncDesc* GetFuncDesc (const type* T)
/* Get the FuncDesc pointer from a function or pointer-to-function type */
{
    if (T[0] == T_PTR) {
	/* Pointer to function */
	++T;
    }

    /* Be sure it's a function type */
    CHECK (T[0] == T_FUNC);

    /* Decode the function descriptor and return it */
    return (FuncDesc*) DecodePtr (T+1);
}



type* GetFuncReturn (type* T)
/* Return a pointer to the return type of a function or pointer-to-function type */
{
    if (T[0] == T_PTR) {
	/* Pointer to function */
	++T;
    }

    /* Be sure it's a function type */
    CHECK (T[0] == T_FUNC);

    /* Return a pointer to the return type */
    return T + 1 + DECODE_SIZE;

}



