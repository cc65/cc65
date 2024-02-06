/*****************************************************************************/
/*                                                                           */
/*                                 tgi-kernel.h                              */
/*                                                                           */
/*                             TGI kernel interface                          */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2002-2012, Ullrich von Bassewitz                                      */
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



#ifndef _TGI_KERNEL_H
#define _TGI_KERNEL_H



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* TGI kernel variables */
extern void*         tgi_drv;           /* Pointer to driver */
extern unsigned char tgi_error;         /* Last error code */
extern unsigned char tgi_gmode;         /* Flag: Graphics mode active */
extern int           tgi_curx;          /* Current drawing cursor X */
extern int           tgi_cury;          /* Current drawing cursor Y */
extern unsigned char tgi_color;         /* Current drawing color */
extern unsigned char tgi_font;          /* Current font type */
extern unsigned      tgi_xres;          /* X resolution of the current mode */
extern unsigned      tgi_yres;          /* Y resolution of the current mode */
extern unsigned char tgi_colorcount;    /* Number of available colors */
extern unsigned char tgi_pagecount;     /* Number of available screens */
extern unsigned char tgi_fontwidth;     /* System font width in pixels */
extern unsigned char tgi_fontheight;    /* System font height in pixels */
extern unsigned      tgi_aspectratio;   /* Aspect ratio as fixed point 8.8 */
extern unsigned char tgi_flags;         /* TGI driver flags */
extern unsigned      tgi_textscalew[2]; /* Vector/bitmap font scale 8.8 */
extern unsigned      tgi_textscaleh[2]; /* Vector/bitmap font scale 8.8 */
extern unsigned      tgi_charwidth;     /* Width of scaled bitmap font */
extern unsigned      tgi_charheight;    /* Height of scaled bitmap font */



/* End of tgi-kernel.h */
#endif




