/*****************************************************************************/
/*                                                                           */
/*				   symdefs.h   	       	       	       	     */
/*                                                                           */
/*		 Symbol definitions for the bin65 binary utils		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2010, Ullrich von Bassewitz                                      */
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



/* Object file tags for imports and exports */



#ifndef SYMDEFS_H
#define SYMDEFS_H



/*****************************************************************************/
/*     	       	    		     Data				     */
/*****************************************************************************/



/* Number of module constructor/destructor declarations for an export */
#define SYM_CONDES_MASK	        0x07U

#define SYM_IS_CONDES(x)	(((x) & SYM_CONDES_MASK) != 0)
#define SYM_GET_CONDES_COUNT(x) ((x) & SYM_CONDES_MASK)
#define SYM_INC_CONDES_COUNT(x) ((x)++)

/* Symbol value type */
#define SYM_CONST     	        0x00U   /* Mask bit for const values */
#define SYM_EXPR      	        0x10U   /* Mask bit for expr values */
#define SYM_MASK_VAL  	        0x10U   /* Value mask */

#define SYM_IS_CONST(x)	        (((x) & SYM_MASK_VAL) == SYM_CONST)
#define SYM_IS_EXPR(x) 	        (((x) & SYM_MASK_VAL) == SYM_EXPR)

/* Symbol usage */
#define SYM_EQUATE              0x00U   /* Mask bit for an equate */
#define SYM_LABEL               0x20U   /* Mask bit for a label */
#define SYM_MASK_LABEL          0x20U   /* Value mask */

#define SYM_IS_EQUATE(x)        (((x) & SYM_MASK_LABEL) == SYM_EQUATE)
#define SYM_IS_LABEL(x)         (((x) & SYM_MASK_LABEL) == SYM_LABEL)



/* End of symdefs.h */

#endif
                


