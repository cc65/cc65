/*****************************************************************************/
/*                                                                           */
/*                                 hashtab.c                                 */
/*                                                                           */
/*                             Generic hash table                            */
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



/* common */
#include "hashtab.h"
#include "xmalloc.h"



/*****************************************************************************/
/*                             struct HashTable                              */
/*****************************************************************************/



void FreeHashTable (HashTable* T)
/* Free a hash table. Note: This will not free the entries in the table! */
{
    if (T) {
        /* Free the contents */
        DoneHashTable (T);
        /* Free the table structure itself */
        xfree (T);
    }
}



static void HT_Alloc (HashTable* T)
/* Allocate table memory */
{
    unsigned I;

    /* Allocate memory */
    T->Table = xmalloc (T->Slots * sizeof (T->Table[0]));

    /* Initialize the table */
    for (I = 0; I < T->Slots; ++I) {
        T->Table[I] = 0;
    }
}



HashNode* HT_Find (const HashTable* T, const void* Key)
/* Find the node with the given index */
{
    unsigned  Hash;
    HashNode* N;

    /* If we don't have a table, there's nothing to find */
    if (T->Table == 0) {
        return 0;
    }

    /* Generate the hash over the index */
    Hash = T->Func->GenHash (Key);

    /* Search for the entry in the given chain */
    N = T->Table[Hash % T->Slots];
    while (N) {

        /* First compare the full hash, to avoid calling the compare function
         * if it is not really necessary.
         */
        if (N->Hash == Hash &&
            T->Func->Compare (Key, T->Func->GetIndex (N->Entry))) {
            /* Found */
            break;
        }

        /* Not found, next entry */
        N = N->Next;
    }

    /* Return what we found */
    return N;
}



void* HT_FindEntry (const HashTable* T, const void* Key)
/* Find the node with the given index and return the corresponding entry */
{
    /* First, search for the hash node */
    HashNode* N = HT_Find (T, Key);

    /* Convert the node into an entry if necessary */
    return N? N->Entry : 0;
}



void HT_Insert (HashTable* T, HashNode* N)
/* Insert a node into the given hash table */
{
    unsigned RHash;

    /* If we don't have a table, we need to allocate it now */
    if (T->Table == 0) {
        HT_Alloc (T);
    }

    /* Generate the hash for the node contents */
    N->Hash = T->Func->GenHash (T->Func->GetIndex (N->Entry));

    /* Calculate the reduced hash */
    RHash = N->Hash % T->Slots;

    /* Insert the entry into the correct chain */
    N->Next = T->Table[RHash];
    T->Table[RHash] = N;

    /* One more entry */
    ++T->Count;
}



void HT_InsertEntry (HashTable* T, void* Entry)
/* Insert an entry into the given hash table */
{
    HT_Insert (T, T->Func->GetHashNode (Entry));
}



void HT_Walk (HashTable* T, void (*F) (void* Entry, void* Data), void* Data)
/* Walk over all nodes of a hash table. For each node, the user supplied
 * function F is called, passing a pointer to the entry, and the data pointer
 * passed to HT_Walk by the caller.
 */
{
    unsigned I;

    /* If we don't have a table there are no entries to walk over */
    if (T->Table == 0) {
        return;
    }

    /* Walk over all chains */
    for (I = 0; I < T->Slots; ++I) {

        /* Get the pointer to the first entry of the hash chain */
        HashNode* N = T->Table[I];

        /* Walk over all entries in this chain */
        while (N) {
            /* Call the user function */
            F (N->Entry, Data);
            /* Next node in chain */
            N = N->Next;
        }

    }
}



