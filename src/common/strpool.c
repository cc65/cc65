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
 * stored in the pool has a unique id, which may be used to access the string
 * in the pool. Identical strings are stored only once in the pool and have
 * identical ids. This means that instead of comparing strings, just the
 * string pool ids must be compared.
 */



#include <string.h>

/* common */
#include "coll.h"
#include "hashfunc.h"
#include "strbuf.h"
#include "strpool.h"
#include "xmalloc.h"



/*****************************************************************************/
/*                                     Data                                  */
/*****************************************************************************/



/* A string pool entry */
struct StringPoolEntry {
    StringPoolEntry*    Next;   /* Pointer to next entry in hash chain */
    unsigned            Hash;   /* Full hash value */
    unsigned            Id;     /* The numeric string id */
    StrBuf              Buf;    /* The string itself */
};



/*****************************************************************************/
/*                          struct StringPoolEntry                           */
/*****************************************************************************/



static StringPoolEntry* NewStringPoolEntry (const StrBuf* S, unsigned Hash, unsigned Id)
/* Create a new string pool entry and return it. */
{
    /* Allocate memory */
    StringPoolEntry* E = xmalloc (sizeof (StringPoolEntry));

    /* Initialize the fields */
    E->Next = 0;
    E->Hash = Hash;
    E->Id   = Id;
    SB_Init (&E->Buf);
    SB_Copy (&E->Buf, S);

    /* Always zero terminate the string */
    SB_Terminate (&E->Buf);

    /* Return the new entry */
    return E;
}



/*****************************************************************************/
/*                                     Code                                  */
/*****************************************************************************/



StringPool* InitStringPool (StringPool* P)
/* Initialize a string pool */
{
    unsigned I;

    /* Initialize the fields */
    for (I = 0; I < sizeof (P->Tab) / sizeof (P->Tab[0]); ++I) {
        P->Tab[I] = 0;
    }
    P->Entries = EmptyCollection;
    P->TotalSize = 0;

    /* Return a pointer to the initialized pool */
    return P;
}



void DoneStringPool (StringPool* P)
/* Free the data of a string pool (but not the data itself) */
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

    /* Clear the hash table */
    for (I = 0; I < sizeof (P->Tab) / sizeof (P->Tab[0]); ++I) {
        P->Tab[I] = 0;
    }

    /* Reset the size */
    P->TotalSize = 0;
}



StringPool* NewStringPool (void)
/* Allocate, initialize and return a new string pool */
{
    /* Allocate memory, initialize and return it */
    return InitStringPool (xmalloc (sizeof (StringPool)));
}



void FreeStringPool (StringPool* P)
/* Free a string pool */
{
    /* Free all entries */
    DoneStringPool (P);

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
 * already exist in the pool, SP_AddBuf will just return the index of the
 * existing string.
 */
{
    /* Calculate the string hash */
    unsigned Hash = HashBuf (S);

    /* Calculate the reduced string hash */
    unsigned RHash = Hash % (sizeof (P->Tab)/sizeof (P->Tab[0]));

    /* Search for an existing entry */
    StringPoolEntry* E = P->Tab[RHash];
    while (E) {
        if (E->Hash == Hash && SB_Compare (&E->Buf, S) == 0) {
            /* Found, return the id of the existing string */
            return E->Id;
        }
        E = E->Next;
    }

    /* We didn't find the entry, so create a new one */
    E = NewStringPoolEntry (S, Hash, CollCount (&P->Entries));

    /* Insert the new entry into the entry collection */
    CollAppend (&P->Entries, E);

    /* Insert the new entry into the hash table */
    E->Next = P->Tab[RHash];
    P->Tab[RHash] = E;

    /* Add up the string size */
    P->TotalSize += SB_GetLen (&E->Buf);

    /* Return the id of the entry */
    return E->Id;
}



unsigned SP_AddStr (StringPool* P, const char* S)
/* Add a string to the buffer and return the index. If the string does already
 * exist in the pool, SP_Add will just return the index of the existing string.
 */
{
    unsigned Id;

    /* First make a string buffer, then add it. This is some overhead, but the
     * routine will probably go.
     */
    StrBuf Buf;
    Id = SP_Add (P, SB_InitFromString (&Buf, S));

    /* Return the id of the new entry */
    return Id;
}



