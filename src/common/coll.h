/*****************************************************************************/
/*                                                                           */
/*				    coll.h				     */
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



#ifndef COLL_H
#define COLL_H



#include "attrib.h"



/*****************************************************************************/
/*     	      	       		     Data				     */
/*****************************************************************************/



/* An array of pointers that grows if needed */
typedef struct Collection Collection;
struct Collection {
    unsigned   	       	Count;		/* Number of items in the list */
    unsigned   	       	Size;		/* Size of allocated array */
    void**	    	Items;		/* Array with dynamic size */
};



/*****************************************************************************/
/*     	      	       		     Code				     */
/*****************************************************************************/



Collection* InitCollection (Collection* C);
/* Initialize a collection and return it. */

void DoneCollection (Collection* C);
/* Free the data for a collection. This will not free the data contained in
 * the collection.
 */

Collection* NewCollection (void);
/* Create and return a new collection with the given initial size */

void FreeCollection (Collection* C);
/* Free a collection */

unsigned CollCount (Collection* C) attribute ((const));
/* Return the number of items in the collection */

void CollInsert (Collection* C, void* Item, unsigned Index);
/* Insert the data at the given position in the collection */

void CollAppend (Collection* C, void* Item);
/* Append an item to the end of the collection */

void* CollAt (Collection* C, unsigned Index) attribute ((const));
/* Return the item at the given index */

void CollDelete (Collection* C, unsigned Index);
/* Remove the item with the given index from the collection. This will not
 * free the item itself, just the pointer. All items with higher indices
 * will get moved to a lower position.
 */

void CollReplace (Collection* C, void* Item, unsigned Index);
/* Replace the item at the given position. The old item will not be freed,
 * just the pointer will et replaced.
 */



/* End of exprlist.h */

#endif




