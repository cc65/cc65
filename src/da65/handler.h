/*****************************************************************************/
/*                                                                           */
/*                                 handler.h                                 */
/*                                                                           */
/*               Opcode handler functions for the disassembler               */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2011, Ullrich von Bassewitz                                      */
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



#ifndef HANDLER_H
#define HANDLER_H



/* common */
#include "attrib.h"

/* da65 */
#include "opctable.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



/* Generic handlers */
void OH_Illegal (const OpcDesc* D attribute ((unused)));
void OH_Accumulator (const OpcDesc*);
void OH_Implicit (const OpcDesc*);
void OH_Immediate (const OpcDesc*);
void OH_Direct (const OpcDesc*);
void OH_DirectX (const OpcDesc*);
void OH_DirectY (const OpcDesc*);
void OH_Absolute (const OpcDesc*);
void OH_AbsoluteX (const OpcDesc*);
void OH_AbsoluteY (const OpcDesc*);
void OH_AbsoluteLong (const OpcDesc*);
void OH_AbsoluteLongX (const OpcDesc*);
void OH_Relative (const OpcDesc*);
void OH_RelativeLong (const OpcDesc*);
void OH_DirectIndirect (const OpcDesc*);
void OH_DirectIndirectY (const OpcDesc*);
void OH_DirectXIndirect (const OpcDesc*);
void OH_AbsoluteIndirect (const OpcDesc*);

void OH_BitBranch (const OpcDesc*);

void OH_ImmediateDirect (const OpcDesc*);
void OH_ImmediateDirectX (const OpcDesc*);
void OH_ImmediateAbsolute (const OpcDesc*);
void OH_ImmediateAbsoluteX (const OpcDesc*);

void OH_StackRelative (const OpcDesc*);
void OH_DirectIndirectLongX (const OpcDesc*);
void OH_StackRelativeIndirectY (const OpcDesc*);
void OH_DirectIndirectLong (const OpcDesc*);
void OH_DirectIndirectLongY (const OpcDesc*);
void OH_BlockMove (const OpcDesc*);
void OH_AbsoluteXIndirect (const OpcDesc*);

/* Mitsubishi 740 */
void OH_DirectImmediate (const OpcDesc*);
void OH_ZeroPageBit (const OpcDesc*);
void OH_AccumulatorBit (const OpcDesc*);
void OH_AccumulatorBitBranch (const OpcDesc*);
void OH_JmpDirectIndirect (const OpcDesc* D);
void OH_SpecialPage (const OpcDesc*);
                    
/* Handlers for special instructions */
void OH_Rts (const OpcDesc*);
void OH_JmpAbsolute (const OpcDesc*);
void OH_JmpAbsoluteIndirect (const OpcDesc* D);



/* End of handler.h */

#endif
