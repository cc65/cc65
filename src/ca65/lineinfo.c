/*****************************************************************************/
/*                                                                           */
/*				  lineinfo.c                                 */
/*                                                                           */
/*			Source file line info structure                      */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001-2011, Ullrich von Bassewitz                                      */
/*                Roemerstrasse 52                                           */
/*                70794 Filderstadt                                          */
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
#include <limits.h>

/* common */
#include "coll.h"
#include "xmalloc.h"

/* ca65 */
#include "global.h"
#include "lineinfo.h"
#include "objfile.h"



/*****************************************************************************/
/*  		       		     Data                                    */
/*****************************************************************************/



/* An invalid line info index */
#define INV_LINEINFO_INDEX      UINT_MAX

/* Collection containing all line infos */
static Collection LineInfoColl = STATIC_COLLECTION_INITIALIZER;

/* Number of valid (=used) line infos in LineInfoColl */
static unsigned UsedLineInfoCount;

/* Entry in CurLineInfo */
typedef struct LineInfoSlot LineInfoSlot;
struct LineInfoSlot {
    unsigned    Type;
    LineInfo*   Info;
};

/* Dynamically allocated array of LineInfoSlots */
static LineInfoSlot* CurLineInfo;
static unsigned AllocatedSlots;
static unsigned UsedSlots;



/*****************************************************************************/
/*                              struct LineInfo                              */
/*****************************************************************************/



static LineInfo* NewLineInfo (unsigned Type, const FilePos* Pos)
/* Create and return a new line info. Usage will be zero. */
{
    /* Allocate memory */
    LineInfo* LI = xmalloc (sizeof (LineInfo));

    /* Initialize the fields */
    LI->Usage   = 0;
    LI->Type    = Type;
    LI->Index   = INV_LINEINFO_INDEX;
    LI->Pos     = *Pos;

    /* Add the line info to the list of all line infos */
    CollAppend (&LineInfoColl, LI);

    /* Return the new struct */
    return LI;
}



/*****************************************************************************/
/*     	       	       	      	     Code			     	     */
/*****************************************************************************/



void InitLineInfo (void)
/* Initialize the line infos */
{
    static const FilePos DefaultPos = STATIC_FILEPOS_INITIALIZER;

    /* Increase the initial count of the line info collection */
    CollGrow (&LineInfoColl, 200);

    /* Allocate 8 slots */
    AllocatedSlots = 8;
    CurLineInfo = xmalloc (AllocatedSlots * sizeof (LineInfoSlot));

    /* Initalize the predefined slots. Be sure to create a new LineInfo for
     * the default source. This is necessary to allow error message to be
     * generated without any input file open.
     */
    UsedSlots = 2;
    CurLineInfo[LI_SLOT_ASM].Type = LI_TYPE_ASM;        /* Count = 0 */
    CurLineInfo[LI_SLOT_ASM].Info = NewLineInfo (LI_TYPE_ASM, &DefaultPos);
    CurLineInfo[LI_SLOT_EXT].Type = LI_TYPE_EXT;        /* Count = 0 */
    CurLineInfo[LI_SLOT_EXT].Info = 0;
}



int AllocLineInfoSlot (unsigned Type, unsigned Count)
/* Allocate a line info slot of the given type and return the slot index */
{
    /* Grow the array if necessary */
    if (UsedSlots >= AllocatedSlots) {
        LineInfoSlot* NewLineInfo;
        AllocatedSlots *= 2;
        NewLineInfo = xmalloc (AllocatedSlots * sizeof (LineInfoSlot));
        memcpy (NewLineInfo, CurLineInfo, UsedSlots * sizeof (LineInfoSlot));
        xfree (CurLineInfo);
        CurLineInfo = NewLineInfo;
    }

    /* Array is now big enough, add the new data */
    CurLineInfo[UsedSlots].Type = LI_MAKE_TYPE(Type, Count);
    CurLineInfo[UsedSlots].Info = 0;

    /* Increment the count and return the index of the new slot */
    return (int) UsedSlots++;
}



void FreeLineInfoSlot (int Slot)
/* Free the line info in the given slot. Note: Alloc/Free must be used in
 * FIFO order.
 */
{
    /* Check the parameter */
    PRECONDITION (Slot == (int) UsedSlots - 1);

    /* Free the last entry */
    CurLineInfo[Slot].Info = 0;
    --UsedSlots;
}



void GenLineInfo (int Slot, const FilePos* Pos)
/* Generate a new line info in the given slot */
{
    /* Get a pointer to the slot */
    LineInfoSlot* S = CurLineInfo + Slot;

    /* Generate new data only if it is different from the existing. */
    if (S->Info && CompareFilePos (&S->Info->Pos, Pos) == 0) {
        /* Already there */
        return;
    }

    /* Allocate new data */
    S->Info = NewLineInfo (S->Type, Pos);
}



void ClearLineInfo (int Slot)
/* Clear the line info in the given slot */
{
    /* Zero the pointer */
    CurLineInfo[Slot].Info = 0;
}



void GetFullLineInfo (Collection* LineInfos, unsigned IncUsage)
/* Return full line infos, that is line infos for all slots in LineInfos. The
 * function will clear LineInfos before usage and will increment the usage
 * counter by IncUsage for all line infos returned.
 */
{
    unsigned I;

    /* Clear the collection */
    CollDeleteAll (LineInfos);

    /* Copy all valid line infos to the collection */
    for (I = 0; I < UsedSlots; ++I) {

        /* Get the line info from the slot */
        LineInfo* LI = CurLineInfo[I].Info;

        /* Ignore empty slots */
        if (LI) {
            LI->Usage += IncUsage;
            CollAppend (LineInfos, LI);
        }
    }
}



LineInfo* ReleaseLineInfo (LineInfo* LI)
/* Decrease the reference count of the given line info and return it. The
 * function will gracefully accept NULL pointers and do nothing in this case.
 */
{
    if (LI) {
        /* Cannot decrease below zero */
       	CHECK (LI->Usage != 0);
        --LI->Usage;
    }
    return LI;
}



static int CmpLineInfo (void* Data attribute ((unused)),
      	 		const void* LI1_, const void* LI2_)
/* Compare function for the sort */
{
    /* Cast the pointers */
    const LineInfo* LI1 = LI1_;
    const LineInfo* LI2 = LI2_;

    /* Unreferenced line infos are always larger, otherwise sort by file,
     * then by line, then by column.
     */
    if ((LI1->Usage == 0) == (LI2->Usage == 0)) {
	/* Both are either referenced or unreferenced */
        return CompareFilePos (&LI1->Pos, &LI2->Pos);
    } else {
	if (LI1->Usage > 0) {
	    return -1;
	} else {
	    return 1;
	}
    }
}



void WriteLineInfo (const Collection* LineInfos)
/* Write a list of line infos to the object file. MakeLineInfoIndex has to
 * be called before!
 */
{
    unsigned I;

    /* Write the count */
    ObjWriteVar (CollCount (LineInfos));

    /* Write the line info indices */
    for (I = 0; I < CollCount (LineInfos); ++I) {

        /* Get a pointer to the line info */
        const LineInfo* LI = CollConstAt (LineInfos, I);

        /* Check the index */
        CHECK (LI->Index != INV_LINEINFO_INDEX);

        /* Write the index to the file */
        ObjWriteVar (LI->Index);
    }
}



void MakeLineInfoIndex (void)
/* Index the line infos */
{
    unsigned I;

    /* Sort the line info list */
    CollSort (&LineInfoColl, CmpLineInfo, 0);

    /* Walk over the list, index the line infos and count the used ones */
    UsedLineInfoCount = 0;
    for (I = 0; I < CollCount (&LineInfoColl); ++I) {
	/* Get a pointer to this line info */
	LineInfo* LI = CollAtUnchecked (&LineInfoColl, I);

        /* If it is invalid, terminate the loop. All unused line infos were
         * placed at the end of the collection by the sort.
         */
        if (LI->Usage == 0) {
            break;
        }

        /* Index and count this one */
        LI->Index = I;
        ++UsedLineInfoCount;
    }
}



void WriteLineInfos (void)
/* Write a list of all line infos to the object file. */
{
    unsigned I;

    /* Tell the object file module that we're about to write line infos */
    ObjStartLineInfos ();

    /* Write the line info count to the list */
    ObjWriteVar (UsedLineInfoCount);

    /* Walk over the list and write all line infos */
    for (I = 0; I < UsedLineInfoCount; ++I) {
        /* Get a pointer to this line info */
        LineInfo* LI = CollAt (&LineInfoColl, I);

        /* Write the type and count of the line info */
        ObjWriteVar (LI->Type);

        /* Write the source file position */
        ObjWritePos (&LI->Pos);
    }

    /* End of line infos */
    ObjEndLineInfos ();
}



