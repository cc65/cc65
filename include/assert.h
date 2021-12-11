/*****************************************************************************/
/*                                                                           */
/*                                 assert.h                                  */
/*                                                                           */
/*                                Diagnostics                                */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2015, Ullrich von Bassewitz                                      */
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



#ifndef _ASSERT_H
#define _ASSERT_H



#undef assert
#ifdef NDEBUG
#  define assert(expr)
#else
extern void __fastcall__ _afailed (const char*, unsigned);
#  define assert(expr)  ((expr)? (void)0 : _afailed(__FILE__, __LINE__))
#endif

/* TODO: Guard with #if __CC65_STD__ >= __CC65_STD_C11__ if there
** is a C11 mode.
*/
#if __CC65_STD__ > __CC65_STD_C99__
#  define static_assert _Static_assert
#endif



/* End of assert.h */
#endif
