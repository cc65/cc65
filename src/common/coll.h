/*****************************************************************************/
/*                                                                           */
/*                                  coll.h                                   */
/*                                                                           */
/*                        Collection (dynamic array)                         */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2011, Ullrich von Bassewitz                                      */
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



#ifndef COLL_H
#define COLL_H



/* common */
#include "attrib.h"
#include "check.h"
#include "inline.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* An array of pointers that grows if needed */
typedef struct Collection Collection;
struct Collection {
    unsigned            Count;          /* Number of items in the list */
    unsigned            Size;           /* Size of allocated array */
    void**              Items;          /* Array with dynamic size */
};

/* An empty collection */
extern const Collection EmptyCollection;

/* Initializer for static collections */
#define STATIC_COLLECTION_INITIALIZER   { 0, 0, 0 }

/* Initializer for auto collections */
#define AUTO_COLLECTION_INITIALIZER     EmptyCollection



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



Collection* InitCollection (Collection* C);
/* Initialize a collection and return it. */

void DoneCollection (Collection* C);
/* Free the data for a collection. This will not free the data contained in
** the collection.
*/

Collection* NewCollection (void);
/* Create and return a new collection */

void FreeCollection (Collection* C);
/* Free a collection */

void CollGrow (Collection* C, unsigned Size);
/* Grow the collection C so it is able to hold Size items without a resize
** being necessary. This can be called for performance reasons if the number
** of items to be placed in the collection is known in advance.
*/

#if defined(HAVE_INLINE)
INLINE unsigned CollCount (const Collection* C)
/* Return the number of items in the collection */
{
    return C->Count;
}
#else
#  define CollCount(C)  (C)->Count
#endif

void CollInsert (Collection* C, void* Item, unsigned Index);
/* Insert the data at the given position in the collection */

#if defined(HAVE_INLINE)
INLINE void CollAppend (Collection* C, void* Item)
/* Append an item to the end of the collection */
{
    /* Insert the item at the end of the current list */
    CollInsert (C, Item, C->Count);
}
#else
void CollAppend (Collection* C, void* Item);
/* Append an item to the end of the collection */
#endif

#if defined(HAVE_INLINE)
INLINE void* CollAt (const Collection* C, unsigned Index)
/* Return the item at the given index */
{
    /* Check the index */
    PRECONDITION (Index < C->Count);

    /* Return the element */
    return C->Items[Index];
}
#else
void* CollAt (const Collection* C, unsigned Index);
/* Return the item at the given index */
#endif

#if defined(HAVE_INLINE)
INLINE void* CollAtUnchecked (const Collection* C, unsigned Index)
/* Return the item at the given index */
{
    /* Return the element */
    return C->Items[Index];
}
#else
#  define CollAtUnchecked(C, Index)     ((C)->Items[(Index)])
#endif

#if defined(HAVE_INLINE)
INLINE const void* CollConstAt (const Collection* C, unsigned Index)
/* Return the item at the given index */
{
    /* Check the index */
    PRECONDITION (Index < C->Count);

    /* Return the element */
    return C->Items[Index];
}
#else
const void* CollConstAt (const Collection* C, unsigned Index);
/* Return the item at the given index */
#endif

#if defined(HAVE_INLINE)
INLINE void* CollLast (Collection* C)
/* Return the last item in a collection */
{
    /* We must have at least one entry */
    PRECONDITION (C->Count > 0);

    /* Return the element */
    return C->Items[C->Count-1];
}
#else
void* CollLast (Collection* C);
/* Return the last item in a collection */
#endif

#if defined(HAVE_INLINE)
INLINE const void* CollConstLast (const Collection* C)
/* Return the last item in a collection */
{
    /* We must have at least one entry */
    PRECONDITION (C->Count > 0);

    /* Return the element */
    return C->Items[C->Count-1];
}
#else
const void* CollConstLast (const Collection* C);
/* Return the last item in a collection */
#endif

#if defined(HAVE_INLINE)
INLINE void* CollPop (Collection* C)
/* Remove the last segment from the stack and return it. Calls FAIL if the
** collection is empty.
*/
{
    /* We must have at least one entry */
    PRECONDITION (C->Count > 0);

    /* Return the element */
    return C->Items[--C->Count];
}
#else
void* CollPop (Collection* C);
/* Remove the last segment from the stack and return it. Calls FAIL if the
** collection is empty.
*/
#endif

int CollIndex (Collection* C, const void* Item);
/* Return the index of the given item in the collection. Return -1 if the
** item was not found in the collection.
*/

void CollDelete (Collection* C, unsigned Index);
/* Remove the item with the given index from the collection. This will not
** free the item itself, just the pointer. All items with higher indices
** will get moved to a lower position.
*/

void CollDeleteItem (Collection* C, const void* Item);
/* Delete the item pointer from the collection. The item must be in the
** collection, otherwise FAIL will be called.
*/

#if defined(HAVE_INLINE)
INLINE void CollDeleteAll (Collection* C)
/* Delete all items from the given collection. This will not free the items
** itself, it will only remove the pointers.
*/
{
    /* This one is easy... */
    C->Count = 0;
}
#else
#  define CollDeleteAll(C)      ((C)->Count = 0)
#endif

#if defined(HAVE_INLINE)
INLINE void CollReplace (Collection* C, void* Item, unsigned Index)
/* Replace the item at the given position. The old item will not be freed,
** just the pointer will get replaced.
*/
{
    /* Check the index */
    PRECONDITION (Index < C->Count);

    /* Replace the item pointer */
    C->Items[Index] = Item;
}
#else
void CollReplace (Collection* C, void* Item, unsigned Index);
/* Replace the item at the given position. The old item will not be freed,
** just the pointer will get replaced.
*/
#endif

void CollReplaceExpand (Collection* C, void* Item, unsigned Index);
/* If Index is a valid index for the collection, replace the item at this
** position by the one passed. If the collection is too small, expand it,
** filling unused pointers with NULL, then add the new item at the given
** position.
*/

void CollMove (Collection* C, unsigned OldIndex, unsigned NewIndex);
/* Move an item from one position in the collection to another. OldIndex
** is the current position of the item, NewIndex is the new index after
** the function has done it's work. Existing entries with indices NewIndex
** and up are moved one position upwards.
*/

void CollMoveMultiple (Collection* C, unsigned Start, unsigned Count, unsigned Target);
/* Move a range of items from one position to another. Start is the index
** of the first item to move, Count is the number of items and Target is
** the index of the target item. The item with the index Start will later
** have the index Target. All items with indices Target and above are moved
** to higher indices.
*/

void CollTransfer (Collection* Dest, const Collection* Source);
/* Transfer all items from Source to Dest. Anything already in Dest is left
** untouched. The items in Source are not changed and are therefore in both
** Collections on return.
*/

void CollSort (Collection* C,
               int (*Compare) (void*, const void*, const void*),
               void* Data);
/* Sort the collection using the given compare function. The data pointer is
** passed as *first* element to the compare function, it's not used by the
** sort function itself. The other two pointer passed to the Compare function
** are pointers to objects.
*/



/* End of coll.h */

#endif
