/*****************************************************************************/
/*                                                                           */
/*                                  conio.h                                  */
/*                                                                           */
/*                            Direct console I/O                             */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2007 Ullrich von Bassewitz                                       */
/*               Roemerstrasse 52                                            */
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



/*
** This is the direct console interface for cc65. I do not like the function
** names very much, but the first version started as a rewrite of Borland's
** conio, and, even if the interface has changed, the names did not.
**
** The interface does direct screen I/O, so it is fast enough for most
** programs. I did not implement text windows, since many applications do
** not need them and should not pay for the additional overhead. It should
** be easy to add text windows on a higher level if needed,
**
** Most routines do not check the parameters. This may be unfortunate but is
** also related to speed. The coordinates are always 0/0 based.
*/



#ifndef _CONIO_H
#define _CONIO_H



#include <stdarg.h>
#include <target.h>



/*****************************************************************************/
/*                                 Functions                                 */
/*****************************************************************************/



void clrscr (void);
/* Clear the whole screen and put the cursor into the top left corner */

unsigned char kbhit (void);
/* Return true if there's a key waiting, return false if not */

void __fastcall__ gotox (unsigned char x);
/* Set the cursor to the specified X position, leave the Y position untouched */

void __fastcall__ gotoy (unsigned char y);
/* Set the cursor to the specified Y position, leave the X position untouched */

void __fastcall__ gotoxy (unsigned char x, unsigned char y);
/* Set the cursor to the specified position */

unsigned char wherex (void);
/* Return the X position of the cursor */

unsigned char wherey (void);
/* Return the Y position of the cursor */

void __fastcall__ cputc (char c);
/* Output one character at the current cursor position */

void __fastcall__ cputcxy (unsigned char x, unsigned char y, char c);
/* Same as "gotoxy (x, y); cputc (c);" */

void __fastcall__ cputs (const char* s);
/* Output a NUL-terminated string at the current cursor position */

void __fastcall__ cputsxy (unsigned char x, unsigned char y, const char* s);
/* Same as "gotoxy (x, y); puts (s);" */

int cprintf (const char* format, ...);
/* Like printf(), but uses direct screen output */

int __fastcall__ vcprintf (const char* format, va_list ap);
/* Like vprintf(), but uses direct screen output */

char cgetc (void);
/* Return a character from the keyboard. If there is no character available,
** the function waits until the user does press a key. If cursor is set to
** 1 (see below), a blinking cursor is displayed while waiting.
*/

char* __fastcall__ cgets (char* buffer, int size);
/* Get a string of characters directly from the console. The function returns
** when size - 1 characters or either CR/LF are read. Note the parameters are
** more aligned with stdio fgets() as opposed to the quirky "standard" conio
** cgets(). Besides providing saner parameters, the function also echoes CRLF
** when either CR/LF are read but does NOT append either in the buffer. This is
** to correspond to stdio fgets() which echoes CRLF, but prevents a "gotcha"
** where the buffer might not be able to accommodate both CR and LF at the end.
**
**   param: buffer - where to save the input, must be non-NULL
**   param: size - size of the buffer, must be > 1
**  return: buffer if successful, NULL on error
**  author: Russell-S-Harper
*/

int cscanf (const char* format, ...);
/* Like scanf(), but uses direct keyboard input */

int __fastcall__ vcscanf (const char* format, va_list ap);
/* Like vscanf(), but uses direct keyboard input */

char cpeekc (void);
/* Return the character from the current cursor position */

unsigned char cpeekcolor (void);
/* Return the color from the current cursor position */

unsigned char cpeekrevers (void);
/* Return the reverse attribute from the current cursor position.
** If the character is reversed, then return 1; return 0 otherwise.
*/

void __fastcall__ cpeeks (char* s, unsigned int length);
/* Return a string of the characters that start at the current cursor position.
** Put the string into the buffer to which "s" points.  The string will have
** "length" characters, then will be '\0'-terminated.
*/

unsigned char __fastcall__ cursor (unsigned char onoff);
/* If onoff is 1, a cursor is displayed when waiting for keyboard input. If
** onoff is 0, the cursor is hidden when waiting for keyboard input. The
** function returns the old cursor setting.
*/

unsigned char __fastcall__ revers (unsigned char onoff);
/* Enable/disable reverse character display. This may not be supported by
** the output device. Return the old setting.
*/

unsigned char __fastcall__ textcolor (unsigned char color);
/* Set the color for text output. The old color setting is returned. */

unsigned char __fastcall__ bgcolor (unsigned char color);
/* Set the color for the background. The old color setting is returned. */

unsigned char __fastcall__ bordercolor (unsigned char color);
/* Set the color for the border. The old color setting is returned. */

void __fastcall__ chline (unsigned char length);
/* Output a horizontal line with the given length starting at the current
** cursor position.
*/

void __fastcall__ chlinexy (unsigned char x, unsigned char y, unsigned char length);
/* Same as "gotoxy (x, y); chline (length);" */

void __fastcall__ cvline (unsigned char length);
/* Output a vertical line with the given length at the current cursor
** position.
*/

void __fastcall__ cvlinexy (unsigned char x, unsigned char y, unsigned char length);
/* Same as "gotoxy (x, y); cvline (length);" */

void __fastcall__ cclear (unsigned char length);
/* Clear part of a line (write length spaces). */

void __fastcall__ cclearxy (unsigned char x, unsigned char y, unsigned char length);
/* Same as "gotoxy (x, y); cclear (length);" */

void __fastcall__ screensize (unsigned char* x, unsigned char* y);
/* Return the current screen size. */

void __fastcall__ cputhex8 (unsigned char val);
void __fastcall__ cputhex16 (unsigned val);
/* These shouldn't be here... */



/*****************************************************************************/
/*                                  Macros                                   */
/*****************************************************************************/



/* On some platforms, functions are not available or are dummys. To suppress
** the call to these functions completely, the platform header files may
** define macros for these functions that start with an underline. If such a
** macro exists, a new macro is defined here, that expands to the one with the
** underline. The reason for this two stepped approach is that it is sometimes
** necessary to take the address of the function, which is not possible when
** using a macro. Since the function prototype is still present, #undefining
** the macro will give access to the actual function.
*/

#ifdef _textcolor
#  define textcolor(color)      _textcolor(color)
#endif
#ifdef _bgcolor
#  define bgcolor(color)        _bgcolor(color)
#endif
#ifdef _bordercolor
#  define bordercolor(color)    _bordercolor(color)
#endif
#ifdef _cpeekcolor
#  define cpeekcolor()          _cpeekcolor()
#endif
#ifdef _cpeekrevers
#  define cpeekrevers()         _cpeekrevers()
#endif

#ifdef _chline
#  define chline(len)      _chline(len)
#endif
#ifdef _cvline
#  define cvline(len)      _cvline(len)
#endif
#ifdef _chlinexy
#  define chlinexy(x, y, len)      _chlinexy(x, y, len)
#endif
#ifdef _cvlinexy
#  define cvlinexy(x, y, len)      _cvlinexy(x, y, len)
#endif

/* End of conio.h */
#endif
