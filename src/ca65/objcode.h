/*****************************************************************************/
/*                                                                           */
/*                                 objcode.h                                 */
/*                                                                           */
/*             Objectcode management for the ca65 macroassembler             */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2008 Ullrich von Bassewitz                                       */
/*               Roemerstrasse 52                                            */
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



#ifndef OBJCODE_H
#define OBJCODE_H



/* ca65 */
#include "expr.h"
#include "strbuf.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void Emit0 (unsigned char OPC);
/* Emit an instruction with a zero sized operand */

void Emit1 (unsigned char OPC, ExprNode* Value);
/* Emit an instruction with an one byte argument */

void Emit2 (unsigned char OPC, ExprNode* Value);
/* Emit an instruction with a two byte argument */

void Emit3 (unsigned char OPC, ExprNode* Expr);
/* Emit an instruction with a three byte argument */

void EmitSigned (ExprNode* Expr, unsigned Size);
/* Emit a signed expression with the given size */

void EmitPCRel (unsigned char OPC, ExprNode* Expr, unsigned Size);
/* Emit an opcode with a PC relative argument of one or two bytes */

void EmitData (const void* Data, unsigned Size);
/* Emit data into the current segment */

void EmitStrBuf (const StrBuf* Data);
/* Emit a string into the current segment */

void EmitByte (ExprNode* Expr);
/* Emit one byte */

void EmitWord (ExprNode* Expr);
/* Emit one word */

void EmitFarAddr (ExprNode* Expr);
/* Emit a 24 bit expression */

void EmitDWord (ExprNode* Expr);
/* Emit one dword */

void EmitFill (unsigned long Count);
/* Emit Count fill bytes */



/* End of objcode.h */

#endif
