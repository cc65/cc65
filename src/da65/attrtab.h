/*****************************************************************************/
/*                                                                           */
/*				   attrtab.h				     */
/*                                                                           */
/*			 Disassembler attribute table			     */
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



#ifndef ATTRTAB_H
#define ATTRTAB_H



/*****************************************************************************/
/*				     Data				     */
/*****************************************************************************/



typedef enum attr_t attr_t;
enum attr_t {
    atDefault	= 0x00,		/* Default style */
    atCode	= 0x01,
    atIllegal	= 0x02,
    atByteTab  	= 0x03,		/* Same as illegal */
    atWordTab	= 0x04,
    atDWordTab	= 0x05,
    atAddrTab	= 0x06,
    atRtsTab	= 0x07,

    atStyleMask = 0x0F		/* Output style */
};



/*****************************************************************************/
/*				     Code	   			     */
/*****************************************************************************/



void MarkRange (unsigned Start, unsigned End, attr_t Attr);
/* Mark a range with the given attribute */

void MarkAddr (unsigned Addr, attr_t Attr);
/* Mark an address with an attribute */

const char* MakeLabelName (unsigned Addr);
/* Make the default label name from the given address and return it in a
 * static buffer.
 */

void AddLabel (unsigned Addr, const char* Name);
/* Add a label */

int HaveLabel (unsigned Addr);
/* Check if there is a label for the given address */

const char* GetLabel (unsigned Addr);
/* Return the label for an address */

unsigned char GetStyle (unsigned Addr);
/* Return the style attribute for the given address */

void DefOutOfRangeLabels (void);
/* Output any labels that are out of the loaded code range */



/* End of attrtab.h */
#endif



