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



#ifndef CONDES_H
#define CONDES_H



/*****************************************************************************/
/*	      	  		   Forwards				     */
/*****************************************************************************/



struct Export;



/*****************************************************************************/
/*				     Data				     */
/*****************************************************************************/



/* Order of the tables */
typedef enum {
    cdIncreasing,		/* Increasing priority - default */
    cdDecreasing		/* Decreasing priority */
} ConDesOrder;



/*****************************************************************************/
/*     	       	     	   	     Code  	      	  	  	     */
/*****************************************************************************/



void ConDesAddExport (struct Export* E);
/* Add the given export to the list of constructors/destructor */

void ConDesSetSegName (unsigned Type, const char* SegName);
/* Set the segment name where the table should go */

void ConDesSetLabel (unsigned Type, const char* Name);
/* Set the label for the given ConDes type */

void ConDesSetCountSym (unsigned Type, const char* Name);
/* Set the name for the given ConDes count symbol */

void ConDesSetOrder (unsigned Type, ConDesOrder Order);
/* Set the sorting oder for the given ConDes table */

int ConDesHasSegName (unsigned Type);
/* Return true if a segment name is already defined for this ConDes type */

int ConDesHasLabel (unsigned Type);
/* Return true if a label is already defined for this ConDes type */

void ConDesCreate (void);
/* Create the condes tables if requested */

void ConDesDump (void);
/* Dump ConDes data to stdout for debugging */



/* End of condes.h */

#endif




