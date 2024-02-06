/*****************************************************************************/
/*                                                                           */
/*                                hashfunc.h                                 */
/*                                                                           */
/*                              Hash functions                               */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2011, Ullrich von Bassewitz                                      */
/*                Roemerstrasse 52                                           */
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



#ifndef HASHFUNC_H
#define HASHFUNC_H



/* common */
#include "attrib.h"
#include "strbuf.h"



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



unsigned HashInt (unsigned V) attribute ((const));
/* Return a hash value for the given integer. */

unsigned HashStr (const char* S) attribute ((const));
/* Return a hash value for the given string */

unsigned HashBuf (const StrBuf* S) attribute ((const));
/* Return a hash value for the given string buffer */



/* End of hashfunc.h */

#endif
