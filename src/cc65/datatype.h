/*****************************************************************************/
/*                                                                           */
/*				  datatype.h				     */
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



#ifndef DATATYPE_H
#define DATATYPE_H



#include <stdio.h>



/*****************************************************************************/
/*     	      	    	       	     Data   				     */
/*****************************************************************************/



/* Data types */
#define	T_END  	       	0x0000
#define T_CHAR 	       	0x0011
#define T_INT  	       	0x0012
#define	T_SHORT	       	0x0013
#define T_LONG 	       	0x0014
#define T_ENUM 	       	0x0015
#define T_UCHAR	       	0x0019
#define T_UINT 	       	0x001A
#define T_USHORT       	0x001B
#define T_ULONG	       	0x001C

#define T_FLOAT	       	0x0025
#define T_DOUBLE       	0x0026

#define T_VOID 	       	0x0001		/* void parameter list */
#define T_FUNC 	       	0x0002		/* Function */

#define T_UNSIGNED     	0x0008		/* Class */
#define T_INTEGER      	0x0010		/* Class */
#define T_REAL 	       	0x0020		/* Class */
#define T_POINTER      	0x0040		/* Class */
#define T_PTR  	       	0x0049
#define T_ARRAY	       	0x004A
#define T_STRUCT       	0x0080
#define T_UNION	       	0x0081
#define T_SMASK	       	0x003F



/* Forward for a symbol entry */
struct SymEntry;

/* Type entry */
typedef unsigned short type;

/* Maximum length of a type string */
#define MAXTYPELEN   	30

/* type elements needed for Encode/Decode */
#define DECODE_SIZE    	5

/* Predefined type strings */
extern type type_int [];
extern type type_uint [];
extern type type_long [];
extern type type_ulong [];
extern type type_void [];
extern type type_pschar [];
extern type type_puchar [];



/*****************************************************************************/
/*     	      	      	       	     Code				     */
/*****************************************************************************/



unsigned TypeLen (const type* Type);
/* Return the length of the type string */

int TypeCmp (const type* T1, const type* T2);
/* Compare two type strings */

type* TypeCpy (type* Dest, const type* Src);
/* Copy a type string */

type* TypeCat (type* Dest, const type* Src);
/* Append Src */

type* TypeDup (const type* Type);
/* Create a copy of the given type on the heap */

type* TypeAlloc (unsigned Len);
/* Allocate memory for a type string of length Len. Len *must* include the
 * trailing T_END.
 */

void TypeFree (type* Type);
/* Free a type string */

type GetDefaultChar (void);
/* Return the default char type (signed/unsigned) depending on the settings */

type* GetCharArrayType (unsigned Len);
/* Return the type for a char array of the given length */

type* GetImplicitFuncType (void);
/* Return a type string for an inplicitly declared function */

void PrintType (FILE* F, const type* Type);
/* Output translation of type array. */

void PrintRawType (FILE* F, const type* Type);
/* Print a type string in raw format (for debugging) */

void Encode (type* Type, unsigned long Val);
/* Encode an unsigned long into a type array */

void EncodePtr (type* Type, void* P);
/* Encode a pointer into a type array */

unsigned long Decode (const type* Type);
/* Decode an unsigned long from a type array */

void* DecodePtr (const type* Type);
/* Decode a pointer from a type array */

int HasEncode (const type* Type);
/* Return true if the given type has encoded data */
    
void CopyEncode (const type* Source, type* Target);
/* Copy encoded data from Source to Target */

unsigned SizeOf (const type* Type);
/* Compute size of object represented by type array. */

unsigned PSizeOf (const type* Type);
/* Compute size of pointer object. */

unsigned TypeOf (const type* Type);
/* Get the code generator base type of the object */

type* Indirect (type* Type);
/* Do one indirection for the given type, that is, return the type where the
 * given type points to.
 */

int IsVoid (const type* Type);
/* Return true if this is a void type */

int IsPtr (const type* Type);
/* Return true if this is a pointer type */

int IsChar (const type* Type);
/* Return true if this is a character type */

int IsInt (const type* Type);
/* Return true if this is an integer type */

int IsLong (const type* Type);
/* Return true if this is a long type (signed or unsigned) */

int IsUnsigned (const type* Type);
/* Return true if this is an unsigned type */

int IsStruct (const type* Type);
/* Return true if this is a struct type */

int IsFunc (const type* Type);
/* Return true if this is a function type */

int IsFastCallFunc (const type* Type);
/* Return true if this is a function type with __fastcall__ calling conventions */

int IsFuncPtr (const type* Type);
/* Return true if this is a function pointer */

int IsArray (const type* Type);
/* Return true if this is an array type */

struct FuncDesc* GetFuncDesc (const type* Type);
/* Get the FuncDesc pointer from a function or pointer-to-function type */



/* End of datatype.h */

#endif



