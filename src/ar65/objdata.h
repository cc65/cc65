/*****************************************************************************/
/*                                                                           */
/*				   objdata.h				     */
/*                                                                           */
/*		Handling object file data for the ar65 archiver		     */
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



#ifndef OBJDATA_H
#define OBJDATA_H



/*****************************************************************************/
/*     	      	    		     Data				     */
/*****************************************************************************/



/* Values for the Flags field */
#define	OBJ_HAVEDATA	0x0001		/* The object data is in the tmp file */
#define OBJ_MARKED	0x0002		/* Generic marker bit */


/* Internal structure holding object file data */
typedef struct ObjData_ ObjData;
struct ObjData_ {
    ObjData*	     	Next;		/* Linked list of all objects */
    char*   	     	Name;		/* Module name */
    unsigned		Index;		/* Module index */
    unsigned 	     	Flags;
    unsigned long    	MTime;		/* Modifiation time of object file */
    unsigned long	Start;		/* Start offset of data in library */
    unsigned long	Size;		/* Size of data in library */
    unsigned long      	ImportSize;    	/* Size of imports */
    void*    	 	Imports;       	/* Imports as raw data */
    unsigned long	ExportSize;	/* Size of exports */
    void*		Exports;       	/* Exports as raw data */
};



/* Object data list management */
extern unsigned		ObjCount;	/* Count of files in the list */
extern ObjData*		ObjRoot;	/* List of object files */
extern ObjData*		ObjLast;	/* Last entry in list */
extern ObjData**	ObjPool;	/* Object files as array */



/*****************************************************************************/
/*     	      	    		     Code		  	   	     */
/*****************************************************************************/



ObjData* NewObjData (void);
/* Allocate a new structure on the heap, insert it into the list, return it */

void FreeObjData (ObjData* O);
/* Free a complete struct */

ObjData* FindObjData (const char* Module);
/* Search for the module with the given name and return it. Return NULL if the
 * module is not in the list.
 */

void DelObjData (const char* Module);
/* Delete the object module from the list */

void MakeObjPool (void);
/* Allocate memory, index the entries and make the ObjPool valid */

const char* GetObjName (unsigned Index);
/* Get the name of a module by index */



/* End of objdata.h */

#endif



