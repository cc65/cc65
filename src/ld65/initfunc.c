/*****************************************************************************/
/*                                                                           */
/*				  initfunc.c				     */
/*                                                                           */
/*			Init/cleanup function handling			     */
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
#include "coll.h"

/* ld65 */
#include "exports.h"
#include "segments.h"
#include "initfunc.h"



/*****************************************************************************/
/*		 		     Data				     */
/*****************************************************************************/



/* List of all exports that are also initializers/cleanup functions */
static Collection   InitFunctions	= STATIC_COLLECTION_INITIALIZER;
static Collection   CleanupFunctions	= STATIC_COLLECTION_INITIALIZER;



/*****************************************************************************/
/*     	       	     	   	     Code  	      	  	  	     */
/*****************************************************************************/



void AddInitFunc (Export* E)
/* Add the given export to the list of initializers */
{
    CollAppend (&InitFunctions, E);
}



void AddCleanupFunc (Export* E)
/* Add the given export to the list of cleanup functions */
{
    CollAppend (&CleanupFunctions, E);
}



