/*****************************************************************************/
/*                                                                           */
/*                                 palconv.h                                 */
/*                                                                           */
/*      Color palette conversions for the sp65 sprite and bitmap utility     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2022,      Karri Kaksonen                                             */
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



#ifndef PALCONV_H
#define PALCONV_H



#include <stdio.h>

/* common */
#include "coll.h"
#include "strbuf.h"

/* sp65 */
#include "bitmap.h"


/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/

StrBuf* PaletteTo (const Bitmap* B, const Collection* A);
/* Convert the palette of bitmap B into some sort of other binary format.
** The output is stored in a string buffer (which is actually a dynamic char
** array) and returned. The actual output format is taken from the "target"
** attribute in the attribute collection A.
*/

void ListPaletteTargets (FILE* F);
/* Output a list of palette targets */

/* End of palette.h */

#endif



