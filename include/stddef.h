/*****************************************************************************/
/*                                                                           */
/*				   stddef.h				     */
/*                                                                           */
/*			      Common definitions			     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2000 Ullrich von Bassewitz                                       */
/*               Wacholderweg 14                                             */
/*               D-70597 Stuttgart                                           */
/* EMail:        uz@musoftware.de                                            */
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



#ifndef _STDDEF_H
#define _STDDEF_H



/* Standard data types */
#ifndef _PTRDIFF_T
#define _PTRDIFF_T
typedef int ptrdiff_t;
#endif
#ifndef _WCHAR_T
#define _WCHAR_T
typedef char wchar_t;
#endif
#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned size_t;
#endif

/* NULL pointer */
#ifdef NULL
#  undef NULL
#endif
#define NULL	0

/* offsetof macro */
#define offsetof(type, member)	(size_t) (&((type*) 0)->member)



/* End of stddef.h */
#endif




