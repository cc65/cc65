/*****************************************************************************/
/*                                                                           */
/*				    coll.c				     */
/*                                                                           */
/*			  Collection (dynamic array)			     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000	 Ullrich von Bassewitz                                       */
/*               Wacholderweg 14                                             */
/*               D-70597 Stuttgart                                           */
/* EMail:        uz@musoftware.de                                            */
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
/*     	      	       		     Code				     */
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
 * the collection.
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



unsigned CollCount (const Collection* C)
/* Return the number of items in the collection */
{
    return C->Count;
}



void CollInsert (Collection* C, void* Item, unsigned Index)
/* Insert the data at the given position in the collection */
{
    /* Check for invalid indices */
    PRECONDITION (Index <= C->Count);

    /* Grow the array if necessary */
    if (C->Count >= C->Size) {
    	/* Must grow */
       	void** NewItems;
	if (C->Size > 0) {
	    C->Size *= 2;
	} else {
	    C->Size = 8;
	}
    	NewItems = xmalloc (C->Size * sizeof (void*));
    	memcpy (NewItems, C->Items, C->Count * sizeof (void*));
    	xfree (C->Items);
    	C->Items = NewItems;
    }

    /* Move the existing elements if needed */
    if (C->Count != Index) {
    	memmove (C->Items+Index+1, C->Items+Index, (C->Count-Index) * sizeof (void*));
    }
    ++C->Count;

    /* Store the new item */
    C->Items[Index] = Item;
}



void CollAppend (Collection* C, void* Item)
/* Append an item to the end of the collection */
{
    /* Insert the item at the end of the current list */
    CollInsert (C, Item, C->Count);
}



void* CollAt (Collection* C, unsigned Index)
/* Return the item at the given index */
{
    /* Check the index */
    PRECONDITION (Index < C->Count);

    /* Return the element */
    return C->Items[Index];
}



const void* CollConstAt (const Collection* C, unsigned Index)
/* Return the item at the given index */
{
    /* Check the index */
    PRECONDITION (Index < C->Count);

    /* Return the element */
    return C->Items[Index];
}



void* CollLast (Collection* C)
/* Return the last item in a collection */
{
    /* We must have at least one entry */
    PRECONDITION (C->Count > 0);

    /* Return the element */
    return C->Items[C->Count-1];
}



const void* CollConstLast (const Collection* C)
/* Return the last item in a collection */
{
    /* We must have at least one entry */
    PRECONDITION (C->Count > 0);

    /* Return the element */
    return C->Items[C->Count-1];
}



void CollDelete (Collection* C, unsigned Index)
/* Remove the item with the given index from the collection. This will not
 * free the item itself, just the pointer. All items with higher indices
 * will get moved to a lower position.
 */
{
    /* Check the index */
    PRECONDITION (Index < C->Count);

    /* Remove the item pointer */
    --C->Count;
    memmove (C->Items+Index, C->Items+Index+1, (C->Count-Index) * sizeof (void*));
}



void CollReplace (Collection* C, void* Item, unsigned Index)
/* Replace the item at the given position. The old item will not be freed,
 * just the pointer will et replaced.
 */
{
    /* Check the index */
    PRECONDITION (Index < C->Count);

    /* Replace the item pointer */
    C->Items[Index] = Item;
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



void CollSort (Collection* C,
	       int (*Compare) (void*, const void*, const void*),
	       void* Data)
/* Sort the collection using the given compare function. The data pointer is
 * passed as *first* element to the compare function, it's not used by the
 * sort function itself. The other two pointer passed to the Compare function
 * are pointers to objects.
 */
{
    if (C->Count > 1) {
	QuickSort (C, 0, C->Count-1, Compare, Data);
    }
}



