/*****************************************************************************/
/*                                                                           */
/*                                 attrib.h                                  */
/*                                                                           */
/*                           Handle gcc attributes                           */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2012, Ullrich von Bassewitz                                      */
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



#ifndef ATTRIB_H
#define ATTRIB_H



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



#ifdef __clang__
#  define attribute(a)      __attribute__(a)
#  define ATTR_UNUSED(x)    __attribute__((__unused__)) x
#  define ATTR_NORETURN     __attribute__((analyzer_noreturn))
#elif defined(__GNUC__)
#  define attribute(a)      __attribute__(a)
#  define ATTR_UNUSED(x)    __attribute__((__unused__)) x
#  define ATTR_NORETURN     __attribute__((noreturn))
#else
#  define attribute(a)
#  define ATTR_UNUSED(x)    x
#  define ATTR_NORETURN
#endif

/* End of attrib.h */

#endif
