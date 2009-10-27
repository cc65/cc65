/*****************************************************************************/
/*                                                                           */
/*                                     tgi.h                                 */
/*                                                                           */
/*                            Tiny graphics interface                        */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2002-2009, Ullrich von Bassewitz                                      */
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



#ifndef _TGI_MODE_H
#include <tgi/tgi-mode.h>
#endif
#ifndef _TGI_ERROR_H
#include <tgi/tgi-error.h>
#endif



/*****************************************************************************/
/*                                  Definitions                              */
/*****************************************************************************/



/* Constants used for tgi_textstyle */
#define TGI_TEXT_HORIZONTAL     0
#define TGI_TEXT_VERTICAL       1

/* The name of the standard tgi driver for a platform */
extern const char tgi_stddrv[];

/* The default tgi mode for a platform */
extern const unsigned char tgi_stdmode;



/*****************************************************************************/
/* 	      	  		   Functions	     			     */
/*****************************************************************************/



void __fastcall__ tgi_load (unsigned char mode);
/* Load and install the matching driver for the given mode. Will just load
 * the driver and check if loading was successul. Will not switch to gaphics
 * mode.
 */

void __fastcall__ tgi_load_driver (const char* name);
/* Load and install the given driver. This function is identical to tgi_load
 * with the only difference that the name of the driver is specified
 * explicitly.
 */

void __fastcall__ tgi_unload (void);
/* Uninstall, then unload the currently loaded driver. Will call tgi_done if
 * necessary.
 */

void __fastcall__ tgi_install (void* driver);
/* Install an already loaded driver. */

void __fastcall__ tgi_uninstall (void);
/* Uninstall the currently loaded driver but do not unload it. Will call
 * tgi_done if necessary.
 */

void __fastcall__ tgi_init (void);
/* Initialize the already loaded graphics driver. */

void __fastcall__ tgi_done (void);
/* End graphics mode, switch back to text mode. Will NOT uninstall or unload
 * the driver!
 */

unsigned char __fastcall__ tgi_geterror (void);
/* Return the error code for the last operation. This will also clear the
 * error.
 */

const char* __fastcall__ tgi_geterrormsg (unsigned char code);
/* Get an error message describing the error in code. */

void __fastcall__ tgi_clear (void);
/* Clear the drawpage. */

unsigned __fastcall__ tgi_getpagecount (void);
/* Returns the number of screen pages available. */

void __fastcall__ tgi_setviewpage (unsigned char page);
/* Set the visible page. Will set an error if the page is not available. */

void __fastcall__ tgi_setdrawpage (unsigned char page);
/* Set the drawable page. Will set an error if the page is not available. */

unsigned char __fastcall__ tgi_getcolorcount (void);
/* Get the number of available colors. */

unsigned char __fastcall__ tgi_getmaxcolor (void);
/* Return the maximum supported color number (the number of colors would
 * then be getmaxcolor()+1).
 */

void __fastcall__ tgi_setcolor (unsigned char color);
/* Set the current drawing color. */

unsigned char __fastcall__ tgi_getcolor (void);
/* Return the current drawing color. */

void __fastcall__ tgi_setpalette (const unsigned char* palette);
/* Set the palette (not available with all drivers/hardware). palette is
 * a pointer to as many entries as there are colors.
 */

const unsigned char* __fastcall__ tgi_getpalette (void);
/* Return the current palette. */

const unsigned char* __fastcall__ tgi_getdefpalette (void);
/* Return the default palette. */

unsigned __fastcall__ tgi_getxres (void);
/* Return the resolution in X direction. */

unsigned __fastcall__ tgi_getmaxx (void);
/* Return the maximum x coordinate. The resolution in x direction is
 * getmaxx() + 1
 */

unsigned __fastcall__ tgi_getyres (void);
/* Return the resolution in Y direction. */

unsigned __fastcall__ tgi_getmaxy (void);
/* Return the maximum y coordinate. The resolution in y direction is
 * getmaxy() + 1
 */

unsigned char __fastcall__ tgi_getpixel (int x, int y);
/* Get the color value of a pixel. */

void __fastcall__ tgi_setpixel (int x, int y);
/* Plot a pixel in the current drawing color. */

void __fastcall__ tgi_gotoxy (int x, int y);
/* Set the graphics cursor to the given position. */

void __fastcall__ tgi_line (int x1, int y1, int x2, int y2);
/* Draw a line in the current drawing color. The graphics cursor will
 * be set to x2/y2 by this call.
 */

void __fastcall__ tgi_lineto (int x2, int y2);
/* Draw a line in the current drawing color from the graphics cursor to the
 * new end point. The graphics cursor will be updated to x2/y2.
 */

void __fastcall__ tgi_circle (int x, int y, unsigned char radius);
/* Draw a circle in the current drawing color. */

void __fastcall__ tgi_bar (int x1, int y1, int x2, int y2);
/* Draw a bar (a filled rectangle) using the current color. */

void __fastcall__ tgi_textstyle (unsigned char magx, unsigned char magy,
                                 unsigned char dir);
/* Set the style for text output. */

unsigned __fastcall__ tgi_textwidth (const char* s);
/* Calculate the width of the text in pixels according to the current text
 * style.
 */

unsigned __fastcall__ tgi_textheight (const char* s);
/* Calculate the height of the text in pixels according to the current text
 * style.
 */

void __fastcall__ tgi_outtext (const char* s);
/* Output text at the current graphics cursor position. The graphics cursor
 * is moved to the end of the text.
 */

void __fastcall__ tgi_outtextxy (int x, int y, const char* s);
/* Output text at the given cursor position. The graphics cursor is moved to
 * the end of the text.
 */

unsigned __fastcall__ tgi_ioctl (unsigned char code, unsigned val);
/* Call the driver specific control function. What this function does for
 * a specific code depends on the driver. The driver will set an error
 * for unknown codes or values.
 */



/* End of tgi.h */
#endif



