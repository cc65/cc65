/*****************************************************************************/
/*                                                                           */
/*				   attrtab.c				     */
/*                                                                           */
/*			 Disassembler attribute table			     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2005 Ullrich von Bassewitz                                       */
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



/* Label + comment */
typedef struct {
    const char*	Name;
    const char* Comment;
} Label;

/* A whole lot of Labels, so we don't have to allocate them separately */
static Label* LabelHeap = 0;
static unsigned LabelsLeft = 0;

/* Attribute table */
static unsigned char AttrTab [0x10000];

/* Symbol table */
static const Label* SymTab [0x10000];



/*****************************************************************************/
/*                               struct Label                                */
/*****************************************************************************/



Label* NewLabel (const char* Name, const char* Comment)
/* Create a new Label structure, initialize and return it */
{
    Label* L;

    /* Check if we have some free labels left */
    if (LabelsLeft == 0) {
        /* Allocate a new block of memory */
        LabelsLeft = 200;
        LabelHeap = xmalloc (LabelsLeft * sizeof (Label));
    }

    /* Get a new one from the buffer */
    L = LabelHeap++;
    --LabelsLeft;

    /* Initialize the new label */
    L->Name = xstrdup (Name);
    L->Comment = Comment? xstrdup (Comment) : 0;

    /* Return the new label */
    return L;
}



/*****************************************************************************/
/*				     Code				     */
/*****************************************************************************/



static void AddrCheck (unsigned Addr)
/* Check if the given address has a valid range */
{
    if (Addr >= 0x10000) {
	Error ("Address out of range: %08X", Addr);
    }
}



unsigned GetGranularity (attr_t Style)
/* Get the granularity for the given style */
{
    switch (Style) {
	case atDefault:	 return 1;
	case atCode:	 return 1;
	case atIllegal:	 return 1;
	case atByteTab:	 return 1;
	case atDByteTab: return 2;
	case atWordTab:	 return 2;
	case atDWordTab: return 4;
	case atAddrTab:  return 2;
	case atRtsTab:   return 2;
	case atTextTab:  return 1;

	case atSkip:
	default:
	    Internal ("GetGraularity called for style = %d", Style);
	    return 0;
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



static const char* MakeLabelName (unsigned Addr)
/* Make the default label name from the given address and return it in a
 * static buffer.
 */
{
    static char LabelBuf [32];
    xsprintf (LabelBuf, sizeof (LabelBuf), "L%04X", Addr);
    return LabelBuf;
}



void AddLabel (unsigned Addr, attr_t Attr, const char* Name, const char* Comment)
/* Add a label */
{
    /* Get an existing label attribute */
    attr_t ExistingAttr = GetLabelAttr (Addr);

    /* Must not have two symbols for one address */
    if (ExistingAttr != atNoLabel) {
    	/* Allow redefinition if identical */
       	if (ExistingAttr == Attr && strcmp (SymTab[Addr]->Name, Name) == 0) {
    	    return;
    	}
    	Error ("Duplicate label for address $%04X: %s/%s", Addr, SymTab[Addr]->Name, Name);
    }

    /* Create a new label */
    SymTab[Addr] = NewLabel (Name, Comment);

    /* Remember the attribute */
    AttrTab[Addr] |= Attr;
}



void AddIntLabel (unsigned Addr)
/* Add an internal label using the address to generate the name. */
{
    AddLabel (Addr, atIntLabel, MakeLabelName (Addr), 0);
}



void AddExtLabel (unsigned Addr, const char* Name, const char* Comment)
/* Add an external label */
{
    AddLabel (Addr, atExtLabel, Name, Comment);
}



void AddDepLabel (unsigned Addr, attr_t Attr, const char* BaseName, unsigned Offs)
/* Add a dependent label at the given address using "base name+Offs" as the new
 * name.
 */
{
    /* Allocate memory for the dependent label name */
    unsigned NameLen = strlen (BaseName);
    char*    DepName = xmalloc (NameLen + 7);	/* "+$ABCD" */

    /* Create the new name in the buffer */
    if (UseHexOffs) {
	sprintf (DepName, "%s+$%02X", BaseName, Offs);
    } else {
	sprintf (DepName, "%s+%u", BaseName, Offs);
    }

    /* Define the labels */
    AddLabel (Addr, Attr | atDepLabel, DepName, 0);

    /* Free the name buffer */
    xfree (DepName);
}



static void AddLabelRange (unsigned Addr, attr_t Attr,
                           const char* Name, const char* Comment,
                           unsigned Count)
/* Add a label for a range. The first entry gets the label "Name" while the
 * others get "Name+offs".
 */
{
    /* Define the label */
    AddLabel (Addr, Attr, Name, Comment);

    /* Define dependent labels if necessary */
    if (Count > 1) {
    	unsigned Offs;

        /* Setup the format string */
        const char* Format = UseHexOffs? "$%02X" : "%u";

    	/* Allocate memory for the dependent label names */
    	unsigned NameLen = strlen (Name);
    	char* 	 DepName = xmalloc (NameLen + 7);	/* "+$ABCD" */
    	char* 	 DepOffs = DepName + NameLen + 1;

    	/* Copy the original name into the buffer */
    	memcpy (DepName, Name, NameLen);
    	DepName[NameLen] = '+';

    	/* Define the labels */
       	for (Offs = 1; Offs < Count; ++Offs) {
    	    sprintf (DepOffs, Format, Offs);
    	    AddLabel (Addr + Offs, Attr | atDepLabel, DepName, 0);
    	}

    	/* Free the name buffer */
    	xfree (DepName);
    }
}



void AddIntLabelRange (unsigned Addr, const char* Name, unsigned Count)
/* Add an internal label for a range. The first entry gets the label "Name"
 * while the others get "Name+offs".
 */
{
    /* Define the label range */
    AddLabelRange (Addr, atIntLabel, Name, 0, Count);
}



void AddExtLabelRange (unsigned Addr, const char* Name, const char* Comment, unsigned Count)
/* Add an external label for a range. The first entry gets the label "Name"
 * while the others get "Name+offs".
 */
{
    /* Define the label range */
    AddLabelRange (Addr, atExtLabel, Name, Comment, Count);
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
    return SymTab[Addr]? SymTab[Addr]->Name : 0;
}



const char* GetComment (unsigned Addr)
/* Return the comment for an address */
{
    /* Check the given address */
    AddrCheck (Addr);

    /* Return the label if any */
    return SymTab[Addr]? SymTab[Addr]->Comment : 0;
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



void DefOutOfRangeLabels (void)
/* Output any labels that are out of the loaded code range */
{
    unsigned long Addr;

    SeparatorLine ();

    /* Low range */
    Addr = 0;
    while (Addr < CodeStart) {
	if (MustDefLabel (Addr)) {
	    DefineConst (SymTab[Addr]->Name, SymTab[Addr]->Comment, Addr);
    	}
        ++Addr;
    }

    /* Skip areas in code range */
    while (Addr <= CodeEnd) {
        if ((AttrTab[Addr] & atStyleMask) == atSkip && MustDefLabel (Addr)) {
	    DefineConst (SymTab[Addr]->Name, SymTab[Addr]->Comment, Addr);
        }
        ++Addr;
    }

    /* High range */
    while (Addr < 0x10000) {
	if (MustDefLabel (Addr)) {
	    DefineConst (SymTab[Addr]->Name, SymTab[Addr]->Comment, Addr);
	}
        ++Addr;
    }

    SeparatorLine ();
}



