/*****************************************************************************/
/*                                                                           */
/*				  codeinfo.h				     */
/*                                                                           */
/*		Additional information about code instructions		     */
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



#ifndef CODEINFO_H
#define CODEINFO_H



/*****************************************************************************/
/*  	       	 	  	     Data				     */
/*****************************************************************************/



/* Flags that tell what a specific instruction does with a register.
 * Please note that *changing* a register must not necessarily mean that the
 * value currently in the register is used. A prominent example is a load
 * instruction: It changes the register contents and does not use the old
 * value. On the flip side, a shift or something similar would use the
 * current value and change it.
 */
#define CI_USE_NONE	0x0000U		/* Use nothing */
#define CI_USE_A	0x0001U		/* Use the A register */
#define CI_USE_X	0x0002U		/* Use the X register */
#define CI_USE_Y	0x0004U		/* Use the Y register */
#define CI_MASK_USE    	0x000FU		/* Extract usage info */

#define CI_CHG_NONE	0x0000U		/* Change nothing */
#define CI_CHG_A	0x0010U		/* Change the A register */
#define CI_CHG_X	0x0020U		/* Change the X register */
#define CI_CHG_Y	0x0040U		/* Change the Y register */
#define CI_MASK_CHG	0x00F0U		/* Extract change info */
			     
#define CI_BRA		0x0100U		/* Instruction is a branch */
#define CI_MASK_BRA	0x0100U		/* Extract branch info */	

#define CI_NONE		0x0000U		/* Nothing used/changed */



/*****************************************************************************/
/*     	       	      	  	     Code				     */
/*****************************************************************************/



/* End of codeinfo.h */
#endif



