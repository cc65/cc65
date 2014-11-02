/*****************************************************************************/
/*                                                                           */
/*                                 objdata.h                                 */
/*                                                                           */
/*              Handling object file data for the ar65 archiver              */
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



#ifndef OBJDATA_H
#define OBJDATA_H



/* common */
#include "coll.h"
#include "objdefs.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Values for the Flags field */
#define OBJ_HAVEDATA    0x0001          /* The object data is in the tmp file */


/* Internal structure holding object file data */
typedef struct ObjData ObjData;
struct ObjData {
    char*               Name;           /* Module name */

    /* Index entry */
    unsigned            Flags;
    unsigned long       MTime;          /* Modifiation time of object file */
    unsigned long       Start;          /* Start offset of data in library */
    unsigned long       Size;           /* Size of data in library */

    /* Object file header */
    ObjHeader           Header;

    /* Basic data needed for simple checks */
    Collection          Strings;        /* Strings from the object file */
    Collection          Exports;        /* Exports list from object file */
};



/* Collection with all object files */
extern Collection       ObjPool;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



ObjData* NewObjData (void);
/* Allocate a new structure on the heap, insert it into the list, return it */

void FreeObjData (ObjData* O);
/* Free a complete struct */

void ClearObjData (ObjData* O);
/* Remove any data stored in O */

ObjData* FindObjData (const char* Module);
/* Search for the module with the given name and return it. Return NULL if the
** module is not in the list.
*/

void DelObjData (const char* Module);
/* Delete the object module from the list */



/* End of objdata.h */

#endif
