/*****************************************************************************/
/*                                                                           */
/*                                  target.h                                 */
/*                                                                           */
/*                        Target specific definitions                        */
/*                                                                           */
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



#ifndef _TARGET_H
#define _TARGET_H



/* Include the correct target specific file */
#if   defined(__APPLE2ENH__)
#  include <apple2enh.h>
#elif defined(__APPLE2__)
#  include <apple2.h>
#elif defined(__ATARI__)
#  include <atari.h>
#elif defined(__ATARI2600__)
#  include <atari2600.h>
#elif defined(__ATARI5200__)
#  include <atari5200.h>
#elif defined(__ATMOS__)
#  include <atmos.h>
#elif defined(__CBM__)
#  include <cbm.h>
#elif defined(__CREATIVISION__)
#  include <creativision.h>
#elif defined(__GAMATE__)
#  include <gamate.h>
#elif defined(__GEOS__)
#  include <geos.h>
#elif defined(__LYNX__)
#  include <lynx.h>
#elif defined(__NES__)
#  include <nes.h>
#elif defined(__OSIC1P__)
#  include <osic1p.h>
#elif defined(__PCE__)
#  include <pce.h>
#elif defined(__SUPERVISION__)
#  include <supervision.h>
#elif defined(__TELESTRAT__)
#  include <telestrat.h>
#endif



/* End of target.h */
#endif
