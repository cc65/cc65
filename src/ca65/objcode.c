/*****************************************************************************/
/*                                                                           */
/*				   objcode.c				     */
/*                                                                           */
/*	       Objectcode management for the ca65 macroassembler	     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2003 Ullrich von Bassewitz                                       */
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



#include <string.h>
#include <errno.h>

/* cc65 */
#include "error.h"
#include "fragment.h"
#include "objcode.h"
#include "segment.h"



/*****************************************************************************/
/*     	      	      	   	     Code				     */
/*****************************************************************************/



void Emit0 (unsigned char OPC)
/* Emit an instruction with a zero sized operand */
{
    Fragment* F = GenFragment (FRAG_LITERAL, 1);
    F->V.Data [0] = OPC;
}



void Emit1 (unsigned char OPC, ExprNode* Value)
/* Emit an instruction with an one byte argument */
{
    Emit0 (OPC);
    EmitByte (Value);
}



void Emit2 (unsigned char OPC, ExprNode* Value)
/* Emit an instruction with a two byte argument */
{
    Emit0 (OPC);
    EmitWord (Value);
}



void Emit3 (unsigned char OPC, ExprNode* Expr)
/* Emit an instruction with a three byte argument */
{
    Emit0 (OPC);
    EmitFarAddr (Expr);
}



void Emit3b (unsigned char OPC, ExprNode* Expr, ExprNode* Bank)
/* Emit an instruction with a three byte argument and separate bank */
{
    Emit0 (OPC);
    EmitWord (Expr);
    EmitByte (Bank);
}



void EmitSigned (ExprNode* Expr, unsigned Size)
/* Emit a signed expression with the given size */
{
    Fragment* F = GenFragment (FRAG_SEXPR, Size);
    F->V.Expr = Expr;
}



void EmitPCRel (unsigned char OPC, ExprNode* Expr, unsigned Size)
/* Emit an opcode with a PC relative argument of one or two bytes */
{
    Emit0 (OPC);
    EmitSigned (Expr, Size);
}



void EmitData (const unsigned char* Data, unsigned Size)
/* Emit data into the current segment */
{
    /* Create lots of fragments for the data */
    while (Size) {
	Fragment* F;

	/* Determine the length of the next fragment */
	unsigned Len = Size;
       	if (Len > sizeof (F->V.Data)) {
     	    Len = sizeof (F->V.Data);
       	}

     	/* Create a new fragment */
     	F = GenFragment (FRAG_LITERAL, Len);

     	/* Copy the data */
     	memcpy (F->V.Data, Data, Len);

     	/* Next chunk */
     	Data += Len;
     	Size -= Len;

    }
}



void EmitByte (ExprNode* Expr)
/* Emit one byte */
{
    if (IsConstExpr (Expr)) {
     	/* Constant expression, emit literal byte */
     	long Val = GetExprVal (Expr);
	FreeExpr (Expr);
     	if ((Val & ~0xFF) != 0) {
     	    Error (ERR_RANGE);
     	}
     	Emit0 (Val & 0xFF);
    } else {
     	/* Create a new fragment */
     	Fragment* F = GenFragment (FRAG_EXPR, 1);

     	/* Set the data */
     	F->V.Expr = Expr;
    }
}



void EmitWord (ExprNode* Expr)
/* Emit one word */
{
    if (IsConstExpr (Expr)) {
     	/* Constant expression, emit literal byte */
     	long Val = GetExprVal (Expr);
	FreeExpr (Expr);
       	if ((Val & ~0xFFFF) != 0) {
     	    Error (ERR_RANGE);
     	}
     	Emit0 (Val & 0xFF);
	Emit0 ((Val >> 8) & 0xFF);
    } else {
     	/* Create a new fragment */
     	Fragment* F = GenFragment (FRAG_EXPR, 2);

     	/* Set the data */
     	F->V.Expr = Expr;
    }
}



void EmitFarAddr (ExprNode* Expr)
/* Emit a 24 bit expression */
{
    if (IsConstExpr (Expr)) {
     	/* Constant expression, emit literal byte */
     	long Val = GetExprVal (Expr);
	FreeExpr (Expr);
       	if ((Val & ~0xFFFFFF) != 0) {
     	    Error (ERR_RANGE);
     	}
     	Emit0 (Val & 0xFF);
	Emit0 ((Val >> 8) & 0xFF);
	Emit0 ((Val >> 16) & 0xFF);
    } else {
     	/* Create a new fragment */
     	Fragment* F = GenFragment (FRAG_EXPR, 3);

     	/* Set the data */
     	F->V.Expr = Expr;
    }
}



void EmitDWord (ExprNode* Expr)
/* Emit one dword */
{
    if (IsConstExpr (Expr)) {
     	/* Constant expression, emit literal byte */
     	long Val = GetExprVal (Expr);
	FreeExpr (Expr);
     	Emit0 (Val & 0xFF);
	Emit0 ((Val >> 8) & 0xFF);
       	Emit0 ((Val >> 16) & 0xFF);
	Emit0 ((Val >> 24) & 0xFF);
    } else {
     	/* Create a new fragment */
     	Fragment* F = GenFragment (FRAG_EXPR, 4);

     	/* Set the data */
     	F->V.Expr = Expr;
    }
}



void EmitFill (unsigned long Count)
/* Emit Count fill bytes */
{
    while (Count) {
	/* Calculate the size of the next chunk */
	unsigned Chunk = (Count > 0xFFFF)? 0xFFFF : (unsigned) Count;
	Count -= Chunk;

	/* Emit one chunk */
	GenFragment (FRAG_FILL, Chunk);
    }
}



