/*****************************************************************************/
/*                                                                           */
/*                                 hashtab.h                                 */
/*                                                                           */
/*                            Generic hash table                             */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2003      Ullrich von Bassewitz                                       */
/*               Römerstrasse 52                                             */
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



#ifndef HASHTAB_H
#define HASHTAB_H



/* common */
#include "inline.h"
#include "xmalloc.h"



/*****************************************************************************/
/*     	     	    		     Data     				     */
/*****************************************************************************/



/* Hash table node */
typedef struct HashNode HashNode;
struct HashNode {
    HashNode*           Next;           /* Next entry in hash list */
    struct HashTable*   Owner;          /* Owner table */
    unsigned            Hash;           /* The full hash value */
    void*               Entry;          /* Pointer to user entry data */
};

#define STATIC_HASHNODE_INITIALIZER(Entry) { 0, 0, 0, Entry }

/* Hash table functions */
typedef struct HashFunctions HashFunctions;
struct HashFunctions {

    unsigned (*GenHash) (const void* Key);
    /* Generate the hash over a key. */

    const void* (*GetKey) (void* Entry);
    /* Given a pointer to the user entry data, return a pointer to the key */

    HashNode* (*GetHashNode) (void* Entry);
    /* Given a pointer to the user entry data, return a pointer to the hash node */

    int (*Compare) (const void* Key1, const void* Key2);
    /* Compare two keys. The function must return a value less than zero if
     * Key1 is smaller than Key2, zero if both are equal, and a value greater
     * than zero if Key1 is greater then Key2.
     */
};

/* Hash table */
typedef struct HashTable HashTable;
struct HashTable {
    unsigned                    Slots;  /* Number of table slots */
    unsigned                    Count;  /* Number of table entries */
    HashNode**                  Table;  /* Table, dynamically allocated */
    const HashFunctions*        Func;   /* Table functions */
};

#define STATIC_HASHTABLE_INITIALIZER(Slots, Func)   { Slots, 0, 0, Func }



/*****************************************************************************/
/*                              struct HashNode                              */
/*****************************************************************************/



#if defined(HAVE_INLINE)
INLINE void InitHashNode (HashNode* N, void* Entry)
/* Initialize a hash node. */
{
    N->Next     = 0;
    N->Owner    = 0;
    N->Entry    = Entry;
}
#else
#define InitHashNode(N, Entry)  \
    (N)->Next   = 0;            \
    (N)->Owner  = 0;            \
    (N)->Entry  = (Entry)
#endif



/*****************************************************************************/
/*                             struct HashTable                              */
/*****************************************************************************/



#if defined(HAVE_INLINE)
INLINE HashTable* InitHashTable (HashTable* T, unsigned Slots, const HashFunctions* Func)
/* Initialize a hash table and return it */
{
    /* Initialize the fields */
    T->Slots    = Slots;
    T->Count    = 0;
    T->Table    = 0;
    T->Func     = Func;

    /* Return the initialized table */
    return T;
}
#else
#define InitHashTable(T, Slots, Func)   \
    (T)->Slots  = (Slots),              \
    (T)->Count  = 0,                    \
    (T)->Table  = 0,                    \
    (T)->Func   = (Func),               \
    (T)
#endif

#if defined(HAVE_INLINE)
INLINE void DoneHashTable (HashTable* T)
/* Destroy the contents of a hash table. Note: This will not free the entries
 * in the table!
 */
{
    /* Just free the array with the table pointers */
    xfree (T->Table);
}
#else
#define DoneHashTable(T)        xfree ((T)->Table)
#endif

#if defined(HAVE_INLINE)
INLINE HashTable* NewHashTable (unsigned Slots, const HashFunctions* Func)
/* Create a new hash table and return it. */
{
    /* Allocate memory, initialize and return it */
    return InitHashTable (xmalloc (sizeof (HashTable)), Slots, Func);
}
#else
#define NewHashTable(Slots, Func) InitHashTable(xmalloc (sizeof (HashTable)), Slots, Func)
#endif

void FreeHashTable (HashTable* T);
/* Free a hash table. Note: This will not free the entries in the table! */

HashNode* HT_Find (const HashTable* T, const void* Key);
/* Find the node with the given key*/

void* HT_FindEntry (const HashTable* T, const void* Key);
/* Find the node with the given key and return the corresponding entry */

void HT_Insert (HashTable* T, HashNode* N);
/* Insert a node into the given hash table */

void HT_InsertEntry (HashTable* T, void* Entry);
/* Insert an entry into the given hash table */

void HT_Walk (HashTable* T, void (*F) (void* Entry, void* Data), void* Data);
/* Walk over all nodes of a hash table. For each node, the user supplied
 * function F is called, passing a pointer to the entry, and the data pointer
 * passed to HT_Walk by the caller.
 */



/* End of hashtab.h */

#endif



