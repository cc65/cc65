/*****************************************************************************/
/*                                                                           */
/*                                     tgi.h                                 */
/*                                                                           */
/*                            Tiny graphics interface                        */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2002      Ullrich von Bassewitz                                       */
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



#ifndef _TGI_H
#define _TGI_H



#ifndef _TGI_MODE_H
#include "tgi/tgi-mode.h"
#endif
#ifndef _TGI_ERROR_H
#include "tgi/tgi-error.h"
#endif



/*****************************************************************************/
/*				     Data                                    */
/*****************************************************************************/



struct palettetype {
    unsigned char   r;          /* Red component */
    unsigned char   g;          /* Green component */
    unsigned char   b;          /* Blue component */
};



/*****************************************************************************/
/* 	      	  		   Functions	     			     */
/*****************************************************************************/



void __fastcall__ tgi_load (unsigned char mode);
/* Install the matching driver for the given mode. Will just load the driver
 * and check if loading was successul. Will not switch to gaphics mode.
 */

void __fastcall__ tgi_load_driver (const char* name);
/* Install the given driver. This function is identical to tgi_load with the
 * only difference that the name of the driver is specified explicitly. You
 * should NOT use this function in most cases, use tgi_load() instead.
 */

void __fastcall__ tgi_unload (void);
/* Unload the currently loaded driver. */

void __fastcall__ tgi_init (unsigned char mode);
/* Initialize the given graphics mode. */

void __fastcall__ tgi_done (void);
/* End graphics mode, switch back to text mode. Will NOT unload the driver! */

unsigned char __fastcall__ tgi_geterror (void);
/* Return the error code for the last operation. This will also clear the
 * error.
 */

void __fastcall__ tgi_clear (void);
/* Clear the screen */

void __fastcall__ tgi_setviewpage (unsigned char page);
/* Set the visible page. */

void __fastcall__ tgi_setdrawpage (unsigned char page);
/* Set the drawable page */

unsigned char __fastcall__ tgi_getmaxcolor (void);
/* Return the maximum supported color number (the number of colors would
 * then be getmaxcolor()+1).
 */

unsigned __fastcall__ tgi_getmaxx (void);
/* Return the maximum x coordinate. The resolution in x direction is
 * getmaxx() + 1
 */

unsigned __fastcall__ tgi_getmaxy (void);
/* Return the maximum y coordinate. The resolution in y direction is
 * getmaxy() + 1
 */

unsigned char __fastcall__ tgi_getcolorcount (void);
/* Get the number of available colors */

unsigned __fastcall__ tgi_getxres (void);
/* Return the resolution in X direction */

unsigned __fastcall__ tgi_getyres (void);
/* Return the resolution in Y direction */

void __fastcall__ tgi_setcolor (unsigned char color);
/* Set the current drawing color */

unsigned char __fastcall__ tgi_getcolor (void);
/* Return the current drawing color */

unsigned char __fastcall__ tgi_getbkcolor (void);
/* Return the current background color */

void __fastcall__ tgi_setbkcolor (unsigned char color);
/* Set the background color */

unsigned char __fastcall__ tgi_getpixel (int x, int y);
/* Get the color value of a pixel */

void __fastcall__ tgi_setpixel (int x, int y);
/* Plot a point in the current drawing color */

void __fastcall__ tgi_line (int x1, int y1, int x2, int y2);
/* Draw a line in the current drawing color */

void __fastcall__ tgi_lineto (int x2, int y2);
/* Draw a line in the current drawing color from the graphics cursor to the
 * new end point.
 */

void __fastcall__ tgi_circle (int x, int y, unsigned char radius);
/* Draw a circle in the current drawing color */

void __fastcall__ tgi_outtext (int x, int y, const char* text);
/* Print a text in graphics mode */

void __fastcall__ tgi_bar (int x1, int y1, int x2, int y2);
/* Draw a bar (a filled rectangle) using the current color */



/* End of tgi.h */
#endif



