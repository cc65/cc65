/*****************************************************************************/
/*                                                                           */
/*				   filetab.h				     */
/*                                                                           */
/*			   Input file table for ca65			     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2003 Ullrich von Bassewitz                                       */
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



#include <string.h>

/* common */
#include "check.h"
#include "coll.h"
#include "hashtab.h"
#include "xmalloc.h"

/* ca65 */
#include "error.h"
#include "filetab.h"
#include "objfile.h"
#include "spool.h"



/*****************************************************************************/
/*                                 Forwards                                  */
/*****************************************************************************/



static unsigned HT_GenHash (const void* Key);
/* Generate the hash over a key. */

static const void* HT_GetKey (void* Entry);
/* Given a pointer to the user entry data, return a pointer to the key. */

static HashNode* HT_GetHashNode (void* Entry);
/* Given a pointer to the user entry data, return a pointer to the hash node */

static int HT_Compare (const void* Key1, const void* Key2);
/* Compare two keys for equality */



/*****************************************************************************/
/*     	       	    		     Data			   	     */
/*****************************************************************************/



/* Number of entries in the table and the mask to generate the hash */
#define HASHTAB_MASK    0x1F
#define HASHTAB_COUNT   (HASHTAB_MASK + 1)

/* An entry in the file table */
typedef struct FileEntry FileEntry;
struct FileEntry {
    HashNode            Node;
    unsigned            Name;           /* File name */
    unsigned  	      	Index;		/* Index of entry */
    unsigned long     	Size;		/* Size of file */
    unsigned long     	MTime;		/* Time of last modification */
};

/* Array of all entries, listed by index */
static Collection FileTab = STATIC_COLLECTION_INITIALIZER;

/* Hash table functions */
static const HashFunctions HashFunc = {
    HT_GenHash,
    HT_GetKey,
    HT_GetHashNode,
    HT_Compare
};

/* Hash table, hashed by name */
static HashTable HashTab = STATIC_HASHTABLE_INITIALIZER (HASHTAB_COUNT, &HashFunc);



/*****************************************************************************/
/*                           Hash table functions                            */
/*****************************************************************************/



static unsigned HT_GenHash (const void* Key)
/* Generate the hash over a key. */
{
    return (*(const unsigned*)Key & HASHTAB_MASK);
}



static const void* HT_GetKey (void* Entry)
/* Given a pointer to the user entry data, return a pointer to the index */
{
    return &((FileEntry*) Entry)->Name;
}



static HashNode* HT_GetHashNode (void* Entry)
/* Given a pointer to the user entry data, return a pointer to the hash node */
{
    return &((FileEntry*) Entry)->Node;
}



static int HT_Compare (const void* Key1, const void* Key2)
/* Compare two keys for equality */
{
    return (*(const unsigned*)Key1 == *(const unsigned*)Key2);
}



/*****************************************************************************/
/*     	       	    	 	     Code			   	     */
/*****************************************************************************/



static FileEntry* NewFileEntry (unsigned Name, unsigned long Size, unsigned long MTime)
/* Create a new FileEntry, insert it into the tables and return it */
{
    /* Allocate memory for the entry */
    FileEntry* F = xmalloc (sizeof (FileEntry));

    /* Initialize the fields */
    InitHashNode (&F->Node, F);
    F->Name     = Name;
    F->Index  	= CollCount (&FileTab) + 1;     /* First file has index #1 */
    F->Size   	= Size;
    F->MTime  	= MTime;

    /* Insert the file into the file table */
    CollAppend (&FileTab, F);

    /* Insert the entry into the hash table */
    HT_Insert (&HashTab, &F->Node);

    /* Return the new entry */
    return F;
}



const char* GetFileName (unsigned Name)
/* Get the name of a file where the name index is known */
{
    const FileEntry* F;

    if (Name == 0) {
	/* Name was defined outside any file scope, use the name of the first
	 * file instead. Errors are then reported with a file position of
     	 * line zero in the first file.
	 */
	if (CollCount (&FileTab) == 0) {
    	    /* No files defined until now */
       	    return "(outside file scope)";
	} else {
            F = CollConstAt (&FileTab, 0);
	}
    } else {
        F = CollConstAt (&FileTab, Name-1);
    }
    return GetString (F->Name);
}



unsigned GetFileIndex (const char* Name)
/* Return the file index for the given file name. */
{
    /* Get the string pool index from the name */
    unsigned NameIdx = GetStringId (Name);

    /* Search in the hash table for the name */
    FileEntry* F = HT_FindEntry (&HashTab, &NameIdx);

    /* If we don't have this index, print a diagnostic and use the main file */
    if (F == 0) {
        Error (ERR_FILENAME_NOT_FOUND, Name);
        return 0;
    } else {
        return F->Index;
    }
}



unsigned AddFile (const char* Name, unsigned long Size, unsigned long MTime)
/* Add a new file to the list of input files. Return the index of the file in
 * the table.
 */
{
    /* Create a new file entry and insert it into the tables */
    FileEntry* F = NewFileEntry (GetStringId (Name), Size, MTime);

    /* Return the index */
    return F->Index;
}



void WriteFiles (void)
/* Write the list of input files to the object file */
{
    unsigned I;

    /* Tell the obj file module that we're about to start the file list */
    ObjStartFiles ();

    /* Write the file count */
    ObjWriteVar (CollCount (&FileTab));

    /* Write the file data */
    for (I = 0; I < CollCount (&FileTab); ++I) {
	/* Get a pointer to the entry */
	const FileEntry* F = CollConstAt (&FileTab, I);
	/* Write the fields */
	ObjWriteVar (F->Name);
	ObjWrite32 (F->MTime);
	ObjWrite32 (F->Size);
    }

    /* Done writing files */
    ObjEndFiles ();
}



