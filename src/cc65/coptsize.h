/*****************************************************************************/
/*                                                                           */
/*                                 coptsize.c                                */
/*                                                                           */
/*                              Size optimizations                           */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2002      Ullrich von Bassewitz                                       */
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



#ifndef COPTSIZE_H
#define COPTSIZE_H



/* cc65 */
#include "codeseg.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



unsigned OptSize1 (CodeSeg* S);
/* Do size optimization by calling special subroutines that preload registers.
** This routine does not work standalone, it needs a following register load
** removal pass.
*/

unsigned OptSize2 (CodeSeg* S);
/* Do size optimization by using shorter code sequences, even if this
** introduces relations between instructions. This step must be one of the
** last steps, because it makes further work much more difficult.
*/



/* End of coptsize.h */

#endif
