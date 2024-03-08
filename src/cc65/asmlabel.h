/*****************************************************************************/
/*                                                                           */
/*                                asmlabel.h                                 */
/*                                                                           */
/*                      Generate assembler code labels                       */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2009 Ullrich von Bassewitz                                       */
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



#ifndef ASMLABEL_H
#define ASMLABEL_H



/*****************************************************************************/
/*                                 Forwards                                  */
/*****************************************************************************/



struct SegContext;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void UseLabelPoolFromSegments (struct SegContext* Seg);
/* Use the info in segments for generating new label numbers */

unsigned GetLocalLabel (void);
/* Get an unused assembler label for the function. Will never return zero. */

const char* LocalLabelName (unsigned L);
/* Make a label name from the given label number. The label name will be
** created in static storage and overwritten when calling the function
** again.
*/

int IsLocalLabelName (const char* Name);
/* Return true if Name is the name of a local label */

unsigned GetLocalDataLabel (void);
/* Get an unused local data label. Will never return zero. */

const char* LocalDataLabelName (unsigned L);
/* Make a label name from the given data label number. The label name will be
** created in static storage and overwritten when calling the function again.
*/

unsigned GetPooledLiteralLabel (void);
/* Get an unused literal label. Will never return zero. */

const char* PooledLiteralLabelName (unsigned L);
/* Make a litral label name from the given label number. The label name will be
** created in static storage and overwritten when calling the function again.
*/



/* End of asmlabel.h */

#endif
