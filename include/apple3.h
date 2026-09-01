/*****************************************************************************/
/*                                                                           */
/*                                 apple3.h                                  */
/*                                                                           */
/*                   Apple /// system specific definitions                   */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* Robert Justice, 2026                                                      */
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



#ifndef _APPLE3_H
#define _APPLE3_H



/* Check for errors */
#if !defined(__APPLE3__)
#  error This module may only be used when compiling for the Apple \/\/\/!
#endif

#include <time.h>
#include <apple3_filetype.h>



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Color defines */
#define COLOR_BLACK       0x00
#define COLOR_MAGENTA     0x01
#define COLOR_DARKBLUE    0x02
#define COLOR_PURPLE      0x03
#define COLOR_DARKGREEN   0x04
#define COLOR_GRAY1       0x05
#define COLOR_MEDBLUE     0x06
#define COLOR_LIGHTBLUE   0x07
#define COLOR_BROWN       0x08
#define COLOR_ORANGE      0x09
#define COLOR_GRAY2       0x0A
#define COLOR_PINK        0x0B
#define COLOR_GREEN       0x0C
#define COLOR_YELLOW      0x0D
#define COLOR_AQUA        0x0E
#define COLOR_WHITE       0x0F


/* Characters codes */
#define CH_ENTER        0x0D
#define CH_ESC          0x1B
#define CH_CURS_LEFT    0x08
#define CH_CURS_RIGHT   0x15

#define CH_DEL          0x7F
#define CH_CURS_UP      0x0B
#define CH_CURS_DOWN    0x0A

/* These are defined to be OpenApple + NumberKey */
#define CH_F1   0xB1
#define CH_F2   0xB2
#define CH_F3   0xB3
#define CH_F4   0xB4
#define CH_F5   0xB5
#define CH_F6   0xB6
#define CH_F7   0xB7
#define CH_F8   0xB8
#define CH_F9   0xB9
#define CH_F10  0xB0


/* Functions that we'll use to go with either Normal or MouseText font to draw boxes and lines */
void dyn_chline (unsigned char h, unsigned char length);
void dyn_cvline (unsigned char v, unsigned char length);
void dyn_chlinexy (unsigned char h, unsigned char x, unsigned char y, unsigned char length);
void dyn_cvlinexy (unsigned char v, unsigned char x, unsigned char y, unsigned char length);

#if defined(DYN_BOX_DRAW)
/* When the user defines DYN_BOX_DRAW, we'll assume they have the mousetext font loaded in the first 32 chars */

#define CH_HLINE        '_'
#define CH_VLINE        0x9F
#define CH_ULCORNER     '_'
#define CH_URCORNER     0x20
#define CH_LLCORNER     0x94
#define CH_LRCORNER     0x9F
#define CH_TTEE         '_'
#define CH_BTEE         0x94
#define CH_LTEE         0x94
#define CH_RTEE         0x9F
#define CH_CROSS        0x94

#else
/* Otherwise, fallback to safety and don't use MouseText at all. */

#define CH_HLINE        '-'
#define CH_VLINE        '|'
#define CH_ULCORNER     '+'
#define CH_URCORNER     '+'
#define CH_LLCORNER     '+'
#define CH_LRCORNER     '+'
#define CH_TTEE         '+'
#define CH_BTEE         '+'
#define CH_LTEE         '+'
#define CH_RTEE         '+'
#define CH_CROSS        '+'

#endif /* DYN_BOX_DRAW */

#define _chline(length)         dyn_chline(CH_HLINE, length)
#define _chlinexy(x, y, length) dyn_chlinexy(CH_HLINE, x ,y, length)
#define _cvline(length)         dyn_cvline(CH_VLINE, length)
#define _cvlinexy(x, y, length) dyn_cvlinexy(CH_VLINE, x, y, length)


/* Masks for joy_read */
#define JOY_UP_MASK     0x10
#define JOY_DOWN_MASK   0x20
#define JOY_LEFT_MASK   0x04
#define JOY_RIGHT_MASK  0x08
#define JOY_BTN_1_MASK  0x40
#define JOY_BTN_2_MASK  0x80


/* Video modes */
#define VIDEOMODE_40x24     0x00   //B&W
#define VIDEOMODE_40x24C    0x01   //Color
#define VIDEOMODE_80x24     0x02   //B&W
#define VIDEOMODE_40COL     VIDEOMODE_40x24
#define VIDEOMODE_80COL     VIDEOMODE_80x24


/* struct stat.st_mode values */
#define S_IFDIR  0x01
#define S_IFREG  0x02
#define S_IFBLK  0xFF
#define S_IFCHR  0xFF
#define S_IFIFO  0xFF
#define S_IFLNK  0xFF
#define S_IFSOCK 0xFF

struct datetime {
    struct {
        unsigned day  :5;
        unsigned mon  :4;
        unsigned year :7;
    }                 date;
    struct {
        unsigned char min;
        unsigned char hour;
    }                 time;
};



/*****************************************************************************/
/*                                 Variables                                 */
/*****************************************************************************/



/* The file stream implementation and the POSIX I/O functions will use the
** following struct to set the date and time stamp on files. This specifically
** applies to the open and fopen functions.
*/
extern struct datetime _datetime;

/* The addresses of the static drivers */
extern void a3_stdjoy_joy[];     /* Referred to by joy_static_stddrv[]   */



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void beep (void);
/* Beep beep. */

int __fastcall__ file_set_type(const char *pathname, unsigned char type);
/* Sets the ProDOS type for the file, returns 0 on success, sets errno on failure */

int __fastcall__ file_set_auxtype(const char *pathname, unsigned int auxtype);
/* Sets the ProDOS auxtype for the file, returns 0 on success, sets errno on failure */

void rebootafterexit (void);
/* Reboot machine after program termination has completed. */


/* The following #defines will cause the matching functions calls in conio.h
** to be overlaid by macros with the same names, saving the function call
** overhead.
*/
#define _cpeekcolor()           COLOR_WHITE
#define _cpeekrevers()          0

//struct tm* __fastcall__ gmtime_dt (const struct datetime* dt);
/* Converts a ProDOS date/time structure to a struct tm */

//time_t __fastcall__ mktime_dt (const struct datetime* dt);
/* Converts a ProDOS date/time structure to a time_t UNIX timestamp */

typedef struct DIR DIR;

unsigned int __fastcall__ dir_entry_count(DIR *dir);
/* Returns the number of active files in a ProDOS directory */

signed char __fastcall__ videomode (unsigned mode);
/* Set the video mode, return the old mode installed.
** Call with one of the VIDEOMODE_xx constants.
*/

void waitvsync (void);
/* Wait for start of next frame */

/* peep/poke supporting extended addressing to access full Apple /// memory */
unsigned char xpeek (unsigned char xbyte, unsigned int addr);
void xpoke (unsigned char xbyte, unsigned int addr, unsigned char val);

/* get sos device driver name for device number */
char* __fastcall__ getdevicename (unsigned char device, char* buf, size_t size);

/* End of apple3.h */
#endif
