/*****************************************************************************/
/*                                                                           */
/*				  declattr.h				     */
/*                                                                           */
/*			    Declaration attributes			     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000     Ullrich von Bassewitz                                        */
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



#ifndef DECLATTR_H
#define DECLATTR_H



#include "declare.h"



/*****************************************************************************/
/*	   	    		     Data				     */
/*****************************************************************************/



/* Supported attribute types */
typedef enum {
    atNone	= -1,	   	/* No attribute */
    atAlias,  	    	   	/* Alias declaration */

    atCount	    	   	/* Number of attributes */
} attrib_t;

/* An actual attribute description */
typedef struct DeclAttr DeclAttr;
struct DeclAttr {
    attrib_t			AttrType;	/* Type of attribute */

    union {
	struct SymEntry*	Sym;		/* Symbol for alias */
    } V;
};



/*****************************************************************************/
/* 	   	    		     Code				     */
/*****************************************************************************/



void ParseAttribute (const Declaration* D, DeclAttr* A);
/* Parse an additional __attribute__ modifier */



/* End of declattr.h */

#endif



