/*****************************************************************************/
/*                                                                           */
/*                                  spool.h                                  */
/*                                                                           */
/*                  Id and message pool for the ld65 linker                  */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2003      Ullrich von Bassewitz                                       */
/*               Römerstrasse 52                                             */
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



#ifndef SPOOL_H
#define SPOOL_H



/* common */
#include "strpool.h"



/*****************************************************************************/
/*     	       	     	      	     Data				     */
/*****************************************************************************/



extern StringPool StrPool;



/*****************************************************************************/
/*   	       		      	     Code				     */
/*****************************************************************************/



#if defined(HAVE_INLINE)
INLINE unsigned GetStringId (const char* S)
/* Return the id of the given string */
{
    return SP_Add (&StrPool, S);
}
#else
#  define GetStringId(S)        SP_Add (&StrPool, (S))
#endif



/* End of spool.h */

#endif




