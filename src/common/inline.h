/*****************************************************************************/
/*                                                                           */
/*                                 inline.h                                  */
/*                                                                           */
/*              Definitions to use the inline compiler feature               */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001-2005  Ullrich von Bassewitz                                      */
/*                Römerstraße 52                                             */
/*                D-70794 Filderstadt                                        */
/* EMail:         uz@cc65.org                                                */
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



#ifndef INLINE_H
#define INLINE_H



/*****************************************************************************/
/*                                  Defines                                  */
/*****************************************************************************/



#if defined(__GNUC__) && !defined(DISABLE_INLINE)
#  define HAVE_INLINE   1
#  define INLINE        static __inline__
#endif



/* End of inline.h */

#endif
