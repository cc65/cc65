/*****************************************************************************/
/*                                                                           */
/*				  graphics.h                                 */
/*                                                                           */
/*			       Graphics library                              */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2001      Ullrich von Bassewitz                                       */
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



#ifndef _GRAPHICS_H
#define _GRAPHICS_H



/*****************************************************************************/
/*		  		   Constants                                 */
/*****************************************************************************/



#define GM_TEXT	       	       	0U     	/* Text mode */
#define GM_320_200_2            1U      /* 320x200, 2 colors (b/w) */
#define GM_160_200_4            2U      /* 160x200, 4 colors */



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



unsigned char __fastcall__ initgraph (unsigned char mode);
/* Initialize the graphics screen to the given mode, return the new mode.
 * Not all modes are available on all systems, check the returned mode
 * to see if the initialization was successful.
 */

void closegraph (void);
/* End graphics mode, switch back to text mode */

void cleargraphscreen (void);
/* Clear the screen */

unsigned char getgraphmode (void);
/* Return the current graphics mode */

unsigned char getmaxcolor (void);
/* Return the maximum supported color number (the number of colors would
 * then be getmaxcolor()+1).
 */

unsigned getmaxx (void);
/* Return the maximum x coordinate. The resolution in x direction is
 * getmaxx() + 1
 */

unsigned getmaxy (void);
/* Return the maximum y coordinate. The resolution in y direction is
 * getmaxy() + 1
 */

void __fastcall__ getaspectratio (unsigned* xasp, unsigned* yasp);
/* Get the aspect ratio for the current graphics mode. yasp will always be
 * 10000, xasp depends on the maxx/mayy ratio of the current resolution.
 */

void __fastcall__ setcolor (unsigned char color);
/* Set the current drawing color */

unsigned char getcolor (void);
/* Return the current drawing color */

unsigned char getbkcolor (void);
/* Return the current background color */

void __fastcall__ setbkcolor (unsigned char color);
/* Set the background color */

void __fastcall__ setpalette (unsigned num, const struct palettetype* palette);
/* Set one palette entry */

void __fastcall__ getpalette (unsigned num, struct palettetype* palette);
/* Get one palette entry */

void __fastcall__ setallpalette (const struct palettetype* allpalette);
/* Set all palette entries */

unsigned char __fastcall__ getpixel (int x, int y);
/* Get the color value of a pixel */

void __fastcall__ putpixel (int x, int y);
/* Plot a point in the current drawing color */

void __fastcall__ line (int x1, int y1, int x2, int y2);
/* Draw a line in the current drawing color */

void __fastcall__ circle (int x, int y, unsigned radius);
/* Draw a circle in the current drawing color */

void __fastcall__ outtext (int x, int y, const char* text);
/* Print a text in graphics mode */

void __fastcall__ bar (int x1, int y1, int x2, int y2);
/* Draw a bar (a filled rectangle) using the current color */



/* End of graphics.h */
#endif



