/*****************************************************************************/
/*                                                                           */
/*                                 filetab.h                                 */
/*                                                                           */
/*                         Input file table for ca65                         */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2008 Ullrich von Bassewitz                                       */
/*               Roemerstrasse 52                                            */
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



#include <stdio.h>
#include <string.h>
#include <errno.h>

/* common */
#include "check.h"
#include "coll.h"
#include "hashtab.h"
#include "xmalloc.h"

/* ca65 */
#include "error.h"
#include "filetab.h"
#include "global.h"
#include "objfile.h"
#include "spool.h"



/*****************************************************************************/
/*                                 Forwards                                  */
/*****************************************************************************/



static unsigned HT_GenHash (const void* Key);
/* Generate the hash over a key. */

static const void* HT_GetKey (const void* Entry);
/* Given a pointer to the user entry data, return a pointer to the key. */

static int HT_Compare (const void* Key1, const void* Key2);
/* Compare two keys. The function must return a value less than zero if
** Key1 is smaller than Key2, zero if both are equal, and a value greater
** than zero if Key1 is greater then Key2.
*/



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Number of entries in the table and the mask to generate the hash */
#define HASHTAB_MASK    0x1F
#define HASHTAB_COUNT   (HASHTAB_MASK + 1)

/* An entry in the file table */
typedef struct FileEntry FileEntry;
struct FileEntry {
    HashNode            Node;
    unsigned            Name;           /* File name */
    unsigned            Index;          /* Index of entry */
    FileType            Type;           /* Type of file */
    unsigned long       Size;           /* Size of file */
    unsigned long       MTime;          /* Time of last modification */
};

/* Array of all entries, listed by index */
static Collection FileTab = STATIC_COLLECTION_INITIALIZER;

/* Hash table functions */
static const HashFunctions HashFunc = {
    HT_GenHash,
    HT_GetKey,
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



static const void* HT_GetKey (const void* Entry)
/* Given a pointer to the user entry data, return a pointer to the index */
{
    return &((FileEntry*) Entry)->Name;
}



static int HT_Compare (const void* Key1, const void* Key2)
/* Compare two keys. The function must return a value less than zero if
** Key1 is smaller than Key2, zero if both are equal, and a value greater
** than zero if Key1 is greater then Key2.
*/
{
    return (int)*(const unsigned*)Key1 - (int)*(const unsigned*)Key2;
}



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



static FileEntry* NewFileEntry (unsigned Name, FileType Type,
                                unsigned long Size, unsigned long MTime)
/* Create a new FileEntry, insert it into the tables and return it */
{
    /* Allocate memory for the entry */
    FileEntry* F = xmalloc (sizeof (FileEntry));

    /* Initialize the fields */
    InitHashNode (&F->Node);
    F->Name     = Name;
    F->Index    = CollCount (&FileTab) + 1;     /* First file has index #1 */
    F->Type     = Type;
    F->Size     = Size;
    F->MTime    = MTime;

    /* Insert the file into the file table */
    CollAppend (&FileTab, F);

    /* Insert the entry into the hash table */
    HT_Insert (&HashTab, F);

    /* Return the new entry */
    return F;
}



const StrBuf* GetFileName (unsigned Name)
/* Get the name of a file where the name index is known */
{
    static const StrBuf ErrorMsg = LIT_STRBUF_INITIALIZER ("(outside file scope)");

    const FileEntry* F;

    if (Name == 0) {
        /* Name was defined outside any file scope, use the name of the first
        ** file instead. Errors are then reported with a file position of
        ** line zero in the first file.
        */
        if (CollCount (&FileTab) == 0) {
            /* No files defined until now */
            return &ErrorMsg;
        } else {
            F = CollConstAt (&FileTab, 0);
        }
    } else {
        F = CollConstAt (&FileTab, Name-1);
    }
    return GetStrBuf (F->Name);
}



unsigned GetFileIndex (const StrBuf* Name)
/* Return the file index for the given file name. */
{
    /* Get the string pool index from the name */
    unsigned NameIdx = GetStrBufId (Name);

    /* Search in the hash table for the name */
    const FileEntry* F = HT_Find (&HashTab, &NameIdx);

    /* If we don't have this index, print a diagnostic and use the main file */
    if (F == 0) {
        Error ("File name `%m%p' not found in file table", Name);
        return 0;
    } else {
        return F->Index;
    }
}



unsigned AddFile (const StrBuf* Name, FileType Type,
                  unsigned long Size, unsigned long MTime)
/* Add a new file to the list of input files. Return the index of the file in
** the table.
*/
{
    /* Create a new file entry and insert it into the tables */
    FileEntry* F = NewFileEntry (GetStrBufId (Name), Type, Size, MTime);

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
        ObjWriteVar (F->Size);
    }

    /* Done writing files */
    ObjEndFiles ();
}



static void WriteEscaped (FILE* F, const char* Name)
/* Write a file name to a dependency file escaping spaces */
{
    while (*Name) {
        if (*Name == ' ') {
            /* Escape spaces */
            fputc ('\\', F);
        }
        fputc (*Name, F);
        ++Name;
    }
}



static void WriteDep (FILE* F, FileType Types)
/* Helper function. Writes all file names that match Types to the output */
{
    unsigned I;

    /* Loop over all files */
    for (I = 0; I < CollCount (&FileTab); ++I) {

        const StrBuf* Filename;

        /* Get the next input file */
        const FileEntry* E = (const FileEntry*) CollAt (&FileTab, I);

        /* Ignore it if it is not of the correct type */
        if ((E->Type & Types) == 0) {
            continue;
        }

        /* If this is not the first file, add a space */
        if (I > 0) {
            fputc (' ', F);
        }

        /* Print the dependency escaping spaces */
        Filename = GetStrBuf (E->Name);
        WriteEscaped (F, SB_GetConstBuf (Filename));
    }
}



static void CreateDepFile (const char* Name, FileType Types)
/* Create a dependency file with the given name and place dependencies for
** all files with the given types there.
*/
{
    /* Open the file */
    FILE* F = fopen (Name, "w");
    if (F == 0) {
        Fatal ("Cannot open dependency file `%s': %s", Name, strerror (errno));
    }

    /* Print the output file followed by a tab char */
    WriteEscaped (F, OutFile);
    fputs (":\t", F);

    /* Write out the dependencies for the output file */
    WriteDep (F, Types);
    fputs ("\n\n", F);

    /* Write out a phony dependency for the included files */
    WriteDep (F, Types);
    fputs (":\n\n", F);

    /* Close the file, check for errors */
    if (fclose (F) != 0) {
        remove (Name);
        Fatal ("Cannot write to dependeny file (disk full?)");
    }
}



void CreateDependencies (void)
/* Create dependency files requested by the user */
{
    if (SB_NotEmpty (&DepName)) {
        CreateDepFile (SB_GetConstBuf (&DepName),
                       FT_MAIN | FT_INCLUDE | FT_BINARY);
    }
    if (SB_NotEmpty (&FullDepName)) {
        CreateDepFile (SB_GetConstBuf (&FullDepName),
                       FT_MAIN | FT_INCLUDE | FT_BINARY | FT_DBGINFO);
    }
}
