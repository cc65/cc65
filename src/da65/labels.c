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



#include <inttypes.h>
#include <stdio.h>
#include <string.h>

/* common */
#include "coll.h"
#include "strbuf.h"
#include "xmalloc.h"
#include "xsprintf.h"

/* da65 */
#include "code.h"
#include "comments.h"
#include "error.h"
#include "global.h"
#include "labels.h"
#include "output.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Label structure how it is found in the label table */
typedef struct Label Label;
struct Label {
    struct Label*       Next;           /* Next entry in linked list */
    uint32_t            Addr;           /* The full address */
    char                Name[1];        /* Symbol name, dynamically allocated */
};

/* Labels use a hash table and a linear list for collision resolution. The
** hash function is easy and effective. It evaluates just the lower bits of
** the address.
*/
#define LABEL_HASH_SIZE         4096u   /* Must be power of two */
static Label* LabelTab[LABEL_HASH_SIZE];



/*****************************************************************************/
/*                               struct Label                                */
/*****************************************************************************/



static Label* NewLabel (uint32_t Addr, const char* Name)
/* Create a new label structure and return it */
{
    /* Get the length of the name */
    unsigned Len = strlen (Name);

    /* Create a new label */
    Label* L = xmalloc (sizeof (Label) + Len);

    /* Fill in the data */
    L->Next = 0;
    L->Addr = Addr;
    memcpy (L->Name, Name, Len + 1);

    /* Return the label just created */
    return L;
}



static uint32_t GetLabelHash (uint32_t Addr)
/* Get the hash for a label at the given address */
{
    return (Addr & (LABEL_HASH_SIZE - 1));
}



static Label* FindLabel (uint32_t Addr)
/* Search for a label for the given address and return it. Returns NULL if
** no label exists for the address.
*/
{
    Label* L = LabelTab[GetLabelHash (Addr)];
    while (L) {
        if (L->Addr == Addr) {
            break;
        }
        L = L->Next;
    }
    return L;
}



static void InsertLabel (Label* L)
/* Insert a label into the tables */
{
    /* Insert into hash table */
    uint32_t Hash = GetLabelHash (L->Addr);
    L->Next = LabelTab[Hash];
    LabelTab[Hash] = L;
}



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static const char* MakeLabelName (uint32_t Addr)
/* Make the default label name from the given address and return it in a
** static buffer.
*/
{
    static char LabelBuf [32];
    xsprintf (LabelBuf, sizeof (LabelBuf), "L%04" PRIX32, Addr);
    return LabelBuf;
}



static void AddLabel (uint32_t Addr, attr_t Attr, const char* Name)
/* Add a label */
{
    /* Get an existing label attribute */
    attr_t ExistingAttr = GetLabelAttr (Addr);

    /* Must not have two symbols for one address */
    if (ExistingAttr != atNoLabel) {
        /* Allow redefinition if identical. Beware: Unnamed labels do not
        ** have an entry in the label table.
        */
        Label* L = FindLabel (Addr);
        if (ExistingAttr == Attr &&
            ((Name == 0 && L == 0) ||
             (Name != 0 && L != 0 && strcmp (Name, L->Name) == 0))) {
            return;
        }
        Error ("Duplicate label for address $%04X (%s): '%s'", Addr,
               L? L->Name : "<unnamed label>",
               Name? Name : "<unnamed label>");
    }

    /* If this is not an unnamed label, create a new label entry and
    ** insert it.
    */
    if (Name != 0) {
        InsertLabel (NewLabel (Addr, Name));
    }

    /* Remember the attribute */
    MarkAddr (Addr, Attr);
}



void AddIntLabel (uint32_t Addr)
/* Add an internal label using the address to generate the name. */
{
    AddLabel (Addr, atIntLabel, MakeLabelName (Addr));
}



void AddExtLabel (uint32_t Addr, const char* Name)
/* Add an external label */
{
    AddLabel (Addr, atExtLabel, Name);
}



void AddUnnamedLabel (uint32_t Addr)
/* Add an unnamed label */
{
    AddLabel (Addr, atUnnamedLabel, 0);
}



void AddDepLabel (uint32_t Addr, attr_t Attr, const char* BaseName, unsigned Offs)
/* Add a dependent label at the given address using "basename+Offs" as the new
** name.
*/
{
    /* Create the new name in the buffer */
    StrBuf Name = AUTO_STRBUF_INITIALIZER;
    if (UseHexOffs) {
        SB_Printf (&Name, "%s+$%02X", BaseName, Offs);
    } else {
        SB_Printf (&Name, "%s+%u", BaseName, Offs);
    }

    /* Define the labels */
    AddLabel (Addr, Attr | atDepLabel, SB_GetConstBuf (&Name));

    /* Free the name buffer */
    SB_Done (&Name);



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



static void AddLabelRange (uint32_t Addr, attr_t Attr,
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



void AddIntLabelRange (uint32_t Addr, const char* Name, unsigned Count)
/* Add an internal label for a range. The first entry gets the label "Name"
** while the others get "Name+offs".
*/
{
    /* Define the label range */
    AddLabelRange (Addr, atIntLabel, Name, Count);
}



void AddExtLabelRange (uint32_t Addr, const char* Name, unsigned Count)
/* Add an external label for a range. The first entry gets the label "Name"
** while the others get "Name+offs".
*/
{
    /* Define the label range */
    AddLabelRange (Addr, atExtLabel, Name, Count);
}



int HaveLabel (uint32_t Addr)
/* Check if there is a label for the given address */
{
    /* Check for a label */
    return (GetLabelAttr (Addr) != atNoLabel);
}



int MustDefLabel (uint32_t Addr)
/* Return true if we must define a label for this address, that is, if there
** is a label at this address, and it is an external or internal label.
*/
{
    /* Get the label attribute */
    attr_t A = GetLabelAttr (Addr);

    /* Check for an internal, external, or unnamed label */
    return (A == atExtLabel || A == atIntLabel || A == atUnnamedLabel);
}



const char* GetLabelName (uint32_t Addr)
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
        const Label* L = FindLabel (Addr);
        return L? L->Name : 0;
    }
}



const char* GetLabel (uint32_t Addr, uint32_t RefFrom)
/* Return the label name for an address, as it is used in a label reference.
** RefFrom is the address the label is referenced from. This is needed in case
** of unnamed labels, to determine the name.
*/
{
    static const char* const FwdLabels[] = {
        ":+", ":++", ":+++", ":++++", ":+++++", ":++++++", ":+++++++",
        ":++++++++", ":+++++++++", ":++++++++++"
    };
    static const char* const BackLabels[] = {
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
            uint32_t I = RefFrom;
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
            uint32_t I = RefFrom;
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
        const Label* L = FindLabel (Addr);
        return L? L->Name : 0;
    }
}



void ForwardLabel (uint32_t Offs)
/* If necessary, output a forward label, one that is within the next few
** bytes and is therefore output as "label = * + x".
*/
{
    /* Calculate the actual address */
    uint32_t Addr = PC + Offs;

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
        Error ("Cannot define unnamed label at address $%04" PRIX32, Addr);
    }

    /* Output the label */
    DefForward (GetLabelName (Addr), GetComment (Addr), Offs);
}



static int CompareLabels (void* Data attribute ((unused)),
                          const void* L1, const void* L2)
/* Compare functions for sorting the out-of-range labels */
{
    if (((const Label*) L1)->Addr < ((const Label*) L2)->Addr) {
        return -1;
    } else if (((const Label*) L1)->Addr > ((const Label*) L2)->Addr) {
        return 1;
    } else {
        return 0;
    }
}



static void DefOutOfRangeLabel (const Label* L)
/* Define one label that is outside code range. */
{
    switch (GetLabelAttr (L->Addr)) {

        case atIntLabel:
        case atExtLabel:
            DefConst (L->Name, GetComment (L->Addr), L->Addr);
            break;

        case atUnnamedLabel:
            Error ("Cannot define unnamed label at address $%04" PRIX32, L->Addr);
            break;

        default:
            break;

    }
}



void DefOutOfRangeLabels (void)
/* Output any labels that are out of the loaded code range */
{
    unsigned I;

    /* This requires somewhat more effort since the labels output should be
    ** sorted by address for better readability. This is not directly
    ** possible when using a hash table, so an intermediate data structure
    ** is required. It is not possible to collect out-of-range labels while
    ** generating them, since they may come from an info file and are added
    ** while no input file was read. Which means it cannot be determined at
    ** that point if they're out-of-range or not.
    */
    Collection Labels = AUTO_COLLECTION_INITIALIZER;
    CollGrow (&Labels, 128);

    /* Walk over the hash and copy all out-of-range labels */
    for (I = 0; I < LABEL_HASH_SIZE; ++I) {
        Label* L = LabelTab[I];
        while (L) {
            if (L->Addr < CodeStart || L->Addr > CodeEnd) {
                CollAppend (&Labels, L);
            }
            L = L->Next;
        }
    }

    /* Sort the out-of-range labels by address */
    CollSort (&Labels, CompareLabels, 0);

    /* Output the labels */
    SeparatorLine ();
    for (I = 0; I < CollCount (&Labels); ++I) {
        DefOutOfRangeLabel (CollConstAt (&Labels, I));
    }
    SeparatorLine ();

    /* Free allocated storage */
    DoneCollection (&Labels);
}
