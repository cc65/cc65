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
/*  	       	 	  	     Data				     */
/*****************************************************************************/



/* Defines for registers. */
#define REG_NONE	0x00U
#define REG_A		0x01U
#define REG_X		0x02U
#define REG_Y		0x04U
#define REG_SREG_LO	0x08U
#define REG_SREG_HI	0x10U
#define REG_TMP1	0x20U
#define REG_PTR1_LO	0x40U
#define REG_PTR1_HI	0x80U
#define	REG_AX		(REG_A | REG_X)
#define REG_XY		(REG_X | REG_Y)
#define REG_AXY		(REG_A | REG_X | REG_Y)



/*****************************************************************************/
/*     	       	      	  	     Code				     */
/*****************************************************************************/



void GetFuncInfo (const char* Name, unsigned char* Use, unsigned char* Chg);
/* For the given function, lookup register information and store it into
 * the given variables. If the function is unknown, assume it will use and
 * load all registers.
 */

int IsZPName (const char* Name);
/* Return true if the given name is a zero page symbol */

unsigned char GetRegInfo (struct CodeSeg* S, unsigned Index);
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




