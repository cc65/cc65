/*****************************************************************************/
/*                                                                           */
/*				   coptc02.h                                 */
/*                                                                           */
/*			 65C02 specific optimizations                        */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001      Ullrich von Bassewitz                                       */
/*               Wacholderweg 14                                             */
/*               D-70597 Stuttgart                                           */
/* EMail:        uz@cc65.org                                                 */
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

/* cc65 */
#include "codeent.h"
#include "codeinfo.h"
#include "error.h"
#include "coptc02.h"



/*****************************************************************************/
/*  	       	  	  	     Data				     */
/*****************************************************************************/



/*****************************************************************************/
/*		  	       Helper functions                              */
/*****************************************************************************/



/*****************************************************************************/
/*	      			     Code                                    */
/*****************************************************************************/



unsigned Opt65C02Ind (CodeSeg* S)
/* Try to use the indirect addressing mode where possible */
{
    unsigned Changes = 0;
    unsigned I;

    /* Generate register info for this step */
    CS_GenRegInfo (S);

    /* Walk over the entries */
    I = 0;
    while (I < CS_GetEntryCount (S)) {

      	/* Get next entry */
       	CodeEntry* E = CS_GetEntry (S, I);

       	/* Check for addressing mode indirect indexed Y where Y is zero. 
	 * Note: All opcodes that are available as (zp),y are also available
	 * as (zp), so we can ignore the actual opcode here.
	 */
	if (E->AM == AM65_ZP_INDY && E->RI->In.RegY == 0) {

	    /* Replace it by indirect addressing mode */
	    CodeEntry* X = NewCodeEntry (E->OPC, AM65_ZP_IND, E->Arg, 0, E->LI);
	    CS_InsertEntry (S, X, I+1);
	    CS_DelEntry (S, I);

	    /* We had changes */
	    ++Changes;

	}

     	/* Next entry */
	++I;

    }

    /* Free register info */
    CS_FreeRegInfo (S);

    /* Return the number of changes made */
    return Changes;
}




