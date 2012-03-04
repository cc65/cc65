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



/* Attribute flags */
enum AttrFlags {
    afNone,
    afInt,                              /* Integer number */
};
typedef enum AttrFlags AttrFlags;

/* */
typedef struct Attr Attr;
struct Attr {
    AttrFlags   Flags;                  /* Attribute flags */
    char*       Name;                   /* Attribute name */
    char        Value[1];               /* Attribute value */
};



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



Attr* NewAttr (const char* Name, const char* Value);
/* Create a new attribute */

void DumpAttrColl (const Collection* C);
/* Dump a collection of attribute/value pairs for debugging */

int FindAttr (const Collection* C, const char* Name, unsigned* Index);
/* Search for an attribute with the given name in the collection. If it is
 * found, the function returns true and Index contains the index of the
 * entry. If Name isn't found, the function returns false and Index
 * will contain the insert position.
 */

void AddAttr (Collection* C, const char* Name, const char* Value);
/* Add an attribute to an alphabetically sorted attribute collection */

void SplitAddAttr (Collection* C, const char* Combined, const char* Name);
/* Split a combined name/value pair and add it as an attribute to C. Some
 * attributes may not need a name. If the name is missing, use Name. If
 * Name is NULL, terminate with an error.
 */



/* End of attr.h */

#endif



