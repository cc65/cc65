/*****************************************************************************/
/*                                                                           */
/*                                 strpool.c                                 */
/*                                                                           */
/*                               A string pool                               */
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



/* A string pool is used to store identifiers and other strings. Each string
 * stored in the pool has a unique id, which may be used to access the string
 * in the pool. Identical strings are only stored once in the pool and have
 * identical ids. This means that instead of comparing strings, just the
 * string pool ids must be compared.
 */



#include <string.h>

/* common */
#include "coll.h"
#include "hashstr.h"
#include "strbuf.h"
#include "strpool.h"
#include "xmalloc.h"



/*****************************************************************************/
/*                                     Data                                  */
/*****************************************************************************/



/* A string pool entry */
struct StrPoolEntry {
    StrPoolEntry*       Next;   /* Pointer to next entry in hash chain */
    unsigned            Hash;   /* Full hash value */
    unsigned            Id;     /* The numeric string id */
    unsigned            Len;    /* Length of the string (excluding terminator) */
    char                S[1];   /* The string itself */
};



/*****************************************************************************/
/*                            struct StrPoolEntry                            */
/*****************************************************************************/



static StrPoolEntry* NewStrPoolEntry (const char* S, unsigned Hash, unsigned Id)
/* Create a new string pool entry and return it. */
{
    /* Get the length of the string */
    unsigned Len = strlen (S);

    /* Allocate memory */
    StrPoolEntry* E = xmalloc (sizeof (StrPoolEntry) + Len);

    /* Initialize the fields */
    E->Next = 0;
    E->Hash = Hash;
    E->Id   = Id;
    E->Len  = Len;
    memcpy (E->S, S, Len+1);

    /* Return the new entry */
    return E;
}



/*****************************************************************************/
/*                                     Code                                  */
/*****************************************************************************/



StrPool* InitStrPool (StrPool* P)
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



void DoneStrPool (StrPool* P)
/* Free the data of a string pool (but not the data itself) */
{
    unsigned I;

    /* Free all entries and clear the entry collection */
    for (I = 0; I < CollCount (&P->Entries); ++I) {
        xfree (CollAtUnchecked (&P->Entries, I));
    }
    CollDeleteAll (&P->Entries);

    /* Clear the hash table */
    for (I = 0; I < sizeof (P->Tab) / sizeof (P->Tab[0]); ++I) {
        P->Tab[I] = 0;
    }

    /* Reset the size */
    P->TotalSize = 0;
}



StrPool* NewStrPool (void)
/* Allocate, initialize and return a new string pool */
{
    /* Allocate memory, initialize and return it */
    return InitStrPool (xmalloc (sizeof (StrPool)));
}



void FreeStrPool (StrPool* P)
/* Free a string pool */
{
    /* Free all entries */
    DoneStrPool (P);

    /* Free the string pool itself */
    xfree (P);
}



const char* SP_Get (const StrPool* P, unsigned Index)
/* Return a string from the pool. Index must exist, otherwise FAIL is called. */
{
    /* Get the collection entry */
    const StrPoolEntry* E = CollConstAt (&P->Entries, Index);

    /* Return the string from the entry */
    return E->S;
}



unsigned SP_Add (StrPool* P, const char* S)
/* Add a string to the buffer and return the index. If the string does already
 * exist in the pool, SP_Add will just return the index of the existing string.
 */
{
    /* Calculate the string hash */
    unsigned Hash = HashStr (S);

    /* Calculate the reduced string hash */
    unsigned RHash = Hash % (sizeof (P->Tab)/sizeof (P->Tab[0]));

    /* Search for an existing entry */
    StrPoolEntry* E = P->Tab[RHash];
    while (E) {
        if (E->Hash == Hash && strcmp (E->S, S) == 0) {
            /* Found, return the id of the existing string */
            return E->Id;
        }
        E = E->Next;
    }

    /* We didn't find the entry, so create a new one */
    E = NewStrPoolEntry (S, Hash, CollCount (&P->Entries));

    /* Insert the new entry into the entry collection */
    CollAppend (&P->Entries, E);

    /* Insert the new entry into the hash table */
    E->Next = P->Tab[RHash];
    P->Tab[RHash] = E;

    /* Add up the string size (plus terminator) */
    P->TotalSize += E->Len + 1;

    /* Return the id of the entry */
    return E->Id;
}



unsigned SP_AddBuf (StrPool* P, const void* Buffer, unsigned Size)
/* Add strings from a string buffer. Buffer must contain a list of zero
 * terminated strings. These strings are added to the pool, starting with
 * the current index. The number of strings added is returned.
 * Beware: The function will do only loose range checking for the buffer
 * limits, so a SEGV may occur if the last string in the buffer is not
 * correctly terminated.
 */
{
    /* Cast the buffer pointer to something useful */
    const char* Buf = Buffer;

    /* Remember the current number of strings in the buffer. */
    unsigned OldCount = SB_GetCount (P);

    /* Add all strings from the buffer */
    while (Size) {

        /* Add the next entry */
        unsigned Id = SP_Add (P, Buf);

        /* Get the entry from the id */
        const StrPoolEntry* E = CollConstAt (&P->Entries, Id);

        /* Skip this string */
        Buf  += E->Len + 1;
        Size -= E->Len + 1;
    }

    /* Return the number of strings added */
    return SB_GetCount (P) - OldCount;
}



void SP_Build (StrPool* P, const void* Buffer, unsigned Size)
/* Delete existing data and use the data from Buffer instead. */
{
    /* Delete old data */
    DoneStrPool (P);

    /* Add the buffer data */
    SP_AddBuf (P, Buffer, Size);
}



