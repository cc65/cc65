/*****************************************************************************/
/*                                                                           */
/*				   stdarg.h				     */
/*                                                                           */
/*			      Variable arguments			     */
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



#ifndef _STDARG_H
#define _STDARG_H



typedef unsigned char* va_list;

#define va_start(ap, fix)      	ap = (va_list)&fix + *(((va_list)&fix)-1) - __fixargs__
#define va_arg(ap,type)	       	((type)*(ap -= ((sizeof (type) + 1) & ~1)))
#define va_copy(dest, src)	((dest)=(src))
#define va_end(ap)

/* This is only valid *before* the first call to va_arg. It will also work
 * only for int sized parameters.
 */
#define va_fix(ap, offs)	*(ap+(__fixargs__-2*offs))



/* End of stdarg.h */
#endif




