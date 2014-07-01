/*****************************************************************************/
/*                                                                           */
/*                                  ctype.h                                  */
/*                                                                           */
/*                            Character handling                             */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2013, Ullrich von Bassewitz                                      */
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



#ifndef _CTYPE_H
#define _CTYPE_H


/* The array containing character classification data */
extern unsigned char _ctype[256];

/* Bits used to specify character classes */
#define _CT_LOWER       0x01    /* 0 - Lower case char */
#define _CT_UPPER       0x02    /* 1 - Upper case char */
#define _CT_DIGIT       0x04    /* 2 - Numeric digit */
#define _CT_XDIGIT      0x08    /* 3 - Hex digit (both lower and upper) */
#define _CT_CNTRL       0x10    /* 4 - Control character */
#define _CT_SPACE       0x20    /* 5 - The space character itself */
#define _CT_OTHER_WS    0x40    /* 6 - Other whitespace ('\f', '\n', '\r', '\t', and '\v') */
#define _CT_SPACE_TAB   0x80    /* 7 - Space or tab character */

/* Bit combinations */
#define _CT_ALNUM       (_CT_LOWER | _CT_UPPER | _CT_DIGIT)
#define _CT_ALPHA       (_CT_LOWER | _CT_UPPER)
#define _CT_NOT_GRAPH   (_CT_CNTRL | _CT_SPACE)
#define _CT_NOT_PRINT   (_CT_CNTRL)
#define _CT_NOT_PUNCT   (_CT_SPACE | _CT_CNTRL | _CT_DIGIT | _CT_UPPER | _CT_LOWER)
#define _CT_WS          (_CT_SPACE | _CT_OTHER_WS)

/* Character classification functions */
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
#if __CC65_STD__ >= __CC65_STD_C99__
int __fastcall__ isblank (int c);       /* New in C99 */
#endif

int __fastcall__ toupper (int c);       /* Always external */
int __fastcall__ tolower (int c);       /* Always external */

#if __CC65_STD__ >= __CC65_STD_CC65__
unsigned char __fastcall__ toascii (unsigned char c);
/* Convert a target-specific character to ASCII. */
#endif



/* When inlining-of-known-functions is enabled, overload most of the above
** functions by macroes. The function prototypes are available again after
** #undef'ing the macroes.
** Please note that the following macroes do NOT handle EOF correctly, as
** stated in the manual. If you need correct behaviour for EOF, don't
** use -Os, or #undefine the following macroes.
*/
#ifdef __OPT_s__

#define isalnum(c)  (__AX__ = (c),                      \
                    __asm__ ("tay"),                    \
                    __asm__ ("lda %v,y", _ctype),       \
                    __asm__ ("and #%b", _CT_ALNUM),     \
                    __AX__)

#define isalpha(c)  (__AX__ = (c),                      \
                    __asm__ ("tay"),                    \
                    __asm__ ("lda %v,y", _ctype),       \
                    __asm__ ("and #%b", _CT_ALPHA),     \
                    __AX__)

#if __CC65_STD__ >= __CC65_STD_C99__
#define isblank(c)  (__AX__ = (c),                      \
                    __asm__ ("tay"),                    \
                    __asm__ ("lda %v,y", _ctype),       \
                    __asm__ ("and #%b", _CT_SPACE_TAB), \
                    __AX__)
#endif

#define iscntrl(c)  (__AX__ = (c),                      \
                    __asm__ ("tay"),                    \
                    __asm__ ("lda %v,y", _ctype),       \
                    __asm__ ("and #%b", _CT_CNTRL),     \
                    __AX__)

#define isdigit(c)  (__AX__ = (c),                      \
                    __asm__ ("tay"),                    \
                    __asm__ ("lda %v,y", _ctype),       \
                    __asm__ ("and #%b", _CT_DIGIT),     \
                    __AX__)

#define isgraph(c)  (__AX__ = (c),                      \
                    __asm__ ("tay"),                    \
                    __asm__ ("lda %v,y", _ctype),       \
                    __asm__ ("and #%b", _CT_NOT_GRAPH), \
                    __asm__ ("cmp #1"),                 \
                    __asm__ ("lda #1"),                 \
                    __asm__ ("sbc #1"),                 \
                    __AX__)

#define islower(c)  (__AX__ = (c),                      \
                    __asm__ ("tay"),                    \
                    __asm__ ("lda %v,y", _ctype),       \
                    __asm__ ("and #%b", _CT_LOWER),     \
                    __AX__)

#define isprint(c)  (__AX__ = (c),                      \
                    __asm__ ("tay"),                    \
                    __asm__ ("lda %v,y", _ctype),       \
                    __asm__ ("and #%b", _CT_NOT_PRINT), \
                    __asm__ ("eor #%b", _CT_NOT_PRINT), \
                    __AX__)

#define ispunct(c)  (__AX__ = (c),                      \
                    __asm__ ("tay"),                    \
                    __asm__ ("lda %v,y", _ctype),       \
                    __asm__ ("and #%b", _CT_NOT_PUNCT), \
                    __asm__ ("cmp #1"),                 \
                    __asm__ ("lda #1"),                 \
                    __asm__ ("sbc #1"),                 \
                    __AX__)

#define isspace(c)  (__AX__ = (c),                      \
                    __asm__ ("tay"),                    \
                    __asm__ ("lda %v,y", _ctype),       \
                    __asm__ ("and #%b", _CT_WS),        \
                    __AX__)

#define isupper(c)  (__AX__ = (c),                      \
                    __asm__ ("tay"),                    \
                    __asm__ ("lda %v,y", _ctype),       \
                    __asm__ ("and #%b", _CT_UPPER),     \
                    __AX__)

#define isxdigit(c) (__AX__ = (c),                      \
                    __asm__ ("tay"),                    \
                    __asm__ ("lda %v,y", _ctype),       \
                    __asm__ ("and #%b", _CT_XDIGIT),    \
                    __AX__)

#endif



/* End of ctype.h */
#endif



