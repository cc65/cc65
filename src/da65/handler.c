/*****************************************************************************/
/*                                                                           */
/*				   handler.c				     */
/*                                                                           */
/*		 Opcode handler functions for the disassembler		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000      Ullrich von Bassewitz                                       */
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



#include <stdarg.h>

/* common */
#include "xsprintf.h"

/* da65 */
#include "attrtab.h"
#include "code.h"
#include "error.h"
#include "global.h"
#include "opctable.h"
#include "output.h"
#include "handler.h"



/*****************************************************************************/
/*		       	       Helper functions				     */
/*****************************************************************************/



static void Mnemonic (const char* M)
/* Indent and output a mnemonic */
{
    Indent (MIndent);
    Output ("%s", M);
}



static void OneLine (const OpcDesc* D, const char* Arg, ...) attribute ((format(printf, 2, 3)));
static void OneLine (const OpcDesc* D, const char* Arg, ...)
/* Output one line with the given mnemonic and argument */
{
    char Buf [256];
    va_list ap;

    /* Mnemonic */
    Mnemonic (D->Mnemo);

    /* Argument */
    va_start (ap, Arg);
    xvsprintf (Buf, sizeof (Buf), Arg, ap);
    va_end (ap);
    Indent (AIndent);
    Output (Buf);

    /* Add the code stuff as comment */
    LineComment (PC, D->Size);

    /* End the line */
    LineFeed ();
}



static const char* GetAddrArg (const OpcDesc* D, unsigned Addr)
/* Return an address argument - a label if we have one, or the address itself */
{
    const char* Label = 0;
    if (D->LabelFlag & lfUseLabel) {
	Label = GetLabel (Addr);
    }
    if (Label) {
	return Label;
    } else {
	static char Buf [32];
	if (Addr < 0x100) {
	    xsprintf (Buf, sizeof (Buf), "$%02X", Addr);
	} else {
	    xsprintf (Buf, sizeof (Buf), "$%04X", Addr);
	}
	return Buf;
    }
}



static void GenerateLabel (const OpcDesc* D, unsigned Addr)
/* Generate a label in pass one if requested */
{
    if (Pass == 1 && !HaveLabel (Addr)) {
	if ((D->LabelFlag & lfGenLabel) != 0 ||
       	    ((D->LabelFlag & lfUseLabel) != 0 && Addr >= CodeStart && Addr <= CodeEnd)) {
	    AddLabel (Addr, atIntLabel, MakeLabelName (Addr));
	}
    }
}



/*****************************************************************************/
/*   	       	     	      	     Code				     */
/*****************************************************************************/



void OH_Accumulator (const OpcDesc* D)
{
    OneLine (D, "a");
}



void OH_Implicit (const OpcDesc* D)
{
    Mnemonic (D->Mnemo);
    LineComment (PC, D->Size);
    LineFeed ();
}



void OH_Immidiate (const OpcDesc* D)
{
    OneLine (D, "#$%02X", GetCodeByte (PC+1));
}



void OH_Direct (const OpcDesc* D)
{
    /* Get the operand */
    unsigned Addr = GetCodeByte (PC+1);

    /* Generate a label in pass 1 */
    GenerateLabel (D, Addr);

    /* Output the line */
    OneLine (D, "%s", GetAddrArg (D, Addr));
}



void OH_DirectX (const OpcDesc* D)
{
    /* Get the operand */
    unsigned Addr = GetCodeByte (PC+1);

    /* Generate a label in pass 1 */
    GenerateLabel (D, Addr);

    /* Output the line */
    OneLine (D, "%s,y", GetAddrArg (D, Addr));
}



void OH_DirectY (const OpcDesc* D)
{
    /* Get the operand */
    unsigned Addr = GetCodeByte (PC+1);

    /* Generate a label in pass 1 */
    GenerateLabel (D, Addr);

    /* Output the line */
    OneLine (D, "%s,y", GetAddrArg (D, Addr));
}



void OH_Absolute (const OpcDesc* D)
{
    /* Get the operand */
    unsigned Addr = GetCodeWord (PC+1);

    /* Generate a label in pass 1 */
    GenerateLabel (D, Addr);

    /* Output the line */
    OneLine (D, "%s", GetAddrArg (D, Addr));
}



void OH_AbsoluteX (const OpcDesc* D)
{
    /* Get the operand */
    unsigned Addr = GetCodeWord (PC+1);

    /* Generate a label in pass 1 */
    GenerateLabel (D, Addr);

    /* Output the line */
    OneLine (D, "%s,x", GetAddrArg (D, Addr));
}



void OH_AbsoluteY (const OpcDesc* D)
{
    /* Get the operand */
    unsigned Addr = GetCodeWord (PC+1);

    /* Generate a label in pass 1 */
    GenerateLabel (D, Addr);

    /* Output the line */
    OneLine (D, "%s,y", GetAddrArg (D, Addr));
}



void OH_AbsoluteLong (const OpcDesc* D)
{
    Error ("Not implemented");
}



void OH_AbsoluteLongX (const OpcDesc* D)
{
    Error ("Not implemented");
}



void OH_Relative (const OpcDesc* D)
{
    /* Get the operand */
    signed char Offs = GetCodeByte (PC+1);

    /* Calculate the target address */
    unsigned Addr = (unsigned) (((int) PC+2) + Offs);

    /* Generate a label in pass 1 */
    GenerateLabel (D, Addr);

    /* Output the line */
    OneLine (D, "%s", GetAddrArg (D, Addr));
}



void OH_RelativeLong (const OpcDesc* D)
{
    Error ("Not implemented");
}



void OH_DirectIndirect (const OpcDesc* D)
{
    /* Get the operand */
    unsigned Addr = GetCodeByte (PC+1);

    /* Generate a label in pass 1 */
    GenerateLabel (D, Addr);

    /* Output the line */
    OneLine (D, "(%s)", GetAddrArg (D, Addr));
}



void OH_DirectIndirectY (const OpcDesc* D)
{
    /* Get the operand */
    unsigned Addr = GetCodeByte (PC+1);

    /* Generate a label in pass 1 */
    GenerateLabel (D, Addr);

    /* Output the line */
    OneLine (D, "(%s),y", GetAddrArg (D, Addr));
}



void OH_DirectXIndirect (const OpcDesc* D)
{
    /* Get the operand */
    unsigned Addr = GetCodeByte (PC+1);

    /* Generate a label in pass 1 */
    GenerateLabel (D, Addr);

    /* Output the line */
    OneLine (D, "(%s,x)", GetAddrArg (D, Addr));
}



void OH_AbsoluteIndirect (const OpcDesc* D)
{
    /* Get the operand */
    unsigned Addr = GetCodeWord (PC+1);

    /* Generate a label in pass 1 */
    GenerateLabel (D, Addr);

    /* Output the line */
    OneLine (D, "(%s)", GetAddrArg (D, Addr));
}



void OH_StackRelative (const OpcDesc* D)
{
    Error ("Not implemented");
}



void OH_DirectIndirectLongX (const OpcDesc* D)
{
    Error ("Not implemented");
}



void OH_StackRelativeIndirectY (const OpcDesc* D)
{
    Error ("Not implemented");
}



void OH_DirectIndirectLong (const OpcDesc* D)
{
    Error ("Not implemented");
}



void OH_DirectIndirectLongY (const OpcDesc* D)
{
    Error ("Not implemented");
}



void OH_BlockMove (const OpcDesc* D)
{
    Error ("Not implemented");
}



void OH_AbsoluteXIndirect (const OpcDesc* D)
{
    Error ("Not implemented");
}



void OH_Rts (const OpcDesc* D)
{
    OH_Implicit (D);
    SeparatorLine();
}



void OH_JmpAbsolute (const OpcDesc* D)
{
    OH_Absolute (D);
    SeparatorLine ();
}



void OH_JmpAbsoluteIndirect (const OpcDesc* D)
{
    OH_AbsoluteIndirect (D);
    SeparatorLine ();
}




