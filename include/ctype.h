/*
 * ctype.h
 *
 * Ullrich von Bassewitz, 03.06.1998
 *
 */



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

#define isalnum(c)  (__AX__ = (c), __asm__ ("\ttay\n\tlda\t__ctype,y\n\tand\t#$07"), __AX__)
#define isalpha(c)  (__AX__ = (c), __asm__ ("\ttay\n\tlda\t__ctype,y\n\tand\t#$03"), __AX__)
#define iscntrl(c)  (__AX__ = (c), __asm__ ("\ttay\n\tlda\t__ctype,y\n\tand\t#$10"), __AX__)
#define isdigit(c)  (__AX__ = (c), __asm__ ("\ttay\n\tlda\t__ctype,y\n\tand\t#$04"), __AX__)
#define isgraph(c)  (__AX__ = (c), __asm__ ("\ttay\n\tlda\t__ctype,y\n\teor\t#$30\n\tand\t#$30"), __AX__)
#define islower(c)  (__AX__ = (c), __asm__ ("\ttay\n\tlda\t__ctype,y\n\tand\t#$01"), __AX__)
#define isprint(c)  (__AX__ = (c), __asm__ ("\ttay\n\tlda\t__ctype,y\n\teor\t#$10\n\tand\t#$10"), __AX__)
#define ispunct(c)  (__AX__ = (c), __asm__ ("\ttay\n\tlda\t__ctype,y\n\teor\t#$37\n\tand\t#$37"), __AX__)
#define isspace(c)  (__AX__ = (c), __asm__ ("\ttay\n\tlda\t__ctype,y\n\tand\t#$60"), __AX__)
#define isupper(c)  (__AX__ = (c), __asm__ ("\ttay\n\tlda\t__ctype,y\n\tand\t#$02"), __AX__)
#define isxdigit(c) (__AX__ = (c), __asm__ ("\ttay\n\tlda\t__ctype,y\n\tand\t#$08"), __AX__)

#ifndef __STRICT_ANSI__
/* cc65 and GNU extension */
#define isblank(c)  (__AX__ = (c), __asm__ ("\ttay\n\tlda\t__ctype,y\n\tand\t#$80"), __AX__)
#endif


#endif



/* End of ctype.h */
#endif



