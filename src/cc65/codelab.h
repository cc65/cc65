/*****************************************************************************/
/*                                                                           */
/*				   codelab.h				     */
/*                                                                           */
/*			     Code label structure			     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001     Ullrich von Bassewitz                                        */
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



#ifndef CODELAB_H
#define CODELAB_H



#include <stdio.h>

/* common */
#include "coll.h"



/*****************************************************************************/
/*				   Forwards				     */
/*****************************************************************************/



struct CodeEntry;



/*****************************************************************************/
/*		  	       struct CodeLabel				     */
/*****************************************************************************/



/* Label flags, bitmapped */
#define LF_DEF		0x0001U		/* Label was defined */

/* Label structure */
typedef struct CodeLabel CodeLabel;
struct CodeLabel {
    CodeLabel*		Next;		/* Next in hash list */
    char*		Name;		/* Label name */
    unsigned short	Hash;		/* Hash over the name */
    unsigned short	Flags;		/* Flag flags */
    struct CodeEntry*	Owner;		/* Owner entry */
    Collection	    	JumpFrom;	/* Entries that jump here */
};



/*****************************************************************************/
/*     	       	      	  	     Code			     	     */
/*****************************************************************************/



CodeLabel* NewCodeLabel (const char* Name, unsigned Hash);
/* Create a new code label, initialize and return it */

void FreeCodeLabel (CodeLabel* L);
/* Free the given code label */

void AddLabelRef (CodeLabel* L, struct CodeEntry* E);
/* Let the CodeEntry E reference the label L */

void MoveLabelRefs (CodeLabel* OldLabel, CodeLabel* NewLabel);
/* Move all references to OldLabel to point to NewLabel. OldLabel will have no
 * more references on return.
 */

void OutputCodeLabel (const CodeLabel* L, FILE* F);
/* Output the code label to a file */



/* End of codelab.h */
#endif



