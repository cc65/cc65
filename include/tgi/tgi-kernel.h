/*****************************************************************************/
/*                                                                           */
/*                                 tgi-kernel.h                              */
/*                                                                           */
/*                             TGI kernel interface                          */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2002-2004 Ullrich von Bassewitz                                       */
/*               Römerstraße 52                                              */
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



#ifndef _TGI_KERNEL_H
#define _TGI_KERNEL_H



/*****************************************************************************/
/*				     Data                                    */
/*****************************************************************************/



/* A structure that describes the header of a graphics driver loaded into
 * memory.
 */
typedef struct {

    /* Data that describes the capabilities of the driver */
    char                id[3];          /* Contains 0x74, 0x67, 0x69 ("tgi") */
    unsigned char       version;        /* Interface version */
    unsigned            xres;           /* X resolution */
    unsigned            yres;           /* Y resolution */
    unsigned char       colorcount;     /* Number of available colors */
    unsigned char       pagecount;      /* Number of screens available */
    unsigned char       res[6];         /* Reserved for extensions */

    /* Jump vectors. Note that these are not C callable */
    void*               install;        /* INSTALL routine */
    void*               uninstall;      /* UNINSTALL routine */
    void*               init;           /* INIT routine */
    void*               done;           /* DONE routine */
    void*		geterror;	/* GETERROR routine */
    void*               control;        /* CONTROL routine */
    void*               clear;          /* CLEAR routine */
    void*               setviewpage;    /* SETVIEWPAGE routine */
    void*               setdrawpage;    /* SETDRAWPAGE routine */
    void*               setcolor;       /* SETCOLOR routine */
    void*               setpalette;     /* SETPALETTE routine */
    void*               getpalette;     /* GETPALETTE routine */
    void*               getdefpalette;  /* GETDEFPALETTE routine */
    void*               setpixel;       /* SETPIXEL routine */
    void*               getpixel;       /* GETPIXEL routine */
    void*               line;           /* LINE routine */
    void*               bar;            /* BAR routine */
    void*               circle;         /* CIRCLE routine */
    void*               irq;            /* IRQ routine */

} tgi_drv_header;



/* TGI kernel variables */
extern tgi_drv_header*	tgi_drv;       	/* Pointer to driver */
extern unsigned char  	tgi_error;     	/* Last error code */
extern unsigned char    tgi_gmode;      /* Flag: Graphics mode active */
extern int              tgi_curx;       /* Current drawing cursor X */
extern int              tgi_cury;       /* Current drawing cursor Y */
extern unsigned char    tgi_color;      /* Current drawing color */
extern unsigned         tgi_xres;       /* X resolution of the current mode */
extern unsigned         tgi_yres;       /* Y resolution of the current mode */
extern unsigned char    tgi_colorcount; /* Number of available colors */
extern unsigned char    tgi_pagecount;  /* Number of available screens */



/*****************************************************************************/
/*                                     Code                                  */
/*****************************************************************************/



const char* __fastcall__ tgi_map_mode (unsigned char mode);
/* Map a tgi mode to a driver name. Returns NULL if no driver available. */



/* End of tgi-kernel.h */
#endif




