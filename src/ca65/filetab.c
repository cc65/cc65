/*****************************************************************************/
/*                                                                           */
/*				   filetab.h				     */
/*                                                                           */
/*			   Input file table for ca65			     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000     Ullrich von Bassewitz                                        */
/*              Wacholderweg 14                                              */
/*              D-70597 Stuttgart                                            */
/* EMail:       uz@musoftware.de                                             */
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
#include "check.h"
#include "hashstr.h"
#include "xmalloc.h"

/* ca65 */
#include "error.h"
#include "objfile.h"
#include "filetab.h"



/*****************************************************************************/
/*     	       	    		     Data			   	     */
/*****************************************************************************/



/* An entry in the file table */
typedef struct FileEntry FileEntry;
struct FileEntry {
    FileEntry* 	      	Next;		/* Next in hash list */
    unsigned	      	Index;		/* Index of entry */
    unsigned long     	Size;		/* Size of file */
    unsigned long     	MTime;		/* Time of last modification */
    char	      	Name[1];	/* Name, dynamically allocated */
};

/* Array of all entries, listed by index */
static FileEntry**	FileTab   = 0;
static unsigned		FileCount = 0;
static unsigned 	FileMax   = 0;

/* Hash table, hashed by name */
#define HASHTAB_SIZE	31
static FileEntry*	HashTab[HASHTAB_SIZE];



/*****************************************************************************/
/*     	       	    		     Code			   	     */
/*****************************************************************************/



static FileEntry* NewFileEntry (const char* Name, unsigned long Size, unsigned long MTime)
/* Create a new FileEntry, insert it into the tables and return it */
{
    /* Get the length of the name */
    unsigned Len = strlen (Name);

    /* Get the hash over the name */
    unsigned Hash = HashStr (Name) % HASHTAB_SIZE;

    /* Allocate memory for the entry */
    FileEntry* F = xmalloc (sizeof (FileEntry) + Len);

    /* Initialize the fields */
    F->Index  	= FileCount+1;
    F->Size   	= Size;
    F->MTime  	= MTime;
    memcpy (F->Name, Name, Len+1);

    /* Count the entries and grow the file table if needed */
    if (FileCount >= FileMax) {
    	/* We need to grow the table. Create a new one. */
    	unsigned NewFileMax   = (FileMax == 0)? 32 : FileMax * 2;
       	FileEntry** NewFileTab = xmalloc (sizeof (FileEntry*) * NewFileMax);

    	/* Copy the old entries */
    	memcpy (NewFileTab, FileTab, sizeof (FileEntry*) * FileCount);

    	/* Use the new table */
    	xfree (FileTab);
    	FileTab = NewFileTab;
    	FileMax = NewFileMax;
    }

    /* Insert the file into the file table */
    FileTab [FileCount++] = F;

    /* Insert the entry into the hash table */
    F->Next = HashTab[Hash];
    HashTab[Hash] = F;

    /* Return the new entry */
    return F;
}



const char* GetFileName (unsigned Name)
/* Get the name of a file where the name index is known */
{
    PRECONDITION (Name <= FileCount);
    if (Name == 0) {
	/* Name was defined outside any file scope, use the name of the first
	 * file instead. Errors are then reported with a file position of
     	 * line zero in the first file.
	 */
	if (FileCount == 0) {
    	    /* No files defined until now */
       	    return "(outside file scope)";
	} else {
       	    return FileTab [0]->Name;
	}
    } else {
        return FileTab [Name-1]->Name;
    }
}



unsigned GetFileIndex (const char* Name)
/* Return the file index for the given file name. */
{
    /* Get the hash over the name */
    unsigned Hash = HashStr (Name) % HASHTAB_SIZE;

    /* Search the linear hash list */
    FileEntry* F = HashTab[Hash];
    while (F) {
	/* Is it this one? */
	if (strcmp (Name, F->Name) == 0) {
	    /* Found, return the index */
	    return F->Index;
	}
	/* No, check next */
	F = F->Next;
    }

    /* Not found, use main file */
    Error (ERR_FILENAME_NOT_FOUND, Name);
    return 0;
}



unsigned AddFile (const char* Name, unsigned long Size, unsigned long MTime)
/* Add a new file to the list of input files. Return the index of the file in
 * the table.
 */
{
    /* Create a new file entry and insert it into the tables */
    FileEntry* F = NewFileEntry (Name, Size, MTime);

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
    ObjWriteVar (FileCount);

    /* Write the file data */
    for (I = 0; I < FileCount; ++I) {
	/* Get a pointer to the entry */
	FileEntry* F = FileTab[I];
	/* Write the fields */
	ObjWrite32 (F->MTime);
	ObjWrite32 (F->Size);
	ObjWriteStr (F->Name);
    }

    /* Done writing files */
    ObjEndFiles ();
}



