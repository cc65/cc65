/*****************************************************************************/
/*                                                                           */
/*				   objcode.h				     */
/*                                                                           */
/*	       Objectcode management for the ca65 macroassembler	     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2001 Ullrich von Bassewitz                                       */
/*               Wacholderweg 14                                             */
/*               D-70597 Stuttgart                                           */
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



/* common */
#include "segdefs.h"

/* ca65 */
#include "expr.h"



/*****************************************************************************/
/*				     Data				     */
/*****************************************************************************/



/* Are we in absolute mode or in relocatable mode? */
extern int 	RelocMode;

/* Definitions for predefined segments */
extern SegDef NullSegDef;
extern SegDef ZeropageSegDef;
extern SegDef DataSegDef;
extern SegDef BssSegDef;
extern SegDef RODataSegDef;
extern SegDef CodeSegDef;



/*****************************************************************************/
/*		 	      Segment management			     */
/*****************************************************************************/



void UseSeg (const SegDef* D);
/* Use the given segment */

const SegDef* GetCurrentSeg (void);
/* Get a pointer to the segment defininition of the current segment */

unsigned GetSegNum (void);
/* Get the number of the current segment */

void SegAlign (unsigned Power, int Val);
/* Align the PC segment to 2^Power. If Val is -1, emit fill fragments (the
 * actual fill value will be determined by the linker), otherwise use the
 * given value.
 */

int IsZPSeg (void);
/* Return true if the current segment is a zeropage segment */

int IsFarSeg (void);
/* Return true if the current segment is a far segment */

unsigned GetSegType (unsigned SegNum);
/* Return the type of the segment with the given number */

unsigned long GetPC (void);
/* Get the program counter of the current segment */

void SetAbsPC (unsigned long AbsPC);
/* Set the program counter in absolute mode */

void SegCheck (void);
/* Check the segments for range and other errors */

void SegDump (void);
/* Dump the contents of all segments */

void WriteSegments (void);
/* Write the segment data to the object file */



/*****************************************************************************/
/*     	      	    	  	     Code				     */
/*****************************************************************************/



void Emit0 (unsigned char OPC);
/* Emit an instruction with a zero sized operand */

void Emit1 (unsigned char OPC, ExprNode* Value);
/* Emit an instruction with an one byte argument */

void Emit2 (unsigned char OPC, ExprNode* Value);
/* Emit an instruction with a two byte argument */

void Emit3 (unsigned char OPC, ExprNode* Expr);
/* Emit an instruction with a three byte argument */

void Emit3b (unsigned char OPC, ExprNode* Expr, ExprNode* Bank);
/* Emit an instruction with a three byte argument and separate bank */

void EmitSigned (ExprNode* Expr, unsigned Size);
/* Emit a signed expression with the given size */

void EmitPCRel (unsigned char OPC, ExprNode* Expr, unsigned Size);
/* Emit an opcode with a PC relative argument of one or two bytes */

void EmitData (const unsigned char* Data, unsigned Size);
/* Emit data into the current segment */

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



