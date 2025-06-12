/*****************************************************************************/
/*                                                                           */
/*                                comments.c                                 */
/*                                                                           */
/*                        Comment management for da65                        */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2006      Ullrich von Bassewitz                                       */
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



/* common */
#include "xmalloc.h"

/* da65 */
#include "attrtab.h"
#include "comments.h"
#include "error.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Comment table */
static const char* CommentTab[0x10000];

#define MAX_LONG_COMMENTS 256
static const char* LongCommentVal[MAX_LONG_COMMENTS];
static unsigned LongCommentAddr[MAX_LONG_COMMENTS];
static unsigned LongCommentsUsed;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static unsigned FindLongIndex (unsigned Addr)
{
    unsigned i;
    for (i = 0; i < LongCommentsUsed; i++) {
        if (LongCommentAddr[i] == Addr) {
            return i;
        }
    }
    return -1;
}



void SetComment (unsigned Addr, const char* Comment)
/* Set a comment for the given address */
{
    /* Check the given address */
    AddrCheck (Addr);

    if (IsLongAddr (Addr)) {
        if (FindLongIndex (Addr)) {
            Warning ("Duplicate comment for address $%06X", Addr);
        } else {
            if (LongCommentsUsed >= MAX_LONG_COMMENTS) {
                Error("Too many long-address comments");
            }
            LongCommentVal[LongCommentsUsed] = xstrdup (Comment);
            LongCommentAddr[LongCommentsUsed] = Addr;
            LongCommentsUsed++;
        }
    } else {
        /* If we do already have a comment, warn and ignore the new one */
        if (CommentTab[Addr]) {
            Warning ("Duplicate comment for address $%04X", Addr);
        } else {
            CommentTab[Addr] = xstrdup (Comment);
        }
    }
}



const char* GetComment (unsigned Addr)
/* Return the comment for an address */
{
    /* Check the given address */
    AddrCheck (Addr);

    if (IsLongAddr (Addr)) {
        const unsigned i = FindLongIndex (Addr);
        if (i < LongCommentsUsed) {
            return LongCommentVal[i];
        }
        return NULL;
    }

    /* Return the label if any */
    return CommentTab[Addr];
}
