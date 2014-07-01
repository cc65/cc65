/*****************************************************************************/
/*                                                                           */
/*                                 labels.c                                  */
/*                                                                           */
/*                         Label management for da65                         */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2006-2007 Ullrich von Bassewitz                                       */
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



#include <stdio.h>
#include <string.h>

/* common */
#include "xmalloc.h"
#include "xsprintf.h"

/* da65 */
#include "attrtab.h"
#include "code.h"
#include "comments.h"
#include "error.h"
#include "global.h"
#include "labels.h"
#include "output.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Symbol table */
static const char* SymTab[0x10000];



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static const char* MakeLabelName (unsigned Addr)
/* Make the default label name from the given address and return it in a
** static buffer.
*/
{
    static char LabelBuf [32];
    xsprintf (LabelBuf, sizeof (LabelBuf), "L%04X", Addr);
    return LabelBuf;
}



static void AddLabel (unsigned Addr, attr_t Attr, const char* Name)
/* Add a label */
{
    /* Get an existing label attribute */
    attr_t ExistingAttr = GetLabelAttr (Addr);

    /* Must not have two symbols for one address */
    if (ExistingAttr != atNoLabel) {
        /* Allow redefinition if identical. Beware: Unnamed labels don't
        ** have a name (you guessed that, didn't you?).
        */
        if (ExistingAttr == Attr &&
            ((Name == 0 && SymTab[Addr] == 0) || strcmp (SymTab[Addr], Name) == 0)) {
            return;
        }
        Error ("Duplicate label for address $%04X: %s/%s", Addr, SymTab[Addr], Name);
    }

    /* Create a new label (xstrdup will return NULL if input NULL) */
    SymTab[Addr] = xstrdup (Name);

    /* Remember the attribute */
    MarkAddr (Addr, Attr);
}



void AddIntLabel (unsigned Addr)
/* Add an internal label using the address to generate the name. */
{
    AddLabel (Addr, atIntLabel, MakeLabelName (Addr));
}



void AddExtLabel (unsigned Addr, const char* Name)
/* Add an external label */
{
    AddLabel (Addr, atExtLabel, Name);
}



void AddUnnamedLabel (unsigned Addr)
/* Add an unnamed label */
{
    AddLabel (Addr, atUnnamedLabel, 0);
}



void AddDepLabel (unsigned Addr, attr_t Attr, const char* BaseName, unsigned Offs)
/* Add a dependent label at the given address using "basename+Offs" as the new
** name.
*/
{
    /* Allocate memory for the dependent label name */
    unsigned NameLen = strlen (BaseName);
    char*    DepName = xmalloc (NameLen + 7);   /* "+$ABCD\0" */

    /* Create the new name in the buffer */
    if (UseHexOffs) {
        sprintf (DepName, "%s+$%02X", BaseName, Offs);
    } else {
        sprintf (DepName, "%s+%u", BaseName, Offs);
    }

    /* Define the labels */
    AddLabel (Addr, Attr | atDepLabel, DepName);

    /* Free the name buffer */
    xfree (DepName);
}



static void AddLabelRange (unsigned Addr, attr_t Attr,
                           const char* Name, unsigned Count)
/* Add a label for a range. The first entry gets the label "Name" while the
** others get "Name+offs".
*/
{
    /* Define the label */
    AddLabel (Addr, Attr, Name);

    /* Define dependent labels if necessary */
    if (Count > 1) {
        unsigned Offs;

        /* Setup the format string */
        const char* Format = UseHexOffs? "$%02X" : "%u";

        /* Allocate memory for the dependent label names */
        unsigned NameLen = strlen (Name);
        char*    DepName = xmalloc (NameLen + 7);       /* "+$ABCD" */
        char*    DepOffs = DepName + NameLen + 1;

        /* Copy the original name into the buffer */
        memcpy (DepName, Name, NameLen);
        DepName[NameLen] = '+';

        /* Define the labels */
        for (Offs = 1; Offs < Count; ++Offs) {
            sprintf (DepOffs, Format, Offs);
            AddLabel (Addr + Offs, Attr | atDepLabel, DepName);
        }

        /* Free the name buffer */
        xfree (DepName);
    }
}



void AddIntLabelRange (unsigned Addr, const char* Name, unsigned Count)
/* Add an internal label for a range. The first entry gets the label "Name"
** while the others get "Name+offs".
*/
{
    /* Define the label range */
    AddLabelRange (Addr, atIntLabel, Name, Count);
}



void AddExtLabelRange (unsigned Addr, const char* Name, unsigned Count)
/* Add an external label for a range. The first entry gets the label "Name"
** while the others get "Name+offs".
*/
{
    /* Define the label range */
    AddLabelRange (Addr, atExtLabel, Name, Count);
}



int HaveLabel (unsigned Addr)
/* Check if there is a label for the given address */
{
    /* Check for a label */
    return (GetLabelAttr (Addr) != atNoLabel);
}



int MustDefLabel (unsigned Addr)
/* Return true if we must define a label for this address, that is, if there
** is a label at this address, and it is an external or internal label.
*/
{
    /* Get the label attribute */
    attr_t A = GetLabelAttr (Addr);

    /* Check for an internal, external, or unnamed label */
    return (A == atExtLabel || A == atIntLabel || A == atUnnamedLabel);
}



const char* GetLabelName (unsigned Addr)
/* Return the label name for an address */
{
    /* Get the label attribute */
    attr_t A = GetLabelAttr (Addr);

    /* Special case unnamed labels, because these don't have a named stored in
    ** the symbol table to save space.
    */
    if (A == atUnnamedLabel) {
        return "";
    } else {
        /* Return the label if any */
        return SymTab[Addr];
    }
}



const char* GetLabel (unsigned Addr, unsigned RefFrom)
/* Return the label name for an address, as it is used in a label reference.
** RefFrom is the address the label is referenced from. This is needed in case
** of unnamed labels, to determine the name.
*/
{
    static const char* FwdLabels[] = {
        ":+", ":++", ":+++", ":++++", ":+++++", ":++++++", ":+++++++",
        ":++++++++", ":+++++++++", ":++++++++++"
    };
    static const char* BackLabels[] = {
        ":-", ":--", ":---", ":----", ":-----", ":------", ":-------",
        ":--------", ":---------", ":----------"
    };

    /* Get the label attribute */
    attr_t A = GetLabelAttr (Addr);

    /* Special case unnamed labels, because these don't have a named stored in
    ** the symbol table to save space.
    */
    if (A == atUnnamedLabel) {

        unsigned Count = 0;

        /* Search forward or backward depending in which direction the label
        ** is.
        */
        if (Addr <= RefFrom) {
            /* Search backwards */
            unsigned I = RefFrom;
            while (Addr < I) {
                --I;
                A = GetLabelAttr (I);
                if (A == atUnnamedLabel) {
                    ++Count;
                    if (Count >= sizeof (BackLabels) / sizeof (BackLabels[0])) {
                        Error ("Too many unnamed labels between label at "
                               "$%04X and reference at $%04X", Addr, RefFrom);
                    }
                }
            }

            /* Return the label name */
            return BackLabels[Count-1];

        } else {
            /* Search forwards */
            unsigned I = RefFrom;
            while (Addr > I) {
                ++I;
                A = GetLabelAttr (I);
                if (A == atUnnamedLabel) {
                    ++Count;
                    if (Count >= sizeof (FwdLabels) / sizeof (FwdLabels[0])) {
                        Error ("Too many unnamed labels between label at "
                               "$%04X and reference at $%04X", Addr, RefFrom);
                    }
                }
            }

            /* Return the label name */
            return FwdLabels[Count-1];
        }

    } else {
        /* Return the label if any */
        return SymTab[Addr];
    }
}



void ForwardLabel (unsigned Offs)
/* If necessary, output a forward label, one that is within the next few
** bytes and is therefore output as "label = * + x".
*/
{
    /* Calculate the actual address */
    unsigned long Addr = PC + Offs;

    /* Get the type of the label */
    attr_t A = GetLabelAttr (Addr);

    /* If there is no label, or just a dependent one, bail out */
    if (A == atNoLabel || (A & atDepLabel) != 0) {
        return;
    }

    /* An unnamed label cannot be output as a forward declaration, so this is
    ** an error.
    */
    if (A == atUnnamedLabel) {
        Error ("Cannot define unnamed label at address $%04lX", Addr);
    }

    /* Output the label */
    DefForward (GetLabelName (Addr), GetComment (Addr), Offs);
}



static void DefOutOfRangeLabel (unsigned long Addr)
/* Define one label that is outside code range. */
{
    switch (GetLabelAttr (Addr)) {

        case atIntLabel:
        case atExtLabel:
            DefConst (SymTab[Addr], GetComment (Addr), Addr);
            break;

        case atUnnamedLabel:
            Error ("Cannot define unnamed label at address $%04lX", Addr);
            break;

        default:
            break;

    }
}



void DefOutOfRangeLabels (void)
/* Output any labels that are out of the loaded code range */
{
    unsigned long Addr;

    SeparatorLine ();

    /* Low range */
    Addr = 0;
    while (Addr < CodeStart) {
        DefOutOfRangeLabel (Addr++);
    }

    /* Skip areas in code range */
    while (Addr <= CodeEnd) {
        if (GetStyleAttr (Addr) == atSkip) {
            DefOutOfRangeLabel (Addr);
        }
        ++Addr;
    }

    /* High range */
    while (Addr < 0x10000) {
        DefOutOfRangeLabel (Addr++);
    }

    SeparatorLine ();
}
