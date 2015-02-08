/*****************************************************************************/
/*                                                                           */
/*                                 segment.c                                 */
/*                                                                           */
/*                         Segment handling for da65                         */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2007-2014, Ullrich von Bassewitz                                      */
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
#include "xmalloc.h"

/* da65 */
#include "attrtab.h"
#include "segment.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Hash definitions */
#define HASH_SIZE       53

/* Segment definition */
typedef struct Segment Segment;
struct Segment {
    Segment*            NextStart;      /* Pointer to next segment */
    unsigned long       Start;
    unsigned            AddrSize;
    char                Name[1];        /* Name, dynamically allocated */
};

/* Table containing the segments. A segment is inserted using its hash
** value. Collisions are handled by single-linked lists.
*/
static Segment* StartTab[HASH_SIZE];    /* Table containing segment starts */



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void AddAbsSegment (unsigned Start, unsigned End, const char* Name)
/* Add an absolute segment to the segment table */
{
    /* Get the length of the name */
    unsigned Len = strlen (Name);

    /* Create a new segment */
    Segment* S = xmalloc (sizeof (Segment) + Len);

    /* Fill in the data */
    S->Start    = Start;
    S->AddrSize = ADDR_SIZE_ABS;
    memcpy (S->Name, Name, Len + 1);

    /* Insert the segment into the hash table */
    S->NextStart = StartTab[Start % HASH_SIZE];
    StartTab[Start % HASH_SIZE] = S;

    /* Mark start and end of the segment */
    MarkAddr (Start, atSegmentStart);
    MarkAddr (End, atSegmentEnd);

    /* Mark the addresses within the segment */
    MarkRange (Start, End, atSegment);
}



char* GetSegmentStartName (unsigned Addr)
/* Return the name of the segment which starts at the given address */
{
    Segment* S = StartTab[Addr % HASH_SIZE];

    /* Search the collision list for the exact address */
    while (S != 0) {
        if (S->Start == Addr) {
            return S->Name;
        }
        S = S->NextStart;
    }

    return 0;
}



unsigned GetSegmentAddrSize (unsigned Addr)
/* Return the address size of the segment which starts at the given address */
{
    Segment* S = StartTab[Addr % HASH_SIZE];

    /* Search the collision list for the exact address */
    while (S != 0) {
        if (S->Start == Addr) {
            return S->AddrSize;
        }
        S = S->NextStart;
    }

    return 0;
}
