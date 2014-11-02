/*****************************************************************************/
/*                                                                           */
/*                                coptpush.h                                 */
/*                                                                           */
/*                          Optimize push sequences                          */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001-2002 Ullrich von Bassewitz                                       */
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



#ifndef COPTPUSH_H
#define COPTPUSH_H



/* cc65 */
#include "codeseg.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



unsigned OptPush1 (CodeSeg* S);
/* Given a sequence
**
**     ldy     #xx
**     jsr     ldaxysp
**     jsr     pushax
**
** If a/x are not used later, replace that by
**
**     ldy     #xx+2
**     jsr     pushwysp
**
** saving 3 bytes and several cycles.
*/

unsigned OptPush2 (CodeSeg* S);
/* A sequence
**
**     jsr     ldaxidx
**     jsr     pushax
**
** may get replaced by
**
**     jsr     pushwidx
*/



/* End of coptpush.h */

#endif
