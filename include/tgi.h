/*****************************************************************************/
/*                                                                           */
/*                                     tgi.h                                 */
/*                                                                           */
/*                            Tiny graphics interface                        */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2002-2013, Ullrich von Bassewitz                                      */
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



#ifndef _TGI_H
#define _TGI_H



#include <tgi/tgi-error.h>
#include <target.h>



/*****************************************************************************/
/*                                  Definitions                              */
/*****************************************************************************/



/* Font constants for use with tgi_settextstyle */
#define TGI_FONT_BITMAP         0
#define TGI_FONT_VECTOR         1

/* Direction constants for use with tgi_settextstyle */
#define TGI_TEXT_HORIZONTAL     0
#define TGI_TEXT_VERTICAL       1

/* The name of the standard tgi driver for a platform */
extern const char tgi_stddrv[];

/* The address of the static standard tgi driver for a platform */
extern const void tgi_static_stddrv[];

/* A vector font definition */
typedef struct tgi_vectorfont tgi_vectorfont;



/*****************************************************************************/
/*                                 Functions                                 */
/*****************************************************************************/



void __fastcall__ tgi_load_driver (const char* name);
/* Load and install the given driver. */

void tgi_unload (void);
/* Uninstall, then unload the currently loaded driver. Will call tgi_done if
** necessary.
*/

void __fastcall__ tgi_install (const void* driver);
/* Install an already loaded driver. */

void tgi_uninstall (void);
/* Uninstall the currently loaded driver but do not unload it. Will call
** tgi_done if necessary.
*/

void tgi_init (void);
/* Initialize the already loaded graphics driver. */

void tgi_done (void);
/* End graphics mode, switch back to text mode. Will NOT uninstall or unload
** the driver!
*/

const tgi_vectorfont* __fastcall__ tgi_load_vectorfont (const char* name);
/* Load a vector font into memory and return it. In case of errors, NULL is
** returned and an error is set, which can be retrieved using tgi_geterror.
** To use the font, it has to be installed using tgi_install_vectorfont.
*/

void __fastcall__ tgi_install_vectorfont (const tgi_vectorfont* font);
/* Install a vector font for use. More than one vector font can be loaded,
** but only one can be active. This function is used to tell which one. Call
** with a NULL pointer to uninstall the currently installed font.
*/

void __fastcall__ tgi_free_vectorfont (const tgi_vectorfont* font);
/* Free a vector font that was previously loaded into memory. */

unsigned char tgi_geterror (void);
/* Return the error code for the last operation. This will also clear the
** error.
*/

const char* __fastcall__ tgi_geterrormsg (unsigned char code);
/* Get an error message describing the error in code. */

void tgi_clear (void);
/* Clear the drawpage. */

unsigned tgi_getpagecount (void);
/* Returns the number of screen pages available. */

void __fastcall__ tgi_setviewpage (unsigned char page);
/* Set the visible page. Will set an error if the page is not available. */

void __fastcall__ tgi_setdrawpage (unsigned char page);
/* Set the drawable page. Will set an error if the page is not available. */

unsigned char tgi_getcolorcount (void);
/* Get the number of available colors. Zero means 256 colors. */

unsigned char tgi_getmaxcolor (void);
/* Return the maximum supported color number (the number of colors would
** then be getmaxcolor()+1).
*/

void __fastcall__ tgi_setcolor (unsigned char color);
/* Set the current drawing color. */

unsigned char tgi_getcolor (void);
/* Return the current drawing color. */

void __fastcall__ tgi_setpalette (const unsigned char* palette);
/* Set the palette (not available with all drivers/hardware). palette is
** a pointer to as many entries as there are colors.
*/

const unsigned char* tgi_getpalette (void);
/* Return the current palette. */

const unsigned char* tgi_getdefpalette (void);
/* Return the default palette. */

unsigned tgi_getxres (void);
/* Return the resolution in X direction. */

unsigned tgi_getmaxx (void);
/* Return the maximum x coordinate. The resolution in x direction is
** getmaxx() + 1
*/

unsigned tgi_getyres (void);
/* Return the resolution in Y direction. */

unsigned tgi_getmaxy (void);
/* Return the maximum y coordinate. The resolution in y direction is
** getmaxy() + 1
*/

unsigned tgi_getaspectratio (void);
/* Returns the aspect ratio for the loaded driver. The aspect ratio is an
** 8.8 fixed point value.
*/

void __fastcall__ tgi_setaspectratio (unsigned aspectratio);
/* Set a new aspect ratio for the loaded driver. The aspect ratio is an
** 8.8 fixed point value.
*/

unsigned char __fastcall__ tgi_getpixel (int x, int y);
/* Get the color value of a pixel. */

void __fastcall__ tgi_setpixel (int x, int y);
/* Plot a pixel in the current drawing color. */

void __fastcall__ tgi_gotoxy (int x, int y);
/* Set the graphics cursor to the given position. */

void __fastcall__ tgi_line (int x1, int y1, int x2, int y2);
/* Draw a line in the current drawing color. The graphics cursor will
** be set to x2/y2 by this call.
*/

void __fastcall__ tgi_lineto (int x2, int y2);
/* Draw a line in the current drawing color from the graphics cursor to the
** new end point. The graphics cursor will be updated to x2/y2.
*/

void __fastcall__ tgi_circle (int x, int y, unsigned char radius);
/* Draw a circle in the current drawing color. */

void __fastcall__ tgi_ellipse (int x, int y, unsigned char rx, unsigned char ry);
/* Draw a full ellipse with center at x/y and radii rx/ry using the current
** drawing color.
*/

void __fastcall__ tgi_arc (int x, int y, unsigned char rx, unsigned char ry,
                           unsigned sa, unsigned ea);
/* Draw an ellipse arc with center at x/y and radii rx/ry using the current
** drawing color. The arc covers the angle between sa and ea (startangle and
** endangle), which must be in the range 0..360 (otherwise the function may
** behave unexpectedly).
*/

void __fastcall__ tgi_pieslice (int x, int y, unsigned char rx, unsigned char ry,
                                unsigned sa, unsigned ea);
/* Draw an ellipse pie slice with center at x/y and radii rx/ry using the
** current drawing color. The pie slice covers the angle between sa and ea
** (startangle and endangle), which must be in the range 0..360 (otherwise the
** function may behave unextectedly).
*/

void __fastcall__ tgi_bar (int x1, int y1, int x2, int y2);
/* Draw a bar (a filled rectangle) using the current color. */

void __fastcall__ tgi_settextdir (unsigned char dir);
/* Set the direction for text output. dir is one of the TGI_TEXT_XXX
** constants.
*/

void __fastcall__ tgi_settextscale (unsigned width, unsigned height);
/* Set the scaling for text output. The scaling factors for width and height
** are 8.8 fixed point values. This means that $100 = 1 $200 = 2 etc.
*/

void __fastcall__ tgi_settextstyle (unsigned width, unsigned height,
                                    unsigned char dir, unsigned char font);
/* Set the style for text output. The scaling factors for width and height
** are 8.8 fixed point values. This means that $100 = 1 $200 = 2 etc.
** dir is one of the TGI_TEXT_XXX constants. font is one of the TGI_FONT_XXX
** constants.
*/

unsigned __fastcall__ tgi_gettextwidth (const char* s);
/* Calculate the width of the text in pixels according to the current text
** style.
*/

unsigned __fastcall__ tgi_gettextheight (const char* s);
/* Calculate the height of the text in pixels according to the current text
** style.
*/

void __fastcall__ tgi_outtext (const char* s);
/* Output text at the current graphics cursor position. The graphics cursor
** is moved to the end of the text.
*/

void __fastcall__ tgi_outtextxy (int x, int y, const char* s);
/* Output text at the given cursor position. The graphics cursor is moved to
** the end of the text.
*/

unsigned __fastcall__ tgi_ioctl (unsigned char code, void* data);
/* Call the driver specific control function. What this function does for
** a specific code depends on the driver. The driver will set an error
** for unknown codes or values.
*/

int __fastcall__ tgi_imulround (int rhs, int lhs);
/* Helper function for functions using sine/cosine: Multiply two values, one
** being an 8.8 fixed point one, and return the rounded and scaled result.
*/



/* End of tgi.h */
#endif
