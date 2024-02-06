/*****************************************************************************/
/*                                                                           */
/*                                 labels.h                                  */
/*                                                                           */
/*                         Label management for da65                         */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2006      Ullrich von Bassewitz                                       */
/*               Roemerstrasse 52                                            */
/*               D-70794 Filderstadt                                         */
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



#ifndef LABELS_H
#define LABELS_H



#include "attrtab.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void AddIntLabel (unsigned Addr);
/* Add an internal label using the address to generate the name. */

void AddExtLabel (unsigned Addr, const char* Name);
/* Add an external label */

void AddUnnamedLabel (unsigned Addr);
/* Add an unnamed label */

void AddDepLabel (unsigned Addr, attr_t Attr, const char* BaseName, unsigned Offs);
/* Add a dependent label at the given address using "base name+Offs" as the new
** name.
*/

void AddIntLabelRange (unsigned Addr, const char* Name, unsigned Count);
/* Add an internal label for a range. The first entry gets the label "Name"
** while the others get "Name+offs".
*/

void AddExtLabelRange (unsigned Addr, const char* Name, unsigned Count);
/* Add an external label for a range. The first entry gets the label "Name"
** while the others get "Name+offs".
*/

int HaveLabel (unsigned Addr);
/* Check if there is a label for the given address */

int MustDefLabel (unsigned Addr);
/* Return true if we must define a label for this address, that is, if there
** is a label at this address, and it is an external or internal label.
*/

const char* GetLabelName (unsigned Addr);
/* Return the label name for an address */

const char* GetLabel (unsigned Addr, unsigned RefFrom);
/* Return the label name for an address, as it is used in a label reference.
** RefFrom is the address the label is referenced from. This is needed in case
** of unnamed labels, to determine the name.
*/

void ForwardLabel (unsigned Offs);
/* If necessary, output a forward label, one that is within the next few
** bytes and is therefore output as "label = * + x".
*/

void DefOutOfRangeLabels (void);
/* Output any labels that are out of the loaded code range */



/* End of labels.h */

#endif
