/*****************************************************************************/
/*                                                                           */
/*                                 objfile.c                                 */
/*                                                                           */
/*         Object file writing routines for the ca65 macroassembler          */
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
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* common */
#include "fname.h"
#include "objdefs.h"

/* ca65 */
#include "global.h"
#include "error.h"
#include "objfile.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* File descriptor */
static FILE* F = 0;

/* Default extension */
#define OBJ_EXT ".o"

/* Header structure */
static ObjHeader Header = {
    OBJ_MAGIC,          /* 32: Magic number */
    OBJ_VERSION,        /* 16: Version number */
    0,                  /* 16: flags */
    0,                  /* 32: Offset to option table */
    0,                  /* 32: Size of options */
    0,                  /* 32: Offset to file table */
    0,                  /* 32: Size of files */
    0,                  /* 32: Offset to segment table */
    0,                  /* 32: Size of segment table */
    0,                  /* 32: Offset to import list */
    0,                  /* 32: Size of import list */
    0,                  /* 32: Offset to export list */
    0,                  /* 32: Size of export list */
    0,                  /* 32: Offset to list of debug symbols */
    0,                  /* 32: Size of debug symbols */
    0,                  /* 32: Offset to list of line infos */
    0,                  /* 32: Size of line infos */
    0,                  /* 32: Offset to string pool */
    0,                  /* 32: Size of string pool */
    0,                  /* 32: Offset to assertion table */
    0,                  /* 32: Size of assertion table */
    0,                  /* 32: Offset into scope table */
    0,                  /* 32: Size of scope table */
    0,                  /* 32: Offset into span table */
    0,                  /* 32: Size of span table */
};



/*****************************************************************************/
/*                         Internally used functions                         */
/*****************************************************************************/



static void ObjWriteError (void)
/* Called on a write error. Will try to close and remove the file, then
** print a fatal error.
*/
{
    /* Remember the error */
    int Error = errno;

    /* Force a close of the file, ignoring errors */
    fclose (F);

    /* Try to remove the file, also ignoring errors */
    remove (OutFile);

    /* Now abort with a fatal error */
    Fatal ("Cannot write to output file `%s': %s", OutFile, strerror (Error));
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
    ObjWrite32 (Header.LineInfoOffs);
    ObjWrite32 (Header.LineInfoSize);
    ObjWrite32 (Header.StrPoolOffs);
    ObjWrite32 (Header.StrPoolSize);
    ObjWrite32 (Header.AssertOffs);
    ObjWrite32 (Header.AssertSize);
    ObjWrite32 (Header.ScopeOffs);
    ObjWrite32 (Header.ScopeSize);
    ObjWrite32 (Header.SpanOffs);
    ObjWrite32 (Header.SpanSize);
}



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void ObjOpen (void)
/* Open the object file for writing, write a dummy header */
{
    /* Do we have a name for the output file? */
    if (OutFile == 0) {
        /* We don't have an output name explicitly given, construct one from
        ** the name of the input file.
        */
        OutFile = MakeFilename (InFile, OBJ_EXT);
    }

    /* Create the output file */
    F = fopen (OutFile, "w+b");
    if (F == 0) {
        Fatal ("Cannot open output file `%s': %s", OutFile, strerror (errno));
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



unsigned long ObjGetFilePos (void)
/* Get the current file position */
{
    long Pos = ftell (F);
    if (Pos < 0) {
        ObjWriteError ();
    }
    return Pos;
}



void ObjSetFilePos (unsigned long Pos)
/* Set the file position */
{
    if (fseek (F, Pos, SEEK_SET) != 0) {
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



void ObjWriteVar (unsigned long V)
/* Write a variable sized value to the file in special encoding */
{
    /* We will write the value to the file in 7 bit chunks. If the 8th bit
    ** is clear, we're done, if it is set, another chunk follows. This will
    ** allow us to encode smaller values with less bytes, at the expense of
    ** needing 5 bytes if a 32 bit value is written to file.
    */
    do {
        unsigned char C = (V & 0x7F);
        V >>= 7;
        if (V) {
            C |= 0x80;
        }
        ObjWrite8 (C);
    } while (V != 0);
}



void ObjWriteStr (const char* S)
/* Write a string to the object file */
{
    unsigned Len = strlen (S);

    /* Write the string with the length preceeded (this is easier for
    ** the reading routine than the C format since the length is known in
    ** advance).
    */
    ObjWriteVar (Len);
    ObjWriteData (S, Len);
}



void ObjWriteBuf (const StrBuf* S)
/* Write a string to the object file */
{
    /* Write the string with the length preceeded (this is easier for
    ** the reading routine than the C format since the length is known in
    ** advance).
    */
    ObjWriteVar (SB_GetLen (S));
    ObjWriteData (SB_GetConstBuf (S), SB_GetLen (S));
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
    /* Write the data entries */
    ObjWriteVar (Pos->Line);
    ObjWriteVar (Pos->Col);
    if (Pos->Name == 0) {
        /* Position is outside file scope, use the main file instead */
        ObjWriteVar (0);
    } else {
        ObjWriteVar (Pos->Name - 1);
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



void ObjStartLineInfos (void)
/* Mark the start of the line info section */
{
    Header.LineInfoOffs = ftell (F);
}



void ObjEndLineInfos (void)
/* Mark the end of the line info section */
{
    Header.LineInfoSize = ftell (F) - Header.LineInfoOffs;
}



void ObjStartStrPool (void)
/* Mark the start of the string pool section */
{
    Header.StrPoolOffs = ftell (F);
}



void ObjEndStrPool (void)
/* Mark the end of the string pool section */
{
    Header.StrPoolSize = ftell (F) - Header.StrPoolOffs;
}



void ObjStartAssertions (void)
/* Mark the start of the assertion table */
{
    Header.AssertOffs = ftell (F);
}



void ObjEndAssertions (void)
/* Mark the end of the assertion table */
{
    Header.AssertSize = ftell (F) - Header.AssertOffs;
}



void ObjStartScopes (void)
/* Mark the start of the scope table */
{
    Header.ScopeOffs = ftell (F);
}



void ObjEndScopes (void)
/* Mark the end of the scope table */
{
    Header.ScopeSize = ftell (F) - Header.ScopeOffs;
}



void ObjStartSpans (void)
/* Mark the start of the span table */
{
    Header.SpanOffs = ftell (F);
}



void ObjEndSpans (void)
/* Mark the end of the span table */
{
    Header.SpanSize = ftell (F) - Header.SpanOffs;
}
