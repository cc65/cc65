/*****************************************************************************/
/*                                                                           */
/*                                 hashtab.h                                 */
/*                                                                           */
/*                            Generic hash table                             */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2003-2011, Ullrich von Bassewitz                                      */
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



#ifndef HASHTAB_H
#define HASHTAB_H



/* common */
#include "inline.h"
#include "xmalloc.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Hash table node. NOTE: This structure must be the first member of a struct
** that is hashed by the module. Having it first allows to omit a pointer to
** the entry itself, because the C standard guarantees that a pointer to a
** struct can be converted to its first member.
*/
typedef struct HashNode HashNode;
struct HashNode {
    HashNode*           Next;           /* Next entry in hash list */
    unsigned            Hash;           /* The full hash value */
};

#define STATIC_HASHNODE_INITIALIZER     { 0, 0, 0 }

/* Hash table functions */
typedef struct HashFunctions HashFunctions;
struct HashFunctions {

    unsigned (*GenHash) (const void* Key);
    /* Generate the hash over a key. */

    const void* (*GetKey) (const void* Entry);
    /* Given a pointer to the user entry data, return a pointer to the key */

    int (*Compare) (const void* Key1, const void* Key2);
    /* Compare two keys. The function must return a value less than zero if
    ** Key1 is smaller than Key2, zero if both are equal, and a value greater
    ** than zero if Key1 is greater then Key2.
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
INLINE void InitHashNode (HashNode* N)
/* Initialize a hash node. */
{
    N->Next     = 0;
}
#else
#define InitHashNode(N)         do { (N)->Next   = 0; } while (0)
#endif



/*****************************************************************************/
/*                             struct HashTable                              */
/*****************************************************************************/



HashTable* InitHashTable (HashTable* T, unsigned Slots, const HashFunctions* Func);
/* Initialize a hash table and return it */

void DoneHashTable (HashTable* T);
/* Destroy the contents of a hash table. Note: This will not free the entries
** in the table!
*/

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

#if defined(HAVE_INLINE)
INLINE unsigned HT_GetCount (const HashTable* T)
/* Return the number of items in the table. */
{
    return T->Count;
}
#else
#define HT_GetCount(T)  ((T)->Count)
#endif

HashNode* HT_FindHash (const HashTable* T, const void* Key, unsigned Hash);
/* Find the node with the given key. Differs from HT_Find in that the hash
** for the key is precalculated and passed to the function.
*/

void* HT_Find (const HashTable* T, const void* Key);
/* Find the entry with the given key and return it */

void HT_Insert (HashTable* T, void* Entry);
/* Insert an entry into the given hash table */

void HT_Remove (HashTable* T, void* Entry);
/* Remove an entry from the given hash table */

void HT_Walk (HashTable* T, int (*F) (void* Entry, void* Data), void* Data);
/* Walk over all nodes of a hash table, optionally deleting entries from the
** table. For each node, the user supplied function F is called, passing a
** pointer to the entry, and the data pointer passed to HT_Walk by the caller.
** If F returns true, the node is deleted from the hash table otherwise it's
** left in place. While deleting the node, the node is not accessed, so it is
** safe for F to free the memory associcated with the entry.
*/



/* End of hashtab.h */

#endif
