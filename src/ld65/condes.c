/*****************************************************************************/
/*                                                                           */
/*				   condes.h				     */
/*                                                                           */
/*		     Module constructor/destructor support		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000      Ullrich von Bassewitz                                       */
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



/* common */
#include "check.h"
#include "coll.h"
#include "xmalloc.h"

/* ld65 */
#include "exports.h"
#include "segments.h"
#include "condes.h"



/*****************************************************************************/
/*     		 		     Data				     */
/*****************************************************************************/



/* Struct describing one condes type */
typedef struct ConDesDesc ConDesDesc;
struct ConDesDesc {
    Collection		ExpList;	/* List of exported symbols */
    char*     		Label;		/* Name of table label */
    char*     		SegName;	/* Name of segment the table is in */
    unsigned char	Enable;		/* Table enabled */
};

/* Array for all types */
static ConDesDesc ConDes[CD_TYPE_COUNT] = {
    { STATIC_COLLECTION_INITIALIZER, 0, 0, 0 },
    { STATIC_COLLECTION_INITIALIZER, 0, 0, 0 },
    { STATIC_COLLECTION_INITIALIZER, 0, 0, 0 },
    { STATIC_COLLECTION_INITIALIZER, 0, 0, 0 },
    { STATIC_COLLECTION_INITIALIZER, 0, 0, 0 },
    { STATIC_COLLECTION_INITIALIZER, 0, 0, 0 },
    { STATIC_COLLECTION_INITIALIZER, 0, 0, 0 },
};



/*****************************************************************************/
/*     	       	     	   	     Code  	      	  	  	     */
/*****************************************************************************/



void ConDesAddExport (struct Export* E)
/* Add the given export to the list of constructors/destructor */
{
    unsigned Type;

    /* Insert the export into all tables for which declarations exist */
    for (Type = 0; Type < CD_TYPE_COUNT; ++Type) {
  	unsigned Prio = E->ConDes[Type];
  	if (Prio != CD_PRIO_NONE) {
       	    CollAppend (&ConDes[Type].ExpList, E);
  	}
    }
}



void ConDesSetSegName (unsigned Type, const char* SegName)
/* Set the segment name where the table should go */
{
    /* Check the parameters */
    PRECONDITION (Type >= CD_TYPE_MIN && Type <= CD_TYPE_MAX && SegName != 0);

    /* Setting the segment name twice is bad */
    CHECK (ConDes[Type].SegName == 0);

    /* Set the name */
    ConDes[Type].SegName = xstrdup (SegName);
}



void ConDesSetLabel (unsigned Type, const char* Name)
/* Set the label for the given ConDes type */
{
    /* Check the parameters */
    PRECONDITION (Type >= CD_TYPE_MIN && Type <= CD_TYPE_MAX && Name != 0);

    /* Setting the label twice is bad */
    CHECK (ConDes[Type].Label == 0);

    /* Set the name */
    ConDes[Type].Label = xstrdup (Name);
}



const char* ConDesGetSegName (unsigned Type)
/* Return the segment name for the given ConDes type */
{
    /* Check the parameters */
    PRECONDITION (Type >= CD_TYPE_MIN && Type <= CD_TYPE_MAX);

    /* Return the name */
    return ConDes[Type].SegName;
}



const char* ConDesGetLabel (unsigned Type)
/* Return the label for the given ConDes type */
{
    /* Check the parameters */
    PRECONDITION (Type >= CD_TYPE_MIN && Type <= CD_TYPE_MAX);

    /* Return the name */
    return ConDes[Type].Label;
}



int ConDesHasSegName (unsigned Type)
/* Return true if a segment name is already defined for this ConDes type */
{
    return (ConDesGetSegName(Type) != 0);
}



int ConDesHasLabel (unsigned Type)
/* Return true if a label is already defined for this ConDes type */
{
    return (ConDesGetLabel(Type) != 0);
}



void ConDesCreate (void)
/* Create the condes tables if requested */
{
}



void ConDesDump (void)
/* Dump ConDes data to stdout for debugging */
{
    unsigned Type;
    for (Type = 0; Type < CD_TYPE_COUNT; ++Type) {
       	Collection* ExpList = &ConDes[Type].ExpList;
	printf ("CONDES(%u): %u symbols\n", Type, CollCount (ExpList));
    }
}





