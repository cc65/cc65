/*****************************************************************************/
/*                                                                           */
/*				   objfile.c				     */
/*                                                                           */
/*	   Object file writing routines for the ca65 macroassembler	     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998     Ullrich von Bassewitz                                        */
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



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "../common/fname.h"
#include "../common/objdefs.h"

#include "global.h"
#include "error.h"
#include "objfile.h"



/*****************************************************************************/
/*     	      	    	  	     Data				     */
/*****************************************************************************/



/* File descriptor */
static FILE* F = 0;

/* Default extension */
#define	OBJ_EXT	".o"

/* Header structure */
static ObjHeader Header = {
    OBJ_MAGIC,
    OBJ_VERSION
};



/*****************************************************************************/
/*			   Internally used functions   			     */
/*****************************************************************************/



static void ObjWriteError (void)
/* Called on a write error. Will try to close and remove the file, then
 * print a fatal error.
 */
{
    /* Remember the error */
    int Error = errno;

    /* Force a close of the file, ignoring errors */
    fclose (F);

    /* Try to remove the file, also ignoring errors */
    remove (OutFile);

    /* Now abort with a fatal error */
    Fatal (FAT_CANNOT_WRITE_OUTPUT, OutFile, strerror (Error));
}



static void ObjWriteHeader (void)
/* Write the object file header to the current file position */
{
    ObjWrite32 (Header.Magic);
    ObjWrite16 (Header.Version);
    ObjWrite16 (Header.Flags);
    ObjWrite32 (Header.OptionOffs);
    ObjWrite32 (Header.OptionSize);
    ObjWrite32 (Header.FileOffs);
    ObjWrite32 (Header.FileSize);
    ObjWrite32 (Header.SegOffs);
    ObjWrite32 (Header.SegSize);
    ObjWrite32 (Header.ImportOffs);
    ObjWrite32 (Header.ImportSize);
    ObjWrite32 (Header.ExportOffs);
    ObjWrite32 (Header.ExportSize);
    ObjWrite32 (Header.DbgSymOffs);
    ObjWrite32 (Header.DbgSymSize);
}



/*****************************************************************************/
/*     	      	      	      	     Code				     */
/*****************************************************************************/



void ObjOpen (void)
/* Open the object file for writing, write a dummy header */
{
    /* Do we have a name for the output file? */
    if (OutFile == 0) {
     	/* We don't have an output name explicitly given, construct one from
     	 * the name of the input file.
     	 */
	OutFile = MakeFilename (InFile, OBJ_EXT);
    }

    /* Create the output file */
    F = fopen (OutFile, "w+b");
    if (F == 0) {
 	Fatal (FAT_CANNOT_OPEN_OUTPUT, OutFile, strerror (errno));
    }

    /* Write a dummy header */
    ObjWriteHeader ();
}



void ObjClose (void)
/* Write an update header and close the object file. */
{
    /* Go back to the beginning */
    if (fseek (F, 0, SEEK_SET) != 0) {
	ObjWriteError ();
    }

    /* If we have debug infos, set the flag in the header */
    if (DbgSyms) {
        Header.Flags |= OBJ_FLAGS_DBGINFO;
    }

    /* Write the updated header */
    ObjWriteHeader ();

    /* Close the file */
    if (fclose (F) != 0) {
	ObjWriteError ();
    }
}



void ObjWrite8 (unsigned V)
/* Write an 8 bit value to the file */
{
    if (putc (V, F) == EOF) {
	ObjWriteError ();
    }
}



void ObjWrite16 (unsigned V)
/* Write a 16 bit value to the file */
{
    ObjWrite8 (V);
    ObjWrite8 (V >> 8);
}



void ObjWrite24 (unsigned long V)
/* Write a 24 bit value to the file */
{
    ObjWrite8 (V);
    ObjWrite8 (V >> 8);
    ObjWrite8 (V >> 16);
}



void ObjWrite32 (unsigned long V)
/* Write a 32 bit value to the file */
{
    ObjWrite8 (V);
    ObjWrite8 (V >> 8);
    ObjWrite8 (V >> 16);
    ObjWrite8 (V >> 24);
}



void ObjWriteStr (const char* S)
/* Write a string to the object file */
{
    unsigned Len = strlen (S);
    if (Len > 255) {
	Internal ("String too long in ObjWriteStr");
    }

    /* Write the string with a length byte preceeded (this is easier for
     * the reading routine than the C format since the length is known in
     * advance).
     */
    ObjWrite8 ((unsigned char) Len);
    ObjWriteData (S, Len);
}



void ObjWriteData (const void* Data, unsigned Size)
/* Write literal data to the file */
{
    if (fwrite (Data, 1, Size, F) != Size) {
     	ObjWriteError ();
    }
}



void ObjWritePos (const FilePos* Pos)
/* Write a file position to the object file */
{
    /* Write the line number as 24 bit value to save one byte */
    ObjWrite24 (Pos->Line);
    ObjWrite8  (Pos->Col);
    if (Pos->Name == 0) {
	/* Position is outside file scope, use the main file instead */
	ObjWrite8 (0);
    } else {
        ObjWrite8  (Pos->Name - 1);
    }
}



void ObjStartOptions (void)
/* Mark the start of the option section */
{
    Header.OptionOffs = ftell (F);
}



void ObjEndOptions (void)
/* Mark the end of the option section */
{
    Header.OptionSize = ftell (F) - Header.OptionOffs;
}



void ObjStartFiles (void)
/* Mark the start of the files section */
{
    Header.FileOffs = ftell (F);
}



void ObjEndFiles (void)
/* Mark the end of the files section */
{
    Header.FileSize = ftell (F) - Header.FileOffs;
}



void ObjStartSegments (void)
/* Mark the start of the segment section */
{
    Header.SegOffs = ftell (F);
}



void ObjEndSegments (void)
/* Mark the end of the segment section */
{
    Header.SegSize = ftell (F) - Header.SegOffs;
}



void ObjStartImports (void)
/* Mark the start of the import section */
{
    Header.ImportOffs = ftell (F);
}



void ObjEndImports (void)
/* Mark the end of the import section */
{
    Header.ImportSize = ftell (F) - Header.ImportOffs;
}



void ObjStartExports (void)
/* Mark the start of the export section */
{
    Header.ExportOffs = ftell (F);
}



void ObjEndExports (void)
/* Mark the end of the export section */
{
    Header.ExportSize = ftell (F) - Header.ExportOffs;
}



void ObjStartDbgSyms (void)
/* Mark the start of the debug symbol section */
{
    Header.DbgSymOffs = ftell (F);
}



void ObjEndDbgSyms (void)
/* Mark the end of the debug symbol section */
{
    Header.DbgSymSize = ftell (F) - Header.DbgSymOffs;
}



