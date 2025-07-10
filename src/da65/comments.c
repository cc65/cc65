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



#include <inttypes.h>
#include <string.h>

/* common */
#include "xmalloc.h"

/* da65 */
#include "attrtab.h"
#include "comments.h"
#include "error.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Comment structure how it is found in the comment table */
typedef struct Comment Comment;
struct Comment {
    struct Comment*     Next;           /* Next entry in linked list */
    uint32_t            Addr;           /* The full address */
    char                Text[1];        /* Text, dynamically allocated */
};

/* Comments use a hash table and a linear list for collision resolution. The
** hash function is easy and effective. It evaluates just the lower bits of
** the address. Since we don't expect many comments, we can keep the table
** small.
*/
#define COMMENT_HASH_SIZE       256u    /* Must be power of two */
static Comment* CommentTab[COMMENT_HASH_SIZE];



/*****************************************************************************/
/*                              struct Comment                               */
/*****************************************************************************/



static Comment* NewComment (uint32_t Addr, const char* Text)
/* Create a new comment structure and return it */
{
    /* Get the length of the text */
    unsigned Len = strlen (Text);

    /* Create a new comment */
    Comment* C = xmalloc (sizeof (Comment) + Len);

    /* Fill in the data */
    C->Next = 0;
    C->Addr = Addr;
    memcpy (C->Text, Text, Len + 1);

    /* Return the comment just created */
    return C;
}



static uint32_t GetCommentHash (uint32_t Addr)
/* Get the hash for a comment at the given address */
{
    return (Addr & (COMMENT_HASH_SIZE - 1));
}



static Comment* FindComment (uint32_t Addr)
/* Search for a comment for the given address and return it. Returns NULL if
** no comment exists for the address.
*/
{
    Comment* C = CommentTab[GetCommentHash (Addr)];
    while (C) {
        if (C->Addr == Addr) {
            break;
        }
        C = C->Next;
    }
    return C;
}



static void InsertComment (Comment* C)
/* Insert a comment into the hash table */
{
    uint32_t Hash = GetCommentHash (C->Addr);
    C->Next = CommentTab[Hash];
    CommentTab[Hash] = C;
}



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void SetComment (uint32_t Addr, const char* Text)
/* Set a comment for the given address */
{
    /* Check the given address */
    AddrCheck (Addr);

    /* If we do already have a comment, warn and ignore the new one */
    Comment* C = FindComment (Addr);
    if (C) {
        Warning ("Duplicate comment for address $%04" PRIX32, Addr);
    } else {
        InsertComment (NewComment (Addr, Text));
    }
}



const char* GetComment (uint32_t Addr)
/* Return the comment for an address */
{
    /* Check the given address */
    AddrCheck (Addr);

    /* Check for a comment and return it */
    const Comment* C = FindComment (Addr);
    return C? C->Text : 0;
}
