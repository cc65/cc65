/*****************************************************************************/
/*									     */
/*				   objfile.c				     */
/*									     */
/*		   Object file handling for the ld65 linker		     */
/*									     */
/*									     */
/*									     */
/* (C) 1998	Ullrich von Bassewitz					     */
/*		Wacholderweg 14						     */
/*		D-70597 Stuttgart					     */
/* EMail:	uz@musoftware.de					     */
/*									     */
/*									     */
/* This software is provided 'as-is', without any expressed or implied	     */
/* warranty.  In no event will the authors be held liable for any damages    */
/* arising from the use of this software.				     */
/*									     */
/* Permission is granted to anyone to use this software for any purpose,     */
/* including commercial applications, and to alter it and redistribute it    */
/* freely, subject to the following restrictions:			     */
/*									     */
/* 1. The origin of this software must not be misrepresented; you must not   */
/*    claim that you wrote the original software. If you use this software   */
/*    in a product, an acknowledgment in the product documentation would be  */
/*    appreciated but is not required.					     */
/* 2. Altered source versions must be plainly marked as such, and must not   */
/*    be misrepresented as being the original software.			     */
/* 3. This notice may not be removed or altered from any source		     */
/*    distribution.   							     */
/*		      							     */
/*****************************************************************************/



#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>		/* EMX needs this */
#include <sys/stat.h>

/* common */
#include "xmalloc.h"

/* ld65 */
#include "dbgsyms.h"
#include "error.h"
#include "exports.h"
#include "fileio.h"
#include "lineinfo.h"
#include "objdata.h"
#include "segments.h"
#include "objfile.h"



/*****************************************************************************/
/*				     Code				     */
/*****************************************************************************/



static const char* GetModule (const char* Name)
/* Get a module name from the file name */
{
    /* Make a module name from the file name */
    const char* Module = Name + strlen (Name);
    while (Module > Name) {
	--Module;
	if (*Module == '/' || *Module == '\\') {
	    ++Module;
	    break;
	}
    }
    if (*Module == 0) {
	Error ("Cannot make module name from `%s'", Name);
    }
    return Module;
}



static void ObjReadHeader (FILE* Obj, ObjHeader* H, const char* Name)
/* Read the header of the object file checking the signature */
{
    H->Version	  = Read16 (Obj);
    if (H->Version != OBJ_VERSION) {
       	Error ("Object file `%s' has wrong version", Name);
    }
    H->Flags	    = Read16 (Obj);
    H->OptionOffs   = Read32 (Obj);
    H->OptionSize   = Read32 (Obj);
    H->FileOffs     = Read32 (Obj);
    H->FileSize     = Read32 (Obj);
    H->SegOffs	    = Read32 (Obj);
    H->SegSize	    = Read32 (Obj);
    H->ImportOffs   = Read32 (Obj);
    H->ImportSize   = Read32 (Obj);
    H->ExportOffs   = Read32 (Obj);
    H->ExportSize   = Read32 (Obj);
    H->DbgSymOffs   = Read32 (Obj);
    H->DbgSymSize   = Read32 (Obj);
    H->LineInfoOffs = Read32 (Obj);
    H->LineInfoSize = Read32 (Obj);
}



void ObjReadFiles (FILE* F, ObjData* O)
/* Read the files list from a file at the current position */
{
    unsigned I;

    O->FileCount  = ReadVar (F);
    O->Files      = xmalloc (O->FileCount * sizeof (char*));
    for (I = 0; I < O->FileCount; ++I) {
       	/* Skip MTime and size */
       	Read32 (F);
       	Read32 (F);
       	/* Read the filename */
       	O->Files [I] = ReadStr (F);
    }
}



void ObjReadImports (FILE* F, ObjData* O)
/* Read the imports from a file at the current position */
{
    unsigned I;

    O->ImportCount = ReadVar (F);
    O->Imports     = xmalloc (O->ImportCount * sizeof (Import*));
    for (I = 0; I < O->ImportCount; ++I) {
   	O->Imports [I] = ReadImport (F, O);
	InsertImport (O->Imports [I]);
    }
}



void ObjReadExports (FILE* F, ObjData* O)
/* Read the exports from a file at the current position */
{
    unsigned I;

    O->ExportCount = ReadVar (F);
    O->Exports     = xmalloc (O->ExportCount * sizeof (Export*));
    for (I = 0; I < O->ExportCount; ++I) {
	O->Exports [I] = ReadExport (F, O);
	InsertExport (O->Exports [I]);
    }
}



void ObjReadDbgSyms (FILE* F, ObjData* O)
/* Read the debug symbols from a file at the current position */
{
    unsigned I;
		     
    O->DbgSymCount = ReadVar (F);
    O->DbgSyms	   = xmalloc (O->DbgSymCount * sizeof (DbgSym*));
    for (I = 0; I < O->DbgSymCount; ++I) {
	O->DbgSyms [I] = ReadDbgSym (F, O);
    }
}



void ObjReadLineInfos (FILE* F, ObjData* O)
/* Read the line infos from a file at the current position */
{
    unsigned I;

    O->LineInfoCount = ReadVar (F);
    O->LineInfos     = xmalloc (O->LineInfoCount * sizeof (LineInfo*));
    for (I = 0; I < O->LineInfoCount; ++I) {
       	O->LineInfos[I] = ReadLineInfo (F, O);
    }
}



void ObjReadSections (FILE* F, ObjData* O)
/* Read the section data from a file at the current position */
{
    unsigned I;

    O->SectionCount = ReadVar (F);
    O->Sections     = xmalloc (O->SectionCount * sizeof (Section*));
    for (I = 0; I < O->SectionCount; ++I) {
	O->Sections [I] = ReadSection (F, O);
    }
}



void ObjAdd (FILE* Obj, const char* Name)
/* Add an object file to the module list */
{
    /* Create a new structure for the object file data */
    ObjData* O = NewObjData ();

    /* The magic was already read and checked, so set it in the header */
    O->Header.Magic = OBJ_MAGIC;

    /* Read and check the header */
    ObjReadHeader (Obj, &O->Header, Name);

    /* Initialize the object module data structure */
    O->Name    	  = xstrdup (GetModule (Name));
    O->Flags   	  = OBJ_HAVEDATA;

    /* Read the files list from the object file */
    fseek (Obj, O->Header.FileOffs, SEEK_SET);
    ObjReadFiles (Obj, O);

    /* Read the imports list from the object file */
    fseek (Obj, O->Header.ImportOffs, SEEK_SET);
    ObjReadImports (Obj, O);

    /* Read the object file exports and insert them into the exports list */
    fseek (Obj, O->Header.ExportOffs, SEEK_SET);
    ObjReadExports (Obj, O);

    /* Read the object debug symbols from the object file */
    fseek (Obj, O->Header.DbgSymOffs, SEEK_SET);
    ObjReadDbgSyms (Obj, O);

    /* Read the line infos from the object file */
    fseek (Obj, O->Header.LineInfoOffs, SEEK_SET);
    ObjReadLineInfos (Obj, O);

    /* Read the segment list from the object file. This must be last, since
     * the expressions stored in the code may reference segments or imported
     * symbols.
     */
    fseek (Obj, O->Header.SegOffs, SEEK_SET);
    ObjReadSections (Obj, O);

    /* Mark this object file as needed */
    O->Flags |= OBJ_REF | OBJ_HAVEDATA;

    /* Done, close the file (we read it only, so no error check) */
    fclose (Obj);
}




