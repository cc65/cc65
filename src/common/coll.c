/*****************************************************************************/
/*                                                                           */
/*                                  coll.c                                   */
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



#include <stdlib.h>
#include <string.h>

/* common */
#include "check.h"
#include "xmalloc.h"

/* cc65 */
#include "coll.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* An empty collection */
const Collection EmptyCollection = STATIC_COLLECTION_INITIALIZER;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



Collection* InitCollection (Collection* C)
/* Initialize a collection and return it. */
{
    /* Intialize the fields. */
    C->Count = 0;
    C->Size  = 0;
    C->Items = 0;

    /* Return the new struct */
    return C;
}



void DoneCollection (Collection* C)
/* Free the data for a collection. This will not free the data contained in
** the collection.
*/
{
    /* Free the pointer array */
    xfree (C->Items);
}



Collection* NewCollection (void)
/* Create and return a new collection with the given initial size */
{
    /* Allocate memory, intialize the collection and return it */
    return InitCollection (xmalloc (sizeof (Collection)));
}



void FreeCollection (Collection* C)
/* Free a collection */
{
    /* Free the data */
    DoneCollection (C);

    /* Free the structure itself */
    xfree (C);
}



void CollGrow (Collection* C, unsigned Size)
/* Grow the collection C so it is able to hold Size items without a resize
** being necessary. This can be called for performance reasons if the number
** of items to be placed in the collection is known in advance.
*/
{
    void** NewItems;

    /* Ignore the call if the collection is already large enough */
    if (Size <= C->Size) {
        return;
    }

    /* Grow the collection */
    C->Size = Size;
    NewItems = xmalloc (C->Size * sizeof (void*));
    memcpy (NewItems, C->Items, C->Count * sizeof (void*));
    xfree (C->Items);
    C->Items = NewItems;
}



void CollInsert (Collection* C, void* Item, unsigned Index)
/* Insert the data at the given position in the collection */
{
    /* Check for invalid indices */
    PRECONDITION (Index <= C->Count);

    /* Grow the array if necessary */
    if (C->Count >= C->Size) {
        /* Must grow */
        CollGrow (C, (C->Size == 0)? 4 : C->Size * 2);
    }

    /* Move the existing elements if needed */
    if (C->Count != Index) {
        memmove (C->Items+Index+1, C->Items+Index, (C->Count-Index) * sizeof (void*));
    }
    ++C->Count;

    /* Store the new item */
    C->Items[Index] = Item;
}



#if !defined(HAVE_INLINE)
void CollAppend (Collection* C, void* Item)
/* Append an item to the end of the collection */
{
    /* Insert the item at the end of the current list */
    CollInsert (C, Item, C->Count);
}                      
#endif



#if !defined(HAVE_INLINE)
void* CollAt (const Collection* C, unsigned Index)
/* Return the item at the given index */
{
    /* Check the index */
    PRECONDITION (Index < C->Count);

    /* Return the element */
    return C->Items[Index];
}
#endif



#if !defined(HAVE_INLINE)
const void* CollConstAt (const Collection* C, unsigned Index)
/* Return the item at the given index */
{
    /* Check the index */
    PRECONDITION (Index < C->Count);

    /* Return the element */
    return C->Items[Index];
}
#endif



#if !defined(HAVE_INLINE)
void* CollLast (Collection* C)
/* Return the last item in a collection */
{
    /* We must have at least one entry */
    PRECONDITION (C->Count > 0);

    /* Return the element */
    return C->Items[C->Count-1];
}
#endif



#if !defined(HAVE_INLINE)
const void* CollConstLast (const Collection* C)
/* Return the last item in a collection */
{
    /* We must have at least one entry */
    PRECONDITION (C->Count > 0);

    /* Return the element */
    return C->Items[C->Count-1];
}
#endif



#if !defined(HAVE_INLINE)
void* CollPop (Collection* C)
/* Remove the last segment from the stack and return it. Calls FAIL if the
** collection is empty.
*/
{
    /* We must have at least one entry */
    PRECONDITION (C->Count > 0);

    /* Return the element */
    return C->Items[--C->Count];
}
#endif



int CollIndex (Collection* C, const void* Item)
/* Return the index of the given item in the collection. Return -1 if the
** item was not found in the collection.
*/
{
    /* Linear search */
    unsigned I;
    for (I = 0; I < C->Count; ++I) {
        if (Item == C->Items[I]) {
            /* Found */
            return (int)I;
        }
    }

    /* Not found */
    return -1;
}



void CollDelete (Collection* C, unsigned Index)
/* Remove the item with the given index from the collection. This will not
** free the item itself, just the pointer. All items with higher indices
** will get moved to a lower position.
*/
{
    /* Check the index */
    PRECONDITION (Index < C->Count);

    /* Remove the item pointer */
    --C->Count;
    memmove (C->Items+Index, C->Items+Index+1, (C->Count-Index) * sizeof (void*));
}



void CollDeleteItem (Collection* C, const void* Item)
/* Delete the item pointer from the collection. The item must be in the
** collection, otherwise FAIL will be called.
*/
{
    /* Get the index of the entry */
    int Index = CollIndex (C, Item);
    CHECK (Index >= 0);

    /* Delete the item with this index */
    --C->Count;
    memmove (C->Items+Index, C->Items+Index+1, (C->Count-Index) * sizeof (void*));
}



#if !defined(HAVE_INLINE)
void CollReplace (Collection* C, void* Item, unsigned Index)
/* Replace the item at the given position. The old item will not be freed,
** just the pointer will get replaced.
*/
{
    /* Check the index */
    PRECONDITION (Index < C->Count);

    /* Replace the item pointer */
    C->Items[Index] = Item;
}
#endif



void CollReplaceExpand (Collection* C, void* Item, unsigned Index)
/* If Index is a valid index for the collection, replace the item at this
** position by the one passed. If the collection is too small, expand it,
** filling unused pointers with NULL, then add the new item at the given
** position.
*/
{
    if (Index < C->Count) {
        /* Collection is already large enough */
        C->Items[Index] = Item;
    } else {
        /* Must expand the collection */
        unsigned Size = C->Size;
        if (Size == 0) {
            Size = 4;
        }
        while (Index >= Size) {
            Size *= 2;
        }
        CollGrow (C, Size);

        /* Fill up unused slots with NULL */
        while (C->Count < Index) {
            C->Items[C->Count++] = 0;
        }

        /* Fill in the item */
        C->Items[C->Count++] = Item;
    }
}



void CollMove (Collection* C, unsigned OldIndex, unsigned NewIndex)
/* Move an item from one position in the collection to another. OldIndex
** is the current position of the item, NewIndex is the new index after
** the function has done it's work. Existing entries with indices NewIndex
** and up are moved one position upwards.
*/
{
    /* Get the item and remove it from the collection */
    void* Item = CollAt (C, OldIndex);
    CollDelete (C, OldIndex);

    /* Correct NewIndex if needed */
    if (NewIndex >= OldIndex) {
        /* Position has changed with removal */
        --NewIndex;
    }

    /* Now insert it at the new position */
    CollInsert (C, Item, NewIndex);
}



void CollMoveMultiple (Collection* C, unsigned Start, unsigned Count, unsigned Target)
/* Move a range of items from one position to another. Start is the index
** of the first item to move, Count is the number of items and Target is
** the index of the target item. The item with the index Start will later
** have the index Target. All items with indices Target and above are moved
** to higher indices.
*/
{
    void** TmpItems;
    unsigned Bytes;

    /* Check the range */
    PRECONDITION (Start < C->Count && Start + Count <= C->Count && Target <= C->Count);

    /* Check for trivial parameters */
    if (Count == 0 || Start == Target) {
        return;
    }

    /* Calculate the raw memory space used by the items to move */
    Bytes = Count * sizeof (void*);

    /* Allocate temporary storage for the items */
    TmpItems = xmalloc (Bytes);

    /* Copy the items we have to move to the temporary storage */
    memcpy (TmpItems, C->Items + Start, Bytes);

    /* Check if the range has to be moved upwards or downwards. Move the
    ** existing items to their final location, so that the space needed
    ** for the items now in temporary storage is unoccupied.
    */
    if (Target < Start) {

        /* Move downwards */
        unsigned BytesToMove = (Start - Target) * sizeof (void*);
        memmove (C->Items+Target+Count, C->Items+Target, BytesToMove);

    } else if (Target < Start + Count) {

        /* Target is inside range */
        FAIL ("Not supported");

    } else {

        /* Move upwards */
        unsigned ItemsToMove = (Target - Start - Count);
        unsigned BytesToMove = ItemsToMove * sizeof (void*);
        memmove (C->Items+Start, C->Items+Target-ItemsToMove, BytesToMove);

        /* Adjust the target index */
        Target -= Count;
    }

    /* Move the old items to their final location */
    memcpy (C->Items + Target, TmpItems, Bytes);

    /* Delete the temporary item space */
    xfree (TmpItems);
}



static void QuickSort (Collection* C, int Lo, int Hi,
                       int (*Compare) (void*, const void*, const void*),
                       void* Data)
/* Internal recursive sort function. */
{
    /* Get a pointer to the items */
    void** Items = C->Items;

    /* Quicksort */
    while (Hi > Lo) {
        int I = Lo + 1;
        int J = Hi;
        while (I <= J) {
            while (I <= J && Compare (Data, Items[Lo], Items[I]) >= 0) {
                ++I;
            }
            while (I <= J && Compare (Data, Items[Lo], Items[J]) < 0) {
                --J;
            }
            if (I <= J) {
                /* Swap I and J */
                void* Tmp = Items[I];
                Items[I]  = Items[J];
                Items[J]  = Tmp;
                ++I;
                --J;
            }
        }
        if (J != Lo) {
            /* Swap J and Lo */
            void* Tmp = Items[J];
            Items[J]  = Items[Lo];
            Items[Lo] = Tmp;
        }
        if (J > (Hi + Lo) / 2) {
            QuickSort (C, J + 1, Hi, Compare, Data);
            Hi = J - 1;
        } else {
            QuickSort (C, Lo, J - 1, Compare, Data);
            Lo = J + 1;
        }
    }
}



void CollTransfer (Collection* Dest, const Collection* Source)
/* Transfer all items from Source to Dest. Anything already in Dest is left
** untouched. The items in Source are not changed and are therefore in both
** Collections on return.
*/
{
    /* Be sure there's enough room in Dest */
    CollGrow (Dest, Dest->Count + Source->Count);

    /* Copy the items */
    memcpy (Dest->Items + Dest->Count,
            Source->Items,
            Source->Count * sizeof (Source->Items[0]));

    /* Bump the counter */
    Dest->Count += Source->Count;
}



void CollSort (Collection* C,
               int (*Compare) (void*, const void*, const void*),
               void* Data)
/* Sort the collection using the given compare function. The data pointer is
** passed as *first* element to the compare function, it's not used by the
** sort function itself. The other two pointer passed to the Compare function
** are pointers to objects.
*/
{
    if (C->Count > 1) {
        QuickSort (C, 0, C->Count-1, Compare, Data);
    }
}
