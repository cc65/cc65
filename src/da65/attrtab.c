/*****************************************************************************/
/*                                                                           */
/*				   attrtab.c				     */
/*                                                                           */
/*			 Disassembler attribute table			     */
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



#include <stdio.h>
#include <string.h>

/* common */
#include "xmalloc.h"
#include "xsprintf.h"

/* da65 */
#include "code.h"
#include "error.h"
#include "global.h"
#include "output.h"
#include "attrtab.h"



/*****************************************************************************/
/*				     Data				     */
/*****************************************************************************/



/* Attribute table */
static unsigned char AttrTab [0x10000];

/* Symbol table */
static const char* SymTab [0x10000];



/*****************************************************************************/
/*				     Code				     */
/*****************************************************************************/



void AddrCheck (unsigned Addr)
/* Check if the given address has a valid range */
{
    if (Addr >= 0x10000) {
	Error ("Address out of range: %08X", Addr);
    }
}



void MarkRange (unsigned Start, unsigned End, attr_t Attr)
/* Mark a range with the given attribute */
{
    /* Do it easy here... */
    while (Start <= End) {
	MarkAddr (Start++, Attr);
    }
}



void MarkAddr (unsigned Addr, attr_t Attr)
/* Mark an address with an attribute */
{
    /* Check the given address */
    AddrCheck (Addr);

    /* We must not have more than one style bit */
    if (Attr & atStyleMask) {
	if (AttrTab[Addr] & atStyleMask) {
	    Error ("Duplicate style for address %04X", Addr);
	}
    }

    /* Set the style */
    AttrTab[Addr] |= Attr;
}



const char* MakeLabelName (unsigned Addr)
/* Make the default label name from the given address and return it in a
 * static buffer.
 */
{
    static char LabelBuf [32];
    xsprintf (LabelBuf, sizeof (LabelBuf), "L%04X", Addr);
    return LabelBuf;
}



void AddLabel (unsigned Addr, attr_t Attr, const char* Name)
/* Add a label */
{
    /* Get an existing label attribute */
    attr_t ExistingAttr = GetLabelAttr (Addr);

    /* Must not have two symbols for one address */
    if (ExistingAttr != atNoLabel) {
	/* Allow redefinition if identical */
       	if (ExistingAttr == Attr && strcmp (SymTab[Addr], Name) == 0) {
	    return;
	}
	Error ("Duplicate label for address %04X: %s/%s", Addr, SymTab[Addr], Name);
    }

    /* Create a new label */
    SymTab[Addr] = xstrdup (Name);

    /* Remember the attribute */
    AttrTab[Addr] |= Attr;
}



int HaveLabel (unsigned Addr)
/* Check if there is a label for the given address */
{
    /* Check the given address */
    AddrCheck (Addr);

    /* Check for a label */
    return (SymTab[Addr] != 0);
}



int MustDefLabel (unsigned Addr)
/* Return true if we must define a label for this address, that is, if there
 * is a label at this address, and it is an external or internal label.
 */
{
    /* Get the label attribute */
    attr_t A = GetLabelAttr (Addr);

    /* Check for an internal or external label */
    return (A == atExtLabel || A == atIntLabel);
}



const char* GetLabel (unsigned Addr)
/* Return the label for an address */
{
    /* Check the given address */
    AddrCheck (Addr);

    /* Return the label if any */
    return SymTab[Addr];
}



unsigned char GetStyleAttr (unsigned Addr)
/* Return the style attribute for the given address */
{
    /* Check the given address */
    AddrCheck (Addr);

    /* Return the attribute */
    return (AttrTab[Addr] & atStyleMask);
}



unsigned char GetLabelAttr (unsigned Addr)
/* Return the label attribute for the given address */
{
    /* Check the given address */
    AddrCheck (Addr);

    /* Return the attribute */
    return (AttrTab[Addr] & atLabelMask);
}



static void DefineConst (unsigned Addr)
/* Define an address constant */
{
    Output ("%s", SymTab [Addr]);
    Indent (AIndent);
    Output ("= $%04X", Addr);
    LineFeed ();
}



void DefOutOfRangeLabels (void)
/* Output any labels that are out of the loaded code range */
{
    unsigned long Addr;

    SeparatorLine ();

    /* Low range */
    for (Addr = 0; Addr < CodeStart; ++Addr) {
	if (MustDefLabel (Addr)) {
	    DefineConst (Addr);
	}
    }

    /* High range */
    for (Addr = CodeEnd+1; Addr < 0x10000; ++Addr) {
	if (MustDefLabel (Addr)) {
	    DefineConst (Addr);
	}
    }

    SeparatorLine ();
}



