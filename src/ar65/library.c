/*****************************************************************************/
/*                                                                           */
/*				   library.c				     */
/*                                                                           */
/*	   Library data structures and helpers for the ar65 archiver	     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2011, Ullrich von Bassewitz                                      */
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



#include <stdio.h>
#include <string.h>
#include <errno.h>

/* common */
#include "bitops.h"
#include "exprdefs.h"
#include "filepos.h"
#include "libdefs.h"
#include "print.h"
#include "symdefs.h"
#include "xmalloc.h"

/* ar65 */
#include "error.h"
#include "global.h"
#include "fileio.h"
#include "objdata.h"
#include "exports.h"
#include "library.h"



/*****************************************************************************/
/*     	       	       	       	     Data     				     */
/*****************************************************************************/



/* File descriptor for the library file */
FILE*			NewLib = 0;
static FILE* 		Lib = 0;
static const char*	LibName = 0;

/* The library header */
static LibHeader       	Header = {
    LIB_MAGIC,
    LIB_VERSION,
    0,
    0
};



/*****************************************************************************/
/*			 Writing file data structures			     */
/*****************************************************************************/



static void ReadHeader (void)
/* Read the header of a library file */
{
    /* Seek to position zero */
    fseek (Lib, 0, SEEK_SET);

    /* Read the header fields, checking magic and version */
    Header.Magic   = Read32 (Lib);
    if (Header.Magic != LIB_MAGIC) {
	Error ("`%s' is not a valid library file", LibName);
    }
    Header.Version = Read16 (Lib);
    if (Header.Version != LIB_VERSION) {
	Error ("Wrong data version in `%s'", LibName);
    }
    Header.Flags   = Read16 (Lib);
    Header.IndexOffs = Read32 (Lib);
}



static void ReadIndexEntry (void)
/* Read one entry in the index */
{
    unsigned I;

    /* Create a new entry and insert it into the list */
    ObjData* O 	= NewObjData ();

    /* Module name/flags/MTime/Start/Size */
    O->Name    	= ReadStr (Lib);
    O->Flags   	= Read16 (Lib);
    O->MTime    = Read32 (Lib);
    O->Start    = Read32 (Lib);
    O->Size     = Read32 (Lib);

    /* Strings */
    O->StringCount = ReadVar (Lib);
    O->Strings     = xmalloc (O->StringCount * sizeof (char*));
    for (I = 0; I < O->StringCount; ++I) {
        O->Strings[I] = ReadStr (Lib);
    }

    /* Imports */
    O->ImportSize = ReadVar (Lib);
    O->Imports    = xmalloc (O->ImportSize);
    ReadData (Lib, O->Imports, O->ImportSize);

    /* Exports */
    O->ExportSize = ReadVar (Lib);
    O->Exports    = xmalloc (O->ExportSize);
    ReadData (Lib, O->Exports, O->ExportSize);
}



static void ReadIndex (void)
/* Read the index of a library file */
{
    unsigned Count;

    /* Seek to the start of the index */
    fseek (Lib, Header.IndexOffs, SEEK_SET);

    /* Read the object file count and calculate the cross ref size */
    Count = ReadVar (Lib);

    /* Read all entries in the index */
    while (Count--) {
	ReadIndexEntry ();
    }
}



/*****************************************************************************/
/*			 Writing file data structures			     */
/*****************************************************************************/



static void WriteHeader (void)
/* Write the header to the library file */
{
    /* Seek to position zero */
    fseek (NewLib, 0, SEEK_SET);

    /* Write the header fields */
    Write32 (NewLib, Header.Magic);
    Write16 (NewLib, Header.Version);
    Write16 (NewLib, Header.Flags);
    Write32 (NewLib, Header.IndexOffs);
}



static void WriteIndexEntry (ObjData* O)
/* Write one index entry */
{
    unsigned I;

    /* Module name/flags/MTime/start/size */
    WriteStr (NewLib, O->Name);
    Write16  (NewLib, O->Flags & ~OBJ_HAVEDATA);
    Write32  (NewLib, O->MTime);
    Write32  (NewLib, O->Start);
    Write32  (NewLib, O->Size);

    /* Strings */
    WriteVar (NewLib, O->StringCount);
    for (I = 0; I < O->StringCount; ++I) {
        WriteStr (NewLib, O->Strings[I]);
    }

    /* Imports */
    WriteVar (NewLib, O->ImportSize);
    WriteData (NewLib, O->Imports, O->ImportSize);

    /* Exports */
    WriteVar (NewLib, O->ExportSize);
    WriteData (NewLib, O->Exports, O->ExportSize);
}



static void WriteIndex (void)
/* Write the index of a library file */
{
    ObjData* O;

    /* Sync I/O in case the last operation was a read */
    fseek (NewLib, 0, SEEK_CUR);

    /* Remember the current offset in the header */
    Header.IndexOffs = ftell (NewLib);

    /* Write the object file count */
    WriteVar (NewLib, ObjCount);

    /* Write the object files */
    O = ObjRoot;
    while (O) {
	WriteIndexEntry (O);
       	O = O->Next;
    }
}



/*****************************************************************************/
/*			       High level stuff				     */
/*****************************************************************************/



void LibOpen (const char* Name, int MustExist, int NeedTemp)
/* Open an existing library and a temporary copy. If MustExist is true, the
 * old library is expected to exist. If NeedTemp is true, a temporary library
 * is created.
 */
{
    /* Remember the name */
    LibName = xstrdup (Name);

    /* Open the existing library for reading */
    Lib = fopen (Name, "rb");
    if (Lib == 0) {

       	/* File does not exist */
       	if (MustExist) {
       	    Error ("Library `%s' does not exist", Name);
       	} else {
	    Warning ("Library `%s' not found - will be created", Name);
       	}

    } else {

        /* We have an existing file: Read the header */
	ReadHeader ();

	/* Now read the existing index */
       	ReadIndex ();

    }

    if (NeedTemp) {
	/* Create the temporary library */
	NewLib = tmpfile ();
	if (NewLib == 0) {
	    Error ("Cannot create temporary file: %s", strerror (errno));
	}

	/* Write a dummy header to the temp file */
	WriteHeader ();
    }
}



unsigned long LibCopyTo (FILE* F, unsigned long Bytes)
/* Copy data from F to the temp library file, return the start position in
 * the temporary library file.
 */
{
    unsigned char Buf [4096];

    /* Remember the position */
    unsigned long Pos = ftell (NewLib);

    /* Copy loop */
    while (Bytes) {
    	unsigned Count = (Bytes > sizeof (Buf))? sizeof (Buf) : Bytes;
    	ReadData (F, Buf, Count);
    	WriteData (NewLib, Buf, Count);
    	Bytes -= Count;
    }

    /* Return the start position */
    return Pos;
}



void LibCopyFrom (unsigned long Pos, unsigned long Bytes, FILE* F)
/* Copy data from the library file into another file */
{
    unsigned char Buf [4096];

    /* Seek to the correct position */
    fseek (Lib, Pos, SEEK_SET);

    /* Copy loop */
    while (Bytes) {
    	unsigned Count = (Bytes > sizeof (Buf))? sizeof (Buf) : Bytes;
    	ReadData (Lib, Buf, Count);
    	WriteData (F, Buf, Count);
    	Bytes -= Count;
    }
}



static unsigned long GetVar (unsigned char** Buf)
/* Get a variable sized value from Buf */
{
    unsigned char C;
    unsigned long V = 0;
    unsigned Shift = 0;
    do {
	/* Read one byte */
       	C = **Buf;
	++(*Buf);
	/* Add this char to the value */
	V |= ((unsigned long)(C & 0x7F)) << Shift;
	/* Next value */
	Shift += 7;
    } while (C & 0x80);

    /* Return the result */
    return V;
}



static void SkipExpr (unsigned char** Buf)
/* Skip an expression in Buf */
{
    /* Get the operation and skip it */
    unsigned char Op = **Buf;
    ++(*Buf);

    /* Filter leaf nodes */
    switch (Op) {

      	case EXPR_NULL:
      	    return;

        case EXPR_LITERAL:
      	    /* 32 bit literal value */
      	    *Buf += 4;
      	    return;

        case EXPR_SYMBOL:
      	    /* Variable seized symbol index */
      	    (void) GetVar (Buf);
      	    return;

        case EXPR_SECTION:
      	    /* 8 bit segment number */
      	    *Buf += 1;
      	    return;
    }

    /* What's left are unary and binary nodes */
    SkipExpr (Buf);    	       	/* Skip left */
    SkipExpr (Buf); 	       	/* Skip right */
}



static void SkipLineInfoList (unsigned char** Buf)
/* Skip a list of line infos in Buf */
{
    /* Number of indices preceeds the list */
    unsigned long Count = GetVar (Buf);

    /* Skip indices */
    while (Count--) {
        (void) GetVar (Buf);
    }
}



static void LibCheckExports (ObjData* O)
/* Insert all exports from the given object file into the global list
 * checking for duplicates.
 */
{
    /* Get a pointer to the buffer */
    unsigned char* Exports = O->Exports;

    /* Get the export count */
    unsigned Count = GetVar (&Exports);

    /* Read the exports */
    Print (stdout, 1, "Module `%s' (%u exports):\n", O->Name, Count);
    while (Count--) {

	const char*     Name;

      	/* Get the export tag and skip the address size */
       	unsigned Type = GetVar (&Exports);
        ++Exports;

	/* condes decls may follow */
	Exports += SYM_GET_CONDES_COUNT (Type);

       	/* Next thing is index of name of symbol */
        Name = GetObjString (O, GetVar (&Exports));

     	/* Skip value of symbol */
     	if (SYM_IS_EXPR (Type)) {
     	    /* Expression tree */
     	    SkipExpr (&Exports);
     	} else {
     	    /* Constant 32 bit value */
     	    Exports += 4;
     	}

     	/* Skip the line info */
       	SkipLineInfoList (&Exports);

      	/* Insert the name into the hash table */
	Print (stdout, 1, "  %s\n", Name);
     	ExpInsert (Name, O->Index);
    }
}



void LibClose (void)
/* Write remaining data, close both files and copy the temp file to the old
 * filename
 */
{
    /* Do we have a temporary library? */
    if (NewLib) {

	unsigned I;
	unsigned char Buf [4096];
	size_t Count;

	/* Index the object files and make an array containing the objects */
	MakeObjPool ();

        /* Walk through the object file list, inserting exports into the
	 * export list checking for duplicates. Copy any data that is still
	 * in the old library into the new one.
	 */
	for (I = 0; I < ObjCount; ++I) {

	    /* Get a pointer to the object */
	    ObjData* O = ObjPool [I];

	    /* Check exports, make global export table */
	    LibCheckExports (O);

	    /* Copy data if needed */
	    if ((O->Flags & OBJ_HAVEDATA) == 0) {
	 	/* Data is still in the old library */
	 	fseek (Lib, O->Start, SEEK_SET);
	 	O->Start = ftell (NewLib);
	 	LibCopyTo (Lib, O->Size);
	 	O->Flags |= OBJ_HAVEDATA;
	    }
	}

	/* Write the index */
	WriteIndex ();

	/* Write the updated header */
	WriteHeader ();

	/* Close the file */
	if (Lib && fclose (Lib) != 0) {
	    Error ("Error closing library: %s", strerror (errno));
	}

	/* Reopen the library and truncate it */
	Lib = fopen (LibName, "wb");
	if (Lib == 0) {
	    Error ("Cannot open library `%s' for writing: %s",
		   LibName, strerror (errno));
	}

	/* Copy the new library to the new one */
	fseek (NewLib, 0, SEEK_SET);
	while ((Count = fread (Buf, 1, sizeof (Buf), NewLib)) != 0) {
	    if (fwrite (Buf, 1, Count, Lib) != Count) {
		Error ("Cannot write to `%s': %s", LibName, strerror (errno));
	    }
	}
    }

    /* Close both files */
    if (Lib && fclose (Lib) != 0) {
     	Error ("Problem closing `%s': %s", LibName, strerror (errno));
    }
    if (NewLib && fclose (NewLib) != 0) {
     	Error ("Problem closing temporary library file: %s", strerror (errno));
    }
}



