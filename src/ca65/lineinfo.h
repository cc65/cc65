/*****************************************************************************/
/*                                                                           */
/*				  lineinfo.h                                 */
/*                                                                           */
/*                     Source file line info management                      */
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



#ifndef LINEINFO_H
#define LINEINFO_H



/* common */
#include "coll.h"
#include "filepos.h"



/*****************************************************************************/
/*		     		     Data                                    */
/*****************************************************************************/



/* Predefined line info slots. These are allocated when initializing the
 * module. Beware: Some code relies on the fact that slot zero is the basic
 * standard line info. It is assumed to be always there.
 */
enum {
    LI_SLOT_ASM         = 0,            /* Normal assembler source */
    LI_SLOT_EXT         = 1,            /* Externally supplied line info */
};

/* Types of line infos. The low byte may be used for some sort of depth
 * counter.
 */
enum {
    LI_MASK_COUNT       = 0x00FF,       /* Mask to extract the count */

    LI_TYPE_EXT         = 0x0100,       /* Externally supplied line info */
    LI_TYPE_ASM         = 0x0200,       /* Normal assembler source */
    LI_TYPE_MACRO       = 0x0300,       /* Macro expansion */
    LI_MASK_TYPE        = 0x7F00,       /* Mask to extract the type */
};

/* The LineInfo structure is shared between several fragments, so we need a
 * reference counter.
 */
typedef struct LineInfo LineInfo;
struct LineInfo {
    unsigned    Usage;                  /* Usage counter */
    unsigned    Type;                   /* Type of line info */
    unsigned    Index;                  /* Index */
    FilePos     Pos;                    /* File position */
};



/*****************************************************************************/
/*     	       	      	      	     Code			     	     */
/*****************************************************************************/



void InitLineInfo (void);
/* Initialize the line infos */

unsigned AllocLineInfoSlot (unsigned Type);
/* Allocate a line info slot of the given type and return the slot index */

void FreeLineInfoSlot (unsigned Slot);
/* Free the line info in the given slot. Note: Alloc/Free must be used in
 * FIFO order.
 */

void GenLineInfo (unsigned Slot, const FilePos* Pos);
/* Generate a new line info in the given slot */

void ClearLineInfo (unsigned Slot);
/* Clear the line info in the given slot */

LineInfo* GetLineInfo (unsigned Slot);
/* Get the line info from the given slot */

void GetFullLineInfo (Collection* LineInfos);
/* Return full line infos, that is line infos for all slots in LineInfos. The
 * function does also increase the usage counter for all line infos returned.
 */

LineInfo* UseLineInfo (LineInfo* LI);
/* Increase the reference count of the given line info and return it. The
 * function will gracefully accept NULL pointers and do nothing in this case.
 */

LineInfo* ReleaseLineInfo (LineInfo* LI);
/* Decrease the reference count of the given line info and return it. The
 * function will gracefully accept NULL pointers and do nothing in this case.
 */

void WriteLineInfo (const Collection* LineInfos);
/* Write a list of line infos to the object file. MakeLineInfoIndex has to
 * be called before!
 */

void MakeLineInfoIndex (void);
/* Index the line infos */

void WriteLineInfos (void);
/* Write a list of all line infos to the object file. */



/* End of lineinfo.h */
#endif



