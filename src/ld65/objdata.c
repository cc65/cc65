/*****************************************************************************/
/*                                                                           */
/*				   objdata.c				     */
/*                                                                           */
/*		 Handling object file data for the ld65 linker		     */
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



#include <string.h>

/* common */
#include "check.h"
#include "xmalloc.h"

/* ld65 */
#include "error.h"
#include "objdata.h"



/*****************************************************************************/
/*     	      	    		     Data				     */
/*****************************************************************************/



/* Object data list management */
unsigned    	ObjCount = 0;	/* Count of object files in the list */
ObjData*    	ObjRoot  = 0;	/* List of object files */
ObjData*    	ObjLast  = 0;	/* Last entry in list */
ObjData**      	ObjPool  = 0;	/* Object files as array */



/*****************************************************************************/
/*     	      	    		     Code				     */
/*****************************************************************************/



ObjData* NewObjData (void)
/* Allocate a new structure on the heap, insert it into the list, return it */
{
    /* Allocate memory */
    ObjData* O = xmalloc (sizeof (ObjData));

    /* Initialize the data */
    O->Next        	= 0;
    O->Name  	   	= 0;
    O->LibName    	= 0;
    O->Flags   	   	= 0;
    O->Start	   	= 0;
    O->ExportCount 	= 0;
    O->Exports     	= 0;
    O->ImportCount 	= 0;
    O->Imports     	= 0;
    O->DbgSymCount	= 0;
    O->DbgSyms		= 0;

    /* Link it into the list */
    if (ObjLast) {
     	ObjLast->Next = O;
     	ObjLast       = O;
    } else {
     	/* First entry */
     	ObjRoot = ObjLast = O;
    }

    /* One object file more now */
    ++ObjCount;

    /* Return the new entry */
    return O;
}



void FreeObjData (ObjData* O)
/* Free a complete struct */
{
    xfree (O->Name);
    xfree (O->Imports);
    xfree (O->Exports);
    xfree (O->DbgSyms);
    xfree (O);
}



const char* GetObjFileName (const ObjData* O)
/* Get the name of the object file. Return "[linker generated]" if the object
 * file is NULL.
 */
{
    return O? O->Name : "[linker generated]";
}



const char* GetSourceFileName (const ObjData* O, unsigned Index)
/* Get the name of the source file with the given index. If O is NULL, return
 * "[linker generated]" as the file name.
 */
{
    /* Check if we have an object file */
    if (O == 0) {

     	/* No object file */
     	return "[linker generated]";

    } else {

	/* Check the parameter */
	PRECONDITION (Index < O->FileCount);

	/* Return the name */
	return O->Files[Index];

    }		 
}




