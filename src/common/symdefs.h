/*****************************************************************************/
/*                                                                           */
/*				   symdefs.h   	       	       	       	     */
/*                                                                           */
/*		 Symbol definitions for the bin65 binary utils		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998     Ullrich von Bassewitz                                        */
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



/* Object file tags for imports and exports */



#ifndef SYMDEFS_H
#define SYMDEFS_H



/*****************************************************************************/
/*     	       	    		     Data				     */
/*****************************************************************************/



/* Import size */
#define IMP_ABS      	0x00   	       	/* Import as normal value */
#define IMP_ZP 	     	0x01		/* Import as zero page symbol */
#define IMP_MASK_SIZE	0x01		/* Size mask */

#define IS_IMP_ABS(x)	(((x) & IMP_MASK_SIZE) == IMP_ABS)
#define IS_IMP_ZP(x)	(((x) & IMP_MASK_SIZE) == IMP_ZP)

/* Export size */
#define EXP_ABS	     	0x00		/* Export as normal value */
#define EXP_ZP 	     	0x01		/* Export as zero page value */
#define EXP_MASK_SIZE	0x01		/* Size mask */

#define IS_EXP_ABS(x)  	(((x) & EXP_MASK_SIZE) == EXP_ABS)
#define IS_EXP_ZP(x)	(((x) & EXP_MASK_SIZE) == EXP_ZP)

/* Export value type */
#define EXP_CONST    	0x00		/* Mask bit for const values */
#define EXP_EXPR     	0x02   	 	/* Mask bit for expr values */
#define EXP_MASK_VAL 	0x02		/* Value mask */

#define IS_EXP_CONST(x)	(((x) & EXP_MASK_VAL) == EXP_CONST)
#define IS_EXP_EXPR(x) 	(((x) & EXP_MASK_VAL) == EXP_EXPR)

/* Export initializer flag */
#define EXP_INIT	0x04   	   	/* Mask bit for initializer export */
#define EXP_MASK_INIT	0x04		/* Value mask */

#define IS_EXP_INIT(x)	(((x) & EXP_MASK_INIT) == EXP_INIT)



/* End of symdefs.h */

#endif



