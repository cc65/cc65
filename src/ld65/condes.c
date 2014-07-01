/*****************************************************************************/
/*                                                                           */
/*                                 condes.c                                  */
/*                                                                           */
/*                   Module constructor/destructor support                   */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2012, Ullrich von Bassewitz                                      */
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



#include <string.h>

/* common */
#include "addrsize.h"
#include "check.h"
#include "coll.h"
#include "filepos.h"
#include "fragdefs.h"
#include "xmalloc.h"

/* ld65 */
#include "condes.h"
#include "exports.h"
#include "fragment.h"
#include "segments.h"
#include "spool.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Struct describing one condes type */
typedef struct ConDesDesc ConDesDesc;
struct ConDesDesc {
    Collection          ExpList;        /* List of exported symbols */
    unsigned            SegName;        /* Name of segment the table is in */
    unsigned            Label;          /* Name of table label */
    unsigned            CountSym;       /* Name of symbol for entry count */
    unsigned char       Order;          /* Table order (increasing/decreasing) */
    ConDesImport        Import;         /* Forced import if any */
};

/* Array for all types */
static ConDesDesc ConDes[CD_TYPE_COUNT] = {
    {
        STATIC_COLLECTION_INITIALIZER,
        INVALID_STRING_ID,
        INVALID_STRING_ID,
        INVALID_STRING_ID,
        cdIncreasing,
        { INVALID_STRING_ID, STATIC_FILEPOS_INITIALIZER, ADDR_SIZE_DEFAULT },
    },{
        STATIC_COLLECTION_INITIALIZER,
        INVALID_STRING_ID,
        INVALID_STRING_ID,
        INVALID_STRING_ID,
        cdIncreasing,
        { INVALID_STRING_ID, STATIC_FILEPOS_INITIALIZER, ADDR_SIZE_DEFAULT },
    },{
        STATIC_COLLECTION_INITIALIZER,
        INVALID_STRING_ID,
        INVALID_STRING_ID,
        INVALID_STRING_ID,
        cdIncreasing,
        { INVALID_STRING_ID, STATIC_FILEPOS_INITIALIZER, ADDR_SIZE_DEFAULT },
    },{
        STATIC_COLLECTION_INITIALIZER,
        INVALID_STRING_ID,
        INVALID_STRING_ID,
        INVALID_STRING_ID,
        cdIncreasing,
        { INVALID_STRING_ID, STATIC_FILEPOS_INITIALIZER, ADDR_SIZE_DEFAULT },
    },{
        STATIC_COLLECTION_INITIALIZER,
        INVALID_STRING_ID,
        INVALID_STRING_ID,
        INVALID_STRING_ID,
        cdIncreasing,
        { INVALID_STRING_ID, STATIC_FILEPOS_INITIALIZER, ADDR_SIZE_DEFAULT },
    },{
        STATIC_COLLECTION_INITIALIZER,
        INVALID_STRING_ID,
        INVALID_STRING_ID,
        INVALID_STRING_ID,
        cdIncreasing,
        { INVALID_STRING_ID, STATIC_FILEPOS_INITIALIZER, ADDR_SIZE_DEFAULT },
    },{
        STATIC_COLLECTION_INITIALIZER,
        INVALID_STRING_ID,
        INVALID_STRING_ID,
        INVALID_STRING_ID,
        cdIncreasing,
        { INVALID_STRING_ID, STATIC_FILEPOS_INITIALIZER, ADDR_SIZE_DEFAULT },
    },
};



/*****************************************************************************/
/*           Internally used function to create the condes tables            */
/*****************************************************************************/



static int ConDesCompare (void* Data, const void* E1, const void* E2)
/* Compare function to sort the exports */
{
    int Cmp;

    /* Data is actually a pointer to a ConDesDesc from the table, E1 and
    ** E2 are exports from the collection. Get the condes type and cast
    ** the void pointers to object pointers.
    */
    ConDesDesc* CD = ((ConDesDesc*) Data);
    int Type = CD - ConDes;
    const Export* Exp1 = (const Export*) E1;
    const Export* Exp2 = (const Export*) E2;

    /* Get the priorities of the two exports */
    unsigned Prio1 = Exp1->ConDes[Type];
    unsigned Prio2 = Exp2->ConDes[Type];

    /* Compare the priorities for this condes type */
    if (Prio1 < Prio2) {
        Cmp = -1;
    } else if (Prio1 > Prio2) {
        Cmp = 1;
    } else {
        /* Use the name in this case */
        Cmp = SB_Compare (GetStrBuf (Exp1->Name), GetStrBuf (Exp2->Name));
    }

    /* Reverse the result for decreasing order */
    if (CD->Order == cdIncreasing) {
        return Cmp;
    } else {
        return -Cmp;
    }
}



static void ConDesCreateOne (ConDesDesc* CD)
/* Create one table if requested */
{
    Segment*    Seg;            /* Segment for table */
    Section*    Sec;            /* Section for table */
    unsigned    Count;          /* Number of exports */
    unsigned    I;

    /* Check if this table has a segment and table label defined. If not,
    ** creation was not requested in the config file - ignore it.
    */
    if (CD->SegName == INVALID_STRING_ID || CD->Label == INVALID_STRING_ID) {
        return;
    }

    /* Check if there is an import for the table label. If not, there is no
    ** reference to the table and we would just waste memory creating the
    ** table.
    */
    if (!IsUnresolved (CD->Label)) {
        return;
    }

    /* Sort the collection of exports according to priority */
    CollSort (&CD->ExpList, ConDesCompare, CD);

    /* Get the segment for the table, create it if needed */
    Seg = GetSegment (CD->SegName, ADDR_SIZE_ABS, 0);

    /* Create a new section for the table */
    Sec = NewSection (Seg, 1, ADDR_SIZE_ABS);

    /* Walk over the exports and create a fragment for each one. We will use
    ** the exported expression without copying it, since it's cheap and there
    ** is currently no place where it gets changed (hope this will not hunt
    ** me later...).
    */
    Count = CollCount (&CD->ExpList);
    for (I = 0; I < Count; ++I) {

        /* Get the export */
        Export* E = CollAt (&CD->ExpList, I);

        /* Create the fragment */
        Fragment* F = NewFragment (FRAG_EXPR, 2, Sec);

        /* Set the expression pointer */
        F->Expr = E->Expr;
    }

    /* Define the table start as an export, offset into section is zero
    ** (the section only contains the table).
    */
    CreateSectionExport (CD->Label, Sec, 0);

    /* If we have a CountSym name given AND if it is referenced, define it
    ** with the number of elements in the table.
    */
    if (CD->CountSym) {
        CreateConstExport (CD->CountSym, Count);
    }
}



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void ConDesAddExport (struct Export* E)
/* Add the given export to the list of constructors/destructor */
{
    unsigned Type;

    /* Insert the export into all tables for which declarations exist */
    for (Type = 0; Type < CD_TYPE_COUNT; ++Type) {
        unsigned Prio = E->ConDes[Type];
        if (Prio != CD_PRIO_NONE) {
            CollAppend (&ConDes[Type].ExpList, E);
        }
    }
}



void ConDesSetSegName (unsigned Type, unsigned SegName)
/* Set the segment name where the table should go */
{
    /* Check the parameters */
    PRECONDITION (Type <= CD_TYPE_MAX && SegName != 0);

    /* Setting the segment name twice is bad */
    CHECK (ConDes[Type].SegName == INVALID_STRING_ID);

    /* Set the name */
    ConDes[Type].SegName = SegName;
}



const ConDesImport* ConDesGetImport (unsigned Type)
/* Get the forced import for the given ConDes type. Returns NULL if there is
** no forced import for this type.
*/
{
    const ConDesImport* Import;

    /* Check the parameters */
    PRECONDITION (Type <= CD_TYPE_MAX);

    /* Return the import */    
    Import = &ConDes[Type].Import;
    return (Import->Name != INVALID_STRING_ID)? Import : 0;
}



void ConDesSetImport (unsigned Type, const ConDesImport* Import)
/* Set the forced import for the given ConDes type */
{
    /* Check the parameters */
    PRECONDITION (Type <= CD_TYPE_MAX && Import != 0);

    /* Setting the import twice is bad */
    CHECK (ConDes[Type].Import.Name == INVALID_STRING_ID);

    /* Set the import and its position */
    ConDes[Type].Import = *Import;
}



void ConDesSetLabel (unsigned Type, unsigned Name)
/* Set the label for the given ConDes type */
{
    /* Check the parameters */
    PRECONDITION (Type <= CD_TYPE_MAX && Name != 0);

    /* Setting the label twice is bad */
    CHECK (ConDes[Type].Label == INVALID_STRING_ID);

    /* Set the name */
    ConDes[Type].Label = Name;
}



void ConDesSetCountSym (unsigned Type, unsigned Name)
/* Set the name for the given ConDes count symbol */
{
    /* Check the parameters */
    PRECONDITION (Type <= CD_TYPE_MAX && Name != 0);

    /* Setting the symbol twice is bad */
    CHECK (ConDes[Type].CountSym == INVALID_STRING_ID);

    /* Set the name */
    ConDes[Type].CountSym = Name;
}



void ConDesSetOrder (unsigned Type, ConDesOrder Order)
/* Set the sorting oder for the given ConDes table */
{
    /* Check the parameters */
    PRECONDITION (Type <= CD_TYPE_MAX);

    /* Set the order */
    ConDes[Type].Order = Order;
}



int ConDesHasSegName (unsigned Type)
/* Return true if a segment name is already defined for this ConDes type */
{
    /* Check the parameters */
    PRECONDITION (Type <= CD_TYPE_MAX);

    return (ConDes[Type].SegName != INVALID_STRING_ID);
}



int ConDesHasLabel (unsigned Type)
/* Return true if a label is already defined for this ConDes type */
{
    /* Check the parameters */
    PRECONDITION (Type <= CD_TYPE_MAX);

    return (ConDes[Type].Label != INVALID_STRING_ID);
}



void ConDesCreate (void)
/* Create the condes tables if requested */
{
    unsigned Type;

    /* Walk over the descriptor array and create a table for each entry */
    for (Type = 0; Type < CD_TYPE_COUNT; ++Type) {
        ConDesCreateOne (ConDes + Type);
    }
}



void ConDesDump (void)
/* Dump ConDes data to stdout for debugging */
{
    unsigned Type;
    for (Type = 0; Type < CD_TYPE_COUNT; ++Type) {
        Collection* ExpList = &ConDes[Type].ExpList;
        printf ("CONDES(%u): %u symbols\n", Type, CollCount (ExpList));
    }
}
