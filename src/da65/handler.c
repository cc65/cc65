/*****************************************************************************/
/*                                                                           */
/*                                 handler.c                                 */
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



#include <stdarg.h>

/* common */
#include "xsprintf.h"

/* da65 */
#include "attrtab.h"
#include "code.h"
#include "error.h"
#include "global.h"
#include "handler.h"
#include "labels.h"
#include "opctable.h"
#include "output.h"



/*****************************************************************************/
/*                             Helper functions                              */
/*****************************************************************************/



static void Mnemonic (const char* M)
/* Indent and output a mnemonic */
{
    Indent (MCol);
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
    Indent (ACol);
    Output ("%s", Buf);

    /* Add the code stuff as comment */
    LineComment (PC, D->Size);

    /* End the line */
    LineFeed ();
}



static const char* GetAbsOverride (unsigned Flags, unsigned Addr)
/* If the instruction requires an abs override modifier, return the necessary
** string, otherwise return the empty string.
*/
{
    if ((Flags & flAbsOverride) != 0 && Addr < 0x100) {
        return "a:";
    } else {
        return "";
    }
}



static const char* GetAddrArg (unsigned Flags, unsigned Addr)
/* Return an address argument - a label if we have one, or the address itself */
{
    const char* Label = 0;
    if (Flags & flUseLabel) {
        Label = GetLabel (Addr, PC);
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



static void GenerateLabel (unsigned Flags, unsigned Addr)
/* Generate a label in pass one if requested */
{
    /* Generate labels in pass #1, and only if we don't have a label already */
    if (Pass == 1 && !HaveLabel (Addr) &&
        /* Check if we must create a label */
        ((Flags & flGenLabel) != 0 ||
         ((Flags & flUseLabel) != 0 && Addr >= CodeStart && Addr <= CodeEnd))) {

        /* As a special case, handle ranges with tables or similar. Within
        ** such a range with a granularity > 1, do only generate dependent
        ** labels for all addresses but the first one. Be sure to generate
        ** a label for the start of the range, however.
        */
        attr_t Style         = GetStyleAttr (Addr);
        unsigned Granularity = GetGranularity (Style);

        if (Granularity == 1) {
            /* Just add the label */
            AddIntLabel (Addr);
        } else {

            /* THIS CODE IS A MESS AND WILL FAIL ON SEVERAL CONDITIONS! ### */


            /* Search for the start of the range or the last non dependent
            ** label in the range.
            */
            unsigned Offs;
            attr_t LabelAttr;
            unsigned LabelAddr = Addr;
            while (LabelAddr > CodeStart) {

                if (Style != GetStyleAttr (LabelAddr-1)) {
                    /* End of range reached */
                    break;
                }
                --LabelAddr;
                LabelAttr = GetLabelAttr (LabelAddr);
                if ((LabelAttr & (atIntLabel|atExtLabel)) != 0) {
                    /* The address has an internal or external label */
                    break;
                }
            }

            /* If the proposed label address doesn't have a label, define one */
            if ((GetLabelAttr (LabelAddr) & (atIntLabel|atExtLabel)) == 0) {
                AddIntLabel (LabelAddr);
            }

            /* Create the label */
            Offs = Addr - LabelAddr;
            if (Offs == 0) {
                AddIntLabel (Addr);
            } else {
                AddDepLabel (Addr, atIntLabel, GetLabelName (LabelAddr), Offs);
            }
        }
    }
}



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void OH_Illegal (const OpcDesc* D attribute ((unused)))
{
    DataByteLine (1);
}



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



void OH_Immediate (const OpcDesc* D)
{
    OneLine (D, "#$%02X", GetCodeByte (PC+1));
}



void OH_Direct (const OpcDesc* D)
{
    /* Get the operand */
    unsigned Addr = GetCodeByte (PC+1);

    /* Generate a label in pass 1 */
    GenerateLabel (D->Flags, Addr);

    /* Output the line */
    OneLine (D, "%s", GetAddrArg (D->Flags, Addr));
}



void OH_DirectX (const OpcDesc* D)
{
    /* Get the operand */
    unsigned Addr = GetCodeByte (PC+1);

    /* Generate a label in pass 1 */
    GenerateLabel (D->Flags, Addr);

    /* Output the line */
    OneLine (D, "%s,x", GetAddrArg (D->Flags, Addr));
}



void OH_DirectY (const OpcDesc* D)
{
    /* Get the operand */
    unsigned Addr = GetCodeByte (PC+1);

    /* Generate a label in pass 1 */
    GenerateLabel (D->Flags, Addr);

    /* Output the line */
    OneLine (D, "%s,y", GetAddrArg (D->Flags, Addr));
}



void OH_Absolute (const OpcDesc* D)
{
    /* Get the operand */
    unsigned Addr = GetCodeWord (PC+1);

    /* Generate a label in pass 1 */
    GenerateLabel (D->Flags, Addr);

    /* Output the line */
    OneLine (D, "%s%s", GetAbsOverride (D->Flags, Addr), GetAddrArg (D->Flags, Addr));
}



void OH_AbsoluteX (const OpcDesc* D)
{
    /* Get the operand */
    unsigned Addr = GetCodeWord (PC+1);

    /* Generate a label in pass 1 */
    GenerateLabel (D->Flags, Addr);

    /* Output the line */
    OneLine (D, "%s%s,x", GetAbsOverride (D->Flags, Addr), GetAddrArg (D->Flags, Addr));
}



void OH_AbsoluteY (const OpcDesc* D)
{
    /* Get the operand */
    unsigned Addr = GetCodeWord (PC+1);

    /* Generate a label in pass 1 */
    GenerateLabel (D->Flags, Addr);

    /* Output the line */
    OneLine (D, "%s%s,y", GetAbsOverride (D->Flags, Addr), GetAddrArg (D->Flags, Addr));
}



void OH_AbsoluteLong (const OpcDesc* D attribute ((unused)))
{
    Error ("Not implemented");
}



void OH_AbsoluteLongX (const OpcDesc* D attribute ((unused)))
{
    Error ("Not implemented");
}



void OH_Relative (const OpcDesc* D)
{
    /* Get the operand */
    signed char Offs = GetCodeByte (PC+1);

    /* Calculate the target address */
    unsigned Addr = (((int) PC+2) + Offs) & 0xFFFF;

    /* Generate a label in pass 1 */
    GenerateLabel (D->Flags, Addr);

    /* Output the line */
    OneLine (D, "%s", GetAddrArg (D->Flags, Addr));
}



void OH_RelativeLong (const OpcDesc* D attribute ((unused)))
{
    Error ("Not implemented");
}



void OH_DirectIndirect (const OpcDesc* D)
{
    /* Get the operand */
    unsigned Addr = GetCodeByte (PC+1);

    /* Generate a label in pass 1 */
    GenerateLabel (D->Flags, Addr);

    /* Output the line */
    OneLine (D, "(%s)", GetAddrArg (D->Flags, Addr));
}



void OH_DirectIndirectY (const OpcDesc* D)
{
    /* Get the operand */
    unsigned Addr = GetCodeByte (PC+1);

    /* Generate a label in pass 1 */
    GenerateLabel (D->Flags, Addr);

    /* Output the line */
    OneLine (D, "(%s),y", GetAddrArg (D->Flags, Addr));
}



void OH_DirectXIndirect (const OpcDesc* D)
{
    /* Get the operand */
    unsigned Addr = GetCodeByte (PC+1);

    /* Generate a label in pass 1 */
    GenerateLabel (D->Flags, Addr);

    /* Output the line */
    OneLine (D, "(%s,x)", GetAddrArg (D->Flags, Addr));
}



void OH_AbsoluteIndirect (const OpcDesc* D)
{
    /* Get the operand */
    unsigned Addr = GetCodeWord (PC+1);

    /* Generate a label in pass 1 */
    GenerateLabel (D->Flags, Addr);

    /* Output the line */
    OneLine (D, "(%s)", GetAddrArg (D->Flags, Addr));
}



void OH_BitBranch (const OpcDesc* D)
{
    /* Get the operands */
    unsigned char TestAddr   = GetCodeByte (PC+1);
    signed char   BranchOffs = GetCodeByte (PC+2);

    /* Calculate the target address for the branch */
    unsigned BranchAddr = (((int) PC+3) + BranchOffs) & 0xFFFF;

    /* Generate labels in pass 1. The bit branch codes are special in that
    ** they don't really match the remainder of the 6502 instruction set (they
    ** are a Rockwell addon), so we must pass additional flags as direct
    ** value to the second GenerateLabel call.
    */
    GenerateLabel (D->Flags, TestAddr);
    GenerateLabel (flLabel, BranchAddr);

    /* Output the line */
    OneLine (D, "%s,%s", GetAddrArg (D->Flags, TestAddr), GetAddrArg (flLabel, BranchAddr));
}



void OH_ImmediateDirect (const OpcDesc* D)
{
    /* Get the operand */
    unsigned Addr = GetCodeByte (PC+2);

    /* Generate a label in pass 1 */
    GenerateLabel (D->Flags, Addr);

    /* Output the line */
    OneLine (D, "#$%02X,%s", GetCodeByte (PC+1), GetAddrArg (D->Flags, Addr));
}



void OH_ImmediateDirectX (const OpcDesc* D)
{
    /* Get the operand */
    unsigned Addr = GetCodeByte (PC+2);

    /* Generate a label in pass 1 */
    GenerateLabel (D->Flags, Addr);

    /* Output the line */
    OneLine (D, "#$%02X,%s,x", GetCodeByte (PC+1), GetAddrArg (D->Flags, Addr));
}



void OH_ImmediateAbsolute (const OpcDesc* D)
{
    /* Get the operand */
    unsigned Addr = GetCodeWord (PC+2);

    /* Generate a label in pass 1 */
    GenerateLabel (D->Flags, Addr);

    /* Output the line */
    OneLine (D, "#$%02X,%s%s", GetCodeByte (PC+1), GetAbsOverride (D->Flags, Addr), GetAddrArg (D->Flags, Addr));
}



void OH_ImmediateAbsoluteX (const OpcDesc* D)
{
    /* Get the operand */
    unsigned Addr = GetCodeWord (PC+2);

    /* Generate a label in pass 1 */
    GenerateLabel (D->Flags, Addr);

    /* Output the line */
    OneLine (D, "#$%02X,%s%s,x", GetCodeByte (PC+1), GetAbsOverride (D->Flags, Addr), GetAddrArg (D->Flags, Addr));
}



void OH_StackRelative (const OpcDesc* D attribute ((unused)))
{
    Error ("Not implemented");
}



void OH_DirectIndirectLongX (const OpcDesc* D attribute ((unused)))
{
    Error ("Not implemented");
}



void OH_StackRelativeIndirectY (const OpcDesc* D attribute ((unused)))
{
    Error ("Not implemented");
}



void OH_DirectIndirectLong (const OpcDesc* D attribute ((unused)))
{
    Error ("Not implemented");
}



void OH_DirectIndirectLongY (const OpcDesc* D attribute ((unused)))
{
    Error ("Not implemented");
}



void OH_BlockMove (const OpcDesc* D attribute ((unused)))
{
    /* Get source operand */
    unsigned Src = GetCodeWord (PC+1);
    /* Get destination operand */
    unsigned Dst = GetCodeWord (PC+3);

    /* Generate a label in pass 1 */
    GenerateLabel (D->Flags, Src);
    GenerateLabel (D->Flags, Dst);

    /* Output the line */
    OneLine (D, "%s%s,%s%s,#$%02X",
             GetAbsOverride (D->Flags, Src), GetAddrArg (D->Flags, Src),
             GetAbsOverride (D->Flags, Dst), GetAddrArg (D->Flags, Dst),
             GetCodeWord (PC+5));
}



void OH_AbsoluteXIndirect (const OpcDesc* D attribute ((unused)))
{
    /* Get the operand */
    unsigned Addr = GetCodeWord (PC+1);

    /* Generate a label in pass 1 */
    GenerateLabel (D->Flags, Addr);

    /* Output the line */
    OneLine (D, "(%s,x)", GetAddrArg (D->Flags, Addr));
}



void OH_DirectImmediate (const OpcDesc* D)
{
    /* Get the operand */
    unsigned Addr = GetCodeByte (PC+1);

    /* Generate a label in pass 1 */
    GenerateLabel (D->Flags, Addr);

    /* Output the line */
    OneLine (D, "%s, #$%02X", GetAddrArg (D->Flags, Addr), GetCodeByte (PC+2));
}



void OH_ZeroPageBit (const OpcDesc* D)
{
    unsigned Bit = GetCodeByte (PC) >> 5;
    unsigned Addr = GetCodeByte (PC+1);

    /* Generate a label in pass 1 */
    GenerateLabel (D->Flags, Addr);

    /* Output the line */
    OneLine (D, "%01X,%s", Bit, GetAddrArg (D->Flags, Addr));
}



void OH_AccumulatorBit (const OpcDesc* D)
{
    unsigned Bit = GetCodeByte (PC) >> 5;

    /* Output the line */
    OneLine (D, "%01X,a", Bit);
}



void OH_AccumulatorBitBranch (const OpcDesc* D)
{
    unsigned Bit = GetCodeByte (PC) >> 5;
    signed char BranchOffs = GetCodeByte (PC+1);

    /* Calculate the target address for the branch */
    unsigned BranchAddr = (((int) PC+3) + BranchOffs) & 0xFFFF;

    /* Generate labels in pass 1 */
    GenerateLabel (flLabel, BranchAddr);

    /* Output the line */
    OneLine (D, "%01X,a,%s", Bit, GetAddrArg (flLabel, BranchAddr));
}



void OH_JmpDirectIndirect (const OpcDesc* D)
{
    OH_DirectIndirect (D);
    if (NewlineAfterJMP) {
        LineFeed ();
    }
    SeparatorLine ();
}



void OH_SpecialPage (const OpcDesc* D)
{
  /* Get the operand */
  unsigned Addr = 0xFF00 + GetCodeByte (PC+1);

  /* Generate a label in pass 1 */
  GenerateLabel (D->Flags, Addr);

  /* OneLine (D, "$FF%02X", (CodeByte (PC+1)); */
  OneLine (D, "%s", GetAddrArg (D->Flags, Addr));
}



void OH_Rts (const OpcDesc* D)
{
    OH_Implicit (D);
    if (NewlineAfterRTS) {
        LineFeed ();
    }
    SeparatorLine();
}



void OH_JmpAbsolute (const OpcDesc* D)
{
    OH_Absolute (D);
    if (NewlineAfterJMP) {
        LineFeed ();
    }
    SeparatorLine ();
}



void OH_JmpAbsoluteIndirect (const OpcDesc* D)
{
    OH_AbsoluteIndirect (D);
    if (NewlineAfterJMP) {
        LineFeed ();
    }
    SeparatorLine ();
}
