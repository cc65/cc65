/*****************************************************************************/
/*                                                                           */
/*				    ctype.h				     */
/*                                                                           */
/*			      Character handling			     */
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



#ifndef _CTYPE_H
#define _CTYPE_H



int __fastcall__ isalnum (int c);
int __fastcall__ isalpha (int c);
int __fastcall__ iscntrl (int c);
int __fastcall__ isdigit (int c);
int __fastcall__ isgraph (int c);
int __fastcall__ islower (int c);
int __fastcall__ isprint (int c);
int __fastcall__ ispunct (int c);
int __fastcall__ isspace (int c);
int __fastcall__ isupper (int c);
int __fastcall__ isxdigit (int c);
#ifndef __STRICT_ANSI__
int __fastcall__ isblank (int c); 	/* cc65 (and GNU) extension */
#endif

int __fastcall__ toupper (int c);	/* Always external */
int __fastcall__ tolower (int c);	/* Always external */



/* When inlining of known function is enabled, overload most of the above
 * functions by macros. The function prototypes are again available after
 * #undef'ing the macros.
*/
#ifdef __OPT_s__


extern unsigned char _ctype[256];

#define isalnum(c)  (__AX__ = (c), __asm__ ("tay\n lda __ctype,y\n and #$07"), __AX__)
#define isalpha(c)  (__AX__ = (c), __asm__ ("tay\n lda __ctype,y\n and #$03"), __AX__)
#define isblank(c)  (__AX__ = (c), __asm__ ("tay\n lda __ctype,y\n and #$80"), __AX__)
#define iscntrl(c)  (__AX__ = (c), __asm__ ("tay\n lda __ctype,y\n and #$10"), __AX__)
#define isdigit(c)  (__AX__ = (c), __asm__ ("tay\n lda __ctype,y\n and #$04"), __AX__)
#define isgraph(c)  (__AX__ = (c), __asm__ ("tay\n lda __ctype,y\n eor #$30\n and #$30"), __AX__)
#define islower(c)  (__AX__ = (c), __asm__ ("tay\n lda __ctype,y\n and #$01"), __AX__)
#define isprint(c)  (__AX__ = (c), __asm__ ("tay\n lda __ctype,y\n eor #$10\n and #$10"), __AX__)
#define ispunct(c)  (__AX__ = (c), __asm__ ("tay\n lda __ctype,y\n eor #$37\n and #$37"), __AX__)
#define isspace(c)  (__AX__ = (c), __asm__ ("tay\n lda __ctype,y\n and #$60"), __AX__)
#define isupper(c)  (__AX__ = (c), __asm__ ("tay\n lda __ctype,y\n and #$02"), __AX__)
#define isxdigit(c) (__AX__ = (c), __asm__ ("tay\n lda __ctype,y\n and #$08"), __AX__)



#endif



/* End of ctype.h */
#endif



