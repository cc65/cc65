/*****************************************************************************/
/*                                                                           */
/*                                 strpool.c                                 */
/*                                                                           */
/*                               A string pool                               */
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



/* A string pool is used to store identifiers and other strings. Each string
** stored in the pool has a unique id, which may be used to access the string
** in the pool. Identical strings are stored only once in the pool and have
** identical ids. This means that instead of comparing strings, just the
** string pool ids must be compared.
*/



#include <string.h>

/* common */
#include "coll.h"
#include "hashfunc.h"
#include "hashtab.h"
#include "strbuf.h"
#include "strpool.h"
#include "xmalloc.h"



/*****************************************************************************/
/*                                 Forwards                                  */
/*****************************************************************************/



static unsigned HT_GenHash (const void* Key);
/* Generate the hash over a key. */

static const void* HT_GetKey (const void* Entry);
/* Given a pointer to the user entry data, return a pointer to the key */

static int HT_Compare (const void* Key1, const void* Key2);
/* Compare two keys. The function must return a value less than zero if
** Key1 is smaller than Key2, zero if both are equal, and a value greater
** than zero if Key1 is greater then Key2.
*/



/*****************************************************************************/
/*                                     Data                                  */
/*****************************************************************************/



/* A string pool entry */
struct StringPoolEntry {
    HashNode            Node;   /* Node for the hash table */
    unsigned            Id;     /* The numeric string id */
    StrBuf              Buf;    /* The string itself */
};

/* A string pool */
struct StringPool {
    Collection          Entries;        /* Entries sorted by number */
    unsigned            TotalSize;      /* Total size of all string data */
    HashTable           Tab;            /* Hash table */
};

/* Hash table functions */
static const HashFunctions HashFunc = {
    HT_GenHash,
    HT_GetKey,
    HT_Compare
};



/*****************************************************************************/
/*                          struct StringPoolEntry                           */
/*****************************************************************************/



static StringPoolEntry* NewStringPoolEntry (const StrBuf* S, unsigned Id)
/* Create a new string pool entry and return it. */
{
    /* Allocate memory */
    StringPoolEntry* E = xmalloc (sizeof (StringPoolEntry));

    /* Initialize the fields */
    InitHashNode (&E->Node);
    E->Id   = Id;
    SB_Init (&E->Buf);
    SB_Copy (&E->Buf, S);

    /* Always zero terminate the string */
    SB_Terminate (&E->Buf);

    /* Return the new entry */
    return E;
}



/*****************************************************************************/
/*                           Hash table functions                            */
/*****************************************************************************/



static unsigned HT_GenHash (const void* Key)
/* Generate the hash over a key. */
{
    return HashBuf (Key);
}



static const void* HT_GetKey (const void* Entry)
/* Given a pointer to the user entry data, return a pointer to the index */
{
    return &((const StringPoolEntry*) Entry)->Buf;
}



static int HT_Compare (const void* Key1, const void* Key2)
/* Compare two keys. The function must return a value less than zero if
** Key1 is smaller than Key2, zero if both are equal, and a value greater
** than zero if Key1 is greater then Key2.
*/
{
    return SB_Compare (Key1, Key2);
}



/*****************************************************************************/
/*                                     Code                                  */
/*****************************************************************************/



StringPool* NewStringPool (unsigned HashSlots)
/* Allocate, initialize and return a new string pool */
{
    /* Allocate memory */
    StringPool* P = xmalloc (sizeof (*P));

    /* Initialize the fields */
    P->Entries   = EmptyCollection;
    P->TotalSize = 0;
    InitHashTable (&P->Tab, HashSlots, &HashFunc);

    /* Return a pointer to the new pool */
    return P;
}



void FreeStringPool (StringPool* P)
/* Free a string pool */
{
    unsigned I;

    /* Free all entries and clear the entry collection */
    for (I = 0; I < CollCount (&P->Entries); ++I) {

        /* Get a pointer to the entry */
        StringPoolEntry* E = CollAtUnchecked (&P->Entries, I);

        /* Free string buffer memory */
        SB_Done (&E->Buf);

        /* Free the memory for the entry itself */
        xfree (E);
    }
    CollDeleteAll (&P->Entries);

    /* Free the hash table */
    DoneHashTable (&P->Tab);

    /* Free the string pool itself */
    xfree (P);
}



const StrBuf* SP_Get (const StringPool* P, unsigned Index)
/* Return a string from the pool. Index must exist, otherwise FAIL is called. */
{
    /* Get the collection entry */
    const StringPoolEntry* E = CollConstAt (&P->Entries, Index);

    /* Return the string from the entry */
    return &E->Buf;
}



unsigned SP_Add (StringPool* P, const StrBuf* S)
/* Add a string buffer to the buffer and return the index. If the string does
** already exist in the pool, SP_AddBuf will just return the index of the
** existing string.
*/
{
    /* Search for a matching entry in the hash table */
    StringPoolEntry* E = HT_Find (&P->Tab, S);

    /* Did we find it? */
    if (E == 0) {

        /* We didn't find the entry, so create a new one */
        E = NewStringPoolEntry (S, CollCount (&P->Entries));

        /* Insert the new entry into the entries collection */
        CollAppend (&P->Entries, E);

        /* Insert the new entry into the hash table */
        HT_Insert (&P->Tab, E);

        /* Add up the string size */
        P->TotalSize += SB_GetLen (&E->Buf);
    }

    /* Return the id of the entry */
    return E->Id;
}



unsigned SP_AddStr (StringPool* P, const char* S)
/* Add a string to the buffer and return the index. If the string does already
** exist in the pool, SP_Add will just return the index of the existing string.
*/
{
    unsigned Id;

    /* First make a string buffer, then add it. This is some overhead, but the
    ** routine will probably go.
    */
    StrBuf Buf;
    Id = SP_Add (P, SB_InitFromString (&Buf, S));

    /* Return the id of the new entry */
    return Id;
}



unsigned SP_GetCount (const StringPool* P)
/* Return the number of strings in the pool */
{
    return CollCount (&P->Entries);
}
