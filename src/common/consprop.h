/*****************************************************************************/
/*                                                                           */
/*                                consprop.h                                 */
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



#ifndef CONSPROP_H
#define CONSPROP_H



/* common */
#include "check.h"
#include "consprop.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Color mode for the program */
enum ColorMode { CM_INVALID = -1, CM_OFF, CM_AUTO, CM_ON };
typedef enum ColorMode ColorMode;

/* Colors */
enum ConsoleColor {
    CC_BLACK, CC_RED, CC_GREEN, CC_BROWN,
    CC_BLUE, CC_MAGENTA, CC_CYAN, CC_LIGHTGRAY,
    CC_GRAY, CC_BRIGHTRED, CC_BRIGHTGREEN, CC_YELLOW,
    CC_BRIGHTBLUE, CC_BRIGHTMAGENTA, CC_BRIGHTCYAN, CC_WHITE,
    CC_COUNT,   /* Number of colors */
};
typedef enum ConsoleColor ConsoleColor;

/* Box drawing characters */
enum BoxDrawing {
    BD_HORIZONTAL,
    BD_VERTICAL,
    BD_ULCORNER,
    BD_LLCORNER,
    BD_URCORNER,
    BD_LRCORNER,
    BD_RVERTICAL,
    BD_LVERTICAL,
    BD_DHORIZONTAL,
    BD_UHORIZONTAL,
    BD_COUNT,           /* Number of available box drawing chars */
};
typedef enum BoxDrawing BoxDrawing;

/* ANSI escape sequences for color - don't use directly */
extern const char CP_ColorSeq[CC_COUNT][2][8];

/* Box drawing characters - don't use directly */
extern const char CP_BoxDrawingSeq[BD_COUNT][2][4];



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void CP_Init (void);
/* Init console properties. Must be called before using any other function or
** data from this module.
**/

ColorMode CP_Parse (const char* Mode);
/* Parse the given string which is assumed to be one of the color modes.
** Return the matching enum or CM_INVALID if there was no match.
*/

int CP_IsTTY (void);
/* Return true if console output goes to a tty */

int CP_IsUTF8 (void);
/* Return true if the console supports UTF-8 */

int CP_HasColor (void);
/* Return true if the console supports color and it should be used */

ColorMode CP_GetColorMode (void);
/* Return the current color mode */

void CP_SetColorMode (ColorMode M);
/* Set the color mode */

void CP_DisableUTF8 (void);
/* Disable UTF-8 support */

void CP_EnableUTF8 (void);
/* Enable UTF-8 support */

static inline const char* CP_Color (ConsoleColor C)
/* Return the ANSI escape sequence for a specific color or an empty string */
{
    PRECONDITION (C >= 0 && C < CC_COUNT);
    return CP_ColorSeq[C][CP_HasColor ()];
}

static inline const char* CP_Reset (void)
/* Return the ANSI escape sequence to reset the colors or an empty string */
{
    return CP_HasColor () ? "\x1b[0m" : "";
}

/* Return specific colors */
static inline const char* CP_Black (void) { return CP_Color (CC_BLACK); }
static inline const char* CP_Red (void) { return CP_Color (CC_RED); }
static inline const char* CP_Green (void) { return CP_Color (CC_GREEN); }
static inline const char* CP_Brown (void) { return CP_Color (CC_BROWN); }
static inline const char* CP_Blue (void) { return CP_Color (CC_BLUE); }
static inline const char* CP_Magenta (void) { return CP_Color (CC_MAGENTA); }
static inline const char* CP_Cyan (void) { return CP_Color (CC_CYAN); }
static inline const char* CP_LightGray (void) { return CP_Color (CC_LIGHTGRAY); }
static inline const char* CP_Gray (void) { return CP_Color (CC_GRAY); }
static inline const char* CP_BrightRed (void) { return CP_Color (CC_BRIGHTRED); }
static inline const char* CP_BrightGreen (void) { return CP_Color (CC_BRIGHTGREEN); }
static inline const char* CP_Yellow (void) { return CP_Color (CC_YELLOW); }
static inline const char* CP_BrightBlue (void) { return CP_Color (CC_BRIGHTBLUE); }
static inline const char* CP_BrightMagenta (void) { return CP_Color (CC_BRIGHTMAGENTA); }
static inline const char* CP_BrightCyan (void) { return CP_Color (CC_BRIGHTCYAN); }
static inline const char* CP_White (void) { return CP_Color (CC_WHITE); }

static inline const char* CP_BoxDrawing (BoxDrawing B)
/* Return the UTF-8 sequence for the box drawing characters */
{
    PRECONDITION (B >= 0 && B < BD_COUNT);
    return CP_BoxDrawingSeq[B][CP_IsUTF8 ()];
}

/* Return specific box drawing character sequences */
static inline const char* CP_Horizontal (void) { return CP_BoxDrawing (BD_HORIZONTAL); }
static inline const char* CP_Vertical (void) { return CP_BoxDrawing (BD_VERTICAL); }
static inline const char* CP_ULCorner (void) { return CP_BoxDrawing (BD_ULCORNER); }
static inline const char* CP_LLCorner (void) { return CP_BoxDrawing (BD_LLCORNER); }
static inline const char* CP_URCorner (void) { return CP_BoxDrawing (BD_URCORNER); }
static inline const char* CP_LRCorner (void) { return CP_BoxDrawing (BD_LRCORNER); }
static inline const char* CP_VerticalRight (void) { return CP_BoxDrawing (BD_RVERTICAL); }
static inline const char* CP_VerticalLeft (void) { return CP_BoxDrawing (BD_LVERTICAL); }
static inline const char* CP_HorizontalDown (void) { return CP_BoxDrawing (BD_DHORIZONTAL); }
static inline const char* CP_HorizontalUp (void) { return CP_BoxDrawing (BD_UHORIZONTAL); }



/* End of consprop.h */

#endif
