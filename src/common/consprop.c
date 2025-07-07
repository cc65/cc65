/*****************************************************************************/
/*                                                                           */
/*                                consprop.c                                 */
/*                                                                           */
/*                            Console properties                             */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2025,      Kugelfuhr                                                  */
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



#include <stdlib.h>
#include <unistd.h>
#ifdef _WIN32
#include <windows.h>
#define isatty _isatty
#endif

/* common */
#include "consprop.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



#ifdef _WIN32
static unsigned CodePage;               /* Windows code page on startup */
#endif

/* State variables */
static int IsTTY = 1;                   /* True if console is a tty */
static int IsUTF8 = 1;                  /* True if console is in UTF-8 mode */
static int Color = 0;                   /* True if we should use color */
static ColorMode CMode = CM_AUTO;       /* Current color mode */

/* ANSI escape sequences for color */
const char CP_ColorSeq[CC_COUNT][2][8] = {
    { "", "\x1b[30m", },                /* Black */
    { "", "\x1b[31m", },                /* Red */
    { "", "\x1b[32m", },                /* Green */
    { "", "\x1b[33m", },                /* Brown */
    { "", "\x1b[34m", },                /* Blue */
    { "", "\x1b[35m", },                /* Magenta */
    { "", "\x1b[36m", },                /* Cyan */
    { "", "\x1b[37m", },                /* LightGray */
    { "", "\x1b[90m", },                /* Gray */
    { "", "\x1b[91m", },                /* BrightRed */
    { "", "\x1b[92m", },                /* BrightGreen */
    { "", "\x1b[93m", },                /* Yellow */
    { "", "\x1b[94m", },                /* BrightBlue */
    { "", "\x1b[95m", },                /* BrightMagenta */
    { "", "\x1b[96m", },                /* BrightCyan */
    { "", "\x1b[97m", },                /* White */
};

/* Box drawing sequences */
const char CP_BoxDrawingSeq[BD_COUNT][2][4] = {
    { "-", "\xE2\x94\x80", },           /* "─" - Horizontal */
    { "|", "\xE2\x94\x82", },           /* "│" - Vertical */
    { "+", "\xE2\x94\x8C", },           /* "┌" - ULCorner */
    { "+", "\xE2\x94\x94", },           /* "└" - LLCorner */
    { "+", "\xE2\x94\x90", },           /* "┐" - URCorner */
    { "+", "\xE2\x94\x98", },           /* "┘" - LRCorner */
    { "+", "\xE2\x94\x9C", },           /* "├" - VerticalRight */
    { "+", "\xE2\x94\xA4", },           /* "┤" - VerticalLeft */
    { "+", "\xE2\x94\xAC", },           /* "┬" - HorizontalDown */
    { "+", "\xE2\x94\xB4", },           /* "┴" - HorizontalUp */
};



/*****************************************************************************/
/*                                 Helpers                                   */
/*****************************************************************************/



#ifdef _WIN32
static void Cleanup()
/* Cleanup on program exit */
{
    SetConsoleOutputCP (CodePage);
}
#endif



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void CP_Init (void)
/* Init console properties. Must be called before using any other function or
** data from this module.
*/
{
    IsTTY = (isatty (STDOUT_FILENO) && isatty (STDERR_FILENO));
    CP_SetColorMode (CMode);

#ifdef _WIN32
    if (IsTTY) {
        CodePage = GetConsoleOutputCP ();
        if (SetConsoleOutputCP (CP_UTF8)) {
            IsUTF8 = 1;
            atexit (Cleanup);
        }
        if (Color) {
            HANDLE StdOut = GetStdHandle (STD_OUTPUT_HANDLE);
            if (StdOut != INVALID_HANDLE_VALUE) {
                DWORD Mode;
                if (GetConsoleMode (StdOut, &Mode)) {
                    Mode |= ENABLE_PROCESSED_OUTPUT |
                            ENABLE_VIRTUAL_TERMINAL_PROCESSING;
                    SetConsoleMode (StdOut, Mode);
                }
            }
        }
    }
#endif
}



int CP_IsTTY (void)
/* Return true if console output goes to a tty */
{
    return IsTTY;
}



int CP_IsUTF8 (void)
/* Return true if the console supports UTF-8 */
{
    return IsUTF8;
}



int CP_HasColor (void)
/* Return true if the console supports color and it should be used */
{
    return Color;
}



ColorMode CP_GetColorMode (void)
/* Return the current color mode */
{
    return CMode;
}



void CP_SetColorMode (ColorMode M)
/* Set the color mode */
{
    CMode = M;
    switch (CMode) {
        case CM_AUTO:   Color = IsTTY;  break;
        case CM_ON:     Color = 1;      break;
        default:        Color = 0;      break;
    }
}



void CP_DisableUTF8 (void)
/* Disable UTF-8 support */
{
    IsUTF8 = 0;
}



void CP_EnableUTF8 (void)
/* Enable UTF-8 support */
{
    IsUTF8 = 1;
}
