/*****************************************************************************/
/*                                                                           */
/*				  codeinfo.h				     */
/*                                                                           */
/*		    Additional information about 6502 code		     */
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



#ifndef CODEINFO_H
#define CODEINFO_H



/*****************************************************************************/
/*				   Forwards				     */
/*****************************************************************************/



struct CodeSeg;



/*****************************************************************************/
/*  	       	 	  	     Data			       	     */
/*****************************************************************************/



/* Defines for registers. */
#define REG_NONE       	0x0000U
#define REG_A  	       	0x0001U
#define REG_X  	       	0x0002U
#define REG_Y  	       	0x0004U
#define REG_SREG_LO    	0x0008U
#define REG_SREG_HI    	0x0010U
#define REG_TMP1       	0x0020U
#define REG_TMP2        0x0040U
#define REG_TMP3        0x0080U
#define REG_PTR1_LO    	0x0100U
#define REG_PTR1_HI    	0x0200U
#define REG_PTR2_LO    	0x0400U
#define REG_PTR2_HI    	0x0800U
#define REG_PTR3_LO    	0x1000U
#define REG_PTR3_HI    	0x2000U
#define REG_PTR4_LO    	0x4000U
#define REG_PTR4_HI    	0x8000U
#define	REG_AX		(REG_A | REG_X)
#define REG_SREG        (REG_SREG_LO | REG_SREG_HI)
#define REG_EAX         (REG_AX | REG_SREG)
#define REG_XY		(REG_X | REG_Y)
#define REG_AXY		(REG_AX | REG_Y)
#define REG_EAXY        (REG_EAX | REG_Y)
#define REG_PTR1        (REG_PTR1_LO | REG_PTR1_HI)
#define REG_PTR2        (REG_PTR2_LO | REG_PTR2_HI)
#define REG_PTR3        (REG_PTR3_LO | REG_PTR3_HI)
#define REG_PTR4        (REG_PTR4_LO | REG_PTR4_HI)
#define REG_ALL         0xFFFFU


/*****************************************************************************/
/*     	       	      	  	     Code			       	     */
/*****************************************************************************/



void GetFuncInfo (const char* Name, unsigned short* Use, unsigned short* Chg);
/* For the given function, lookup register information and store it into
 * the given variables. If the function is unknown, assume it will use and
 * load all registers.
 */

int IsZPName (const char* Name, unsigned short* RegInfo);
/* Return true if the given name is a zero page symbol. If the RegInfo
 * pointer is not NULL, it is filled with the register info for the
 * zero page location found.
 */

unsigned GetRegInfo (struct CodeSeg* S, unsigned Index);
/* Determine register usage information for the instructions starting at the
 * given index.
 */

int RegAUsed (struct CodeSeg* S, unsigned Index);
/* Check if the value in A is used. */

int RegXUsed (struct CodeSeg* S, unsigned Index);
/* Check if the value in X is used. */

int RegYUsed (struct CodeSeg* S, unsigned Index);
/* Check if the value in Y is used. */



/* End of codeinfo.h */
#endif




