/*****************************************************************************/
/*                                                                           */
/*				   exports.h				     */
/*                                                                           */
/*	     	      Exports handing for the ld65 linker		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2000 Ullrich von Bassewitz                                       */
/*               Wacholderweg 14                                             */
/*               D-70597 Stuttgart                                           */
/* EMail:        uz@musoftware.de                                            */
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



#ifndef EXPORTS_H
#define EXPORTS_H



#include <stdio.h>

/* common */
#include "cddefs.h"
#include "exprdefs.h"
#include "filepos.h"

/* ld65 */
#include "objdata.h"
#include "config.h"



/*****************************************************************************/
/*     	      	    	      	     Data				     */
/*****************************************************************************/



/* Import symbol structure */
typedef struct Import Import;
struct Import {
    Import*  		Next;		/* Single linked list */
    ObjData* 		Obj;		/* Object file that imports the name */
    FilePos  		Pos;		/* File position of reference */
    union {
	struct Export*	Exp;		/* Matching export for this import */
	const char*  	Name;		/* Name if not in table */
    } V;
    unsigned char    	Type;		/* Type of import */
};



/* Export symbol structure */
typedef struct Export Export;
struct Export {
    Export*    	       	Next;  		/* Hash table link */
    unsigned 		Flags;		/* Generic flags */
    ObjData* 		Obj;		/* Object file that exports the name */
    unsigned 		ImpCount;	/* How many imports for this symbol? */
    Import*  		ImpList;	/* List of imports for this symbol */
    FilePos  		Pos;		/* File position of definition */
    ExprNode*  		Expr;		/* Expression (0 if not def'd) */
    unsigned char	Type;		/* Type of export */		  
    unsigned char	ConDes[CD_TYPE_COUNT];	/* Constructor/destructor decls */
    char*      	       	Name;		/* Name - dynamically allocated */
};



/* Prototype of a function that is called if an undefined symbol is found. It
 * may check if the symbol is an external symbol (for binary formats that
 * support externals) and will return zero if the symbol could not be
 * resolved, or a value != zero if the symbol could be resolved. The
 * CheckExports routine will print out the missing symbol in the first case.
 */
typedef int (*ExpCheckFunc) (const char* Name, void* Data);



/*****************************************************************************/
/*     	      	    		     Code			       	     */
/*****************************************************************************/



Import* ReadImport (FILE* F, ObjData* Obj);
/* Read an import from a file and insert it into the table */

void InsertImport (Import* I);
/* Insert an import into the table */

Export* ReadExport (FILE* F, ObjData* Obj);
/* Read an export from a file */

void InsertExport (Export* E);
/* Insert an exported identifier and check if it's already in the list */

Export* CreateConstExport (const char* Name, long Value);
/* Create an export for a literal date */

Export* CreateMemExport (const char* Name, Memory* Mem, unsigned long Offs);
/* Create an relative export for a memory area offset */

int IsUnresolved (const char* Name);
/* Check if this symbol is an unresolved export */

int IsConstExport (const Export* E);
/* Return true if the expression associated with this export is const */

long GetExportVal (const Export* E);
/* Get the value of this export */

void CheckExports (ExpCheckFunc F, void* Data);
/* Check if there are any unresolved symbols. On unresolved symbols, F is
 * called (see the comments on ExpCheckFunc in the data section).
 */

void PrintExportMap (FILE* F);
/* Print an export map to the given file */

void PrintImportMap (FILE* F);
/* Print an import map to the given file */

void PrintExportLabels (FILE* F);
/* Print the exports in a VICE label file */

void MarkExport (Export* E);
/* Mark the export */

void UnmarkExport (Export* E);
/* Remove the mark from the export */

int ExportHasMark (Export* E);
/* Return true if the export has a mark */

void CircularRefError (const Export* E);
/* Print an error about a circular reference using to define the given export */



/* End of exports.h */

#endif



