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
#include "error.h"
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



void AddLabel (unsigned Addr, const char* Name)
/* Add a label */
{
    /* Check the given address */
    AddrCheck (Addr);

    /* Must not have two symbols for one address */
    if (SymTab[Addr] != 0) {
	if (strcmp (SymTab[Addr], Name) == 0) {
	    /* Allow label if it has the same name */
	    return;
	}
	Error ("Duplicate label for address %04X: %s/%s", Addr, SymTab[Addr], Name);
    }

    /* Create a new label */
    SymTab[Addr] = xstrdup (Name);	   
}



int HaveLabel (unsigned Addr)
/* Check if there is a label for the given address */
{
    /* Check the given address */
    AddrCheck (Addr);

    /* Check for a label */
    return (SymTab[Addr] != 0);
}



const char* GetLabel (unsigned Addr)
/* Return the label for an address */
{
    /* Check the given address */
    AddrCheck (Addr);

    /* Return the label if any */
    return SymTab[Addr];
}



unsigned char GetStyle (unsigned Addr)
/* Return the style attribute for the given address */
{
    /* Check the given address */
    AddrCheck (Addr);

    /* Return the attribute */
    return (AttrTab[Addr] & atStyleMask);
}



