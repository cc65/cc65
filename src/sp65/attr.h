/*****************************************************************************/
/*                                                                           */
/*                                  attr.h                                   */
/*                                                                           */
/*                          Command line attributes                          */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2012,      Ullrich von Bassewitz                                      */
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



#ifndef ATTRCOLL_H
#define ATTRCOLL_H



/* common */
#include "coll.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Attribute structure */
typedef struct Attr Attr;
struct Attr {
    char*       Name;           /* Attribute name - points into Value */
    char        Value[1];       /* Attribute value followed by Name */
};



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



Attr* NewAttr (const char* Name, const char* Value);
/* Create a new attribute */

void FreeAttr (Attr* A);
/* Free an attribute structure */

void DumpAttrColl (const Collection* C);
/* Dump a collection of attribute/value pairs for debugging */

int FindAttr (const Collection* C, const char* Name, unsigned* Index);
/* Search for an attribute with the given name in the collection. If it is
** found, the function returns true and Index contains the index of the
** entry. If Name isn't found, the function returns false and Index
** will contain the insert position.
*/

const Attr* GetAttr (const Collection* C, const char* Name);
/* Search for an attribute with the given name and return it. The function
** returns NULL if the attribute wasn't found.
*/

const Attr* NeedAttr (const Collection* C, const char* Name, const char* Op);
/* Search for an attribute with the given name and return it. If the attribute
** is not found, the function terminates with an error using Op as additional
** context in the error message.
*/

const char* GetAttrVal (const Collection* C, const char* Name);
/* Search for an attribute with the given name and return its value. The
** function returns NULL if the attribute wasn't found.
*/

const char* NeedAttrVal (const Collection* C, const char* Name, const char* Op);
/* Search for an attribute with the given name and return its value. If the
** attribute wasn't not found, the function terminates with an error using
** Op as additional context in the error message.
*/

void AddAttr (Collection* C, const char* Name, const char* Value);
/* Add an attribute to an alphabetically sorted attribute collection */

void SplitAddAttr (Collection* C, const char* Combined, const char* Name);
/* Split a combined name/value pair and add it as an attribute to C. Some
** attributes may not need a name. If the name is missing, use Name. If
** Name is NULL, terminate with an error.
*/

Collection* ParseAttrList (const char* List, const char** NameList, unsigned NameCount);
/* Parse a list containing name/value pairs into a sorted collection. Some
** attributes may not need a name, so NameList contains these names. If there
** were no errors, the function returns a alphabetically sorted collection
** containing Attr entries.
*/

void FreeAttrList (Collection* C);
/* Free a list of attributes */



/* End of attr.h */

#endif
