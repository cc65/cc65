/*****************************************************************************/
/*                                                                           */
/*                               joy-kernel.h                                */
/*                                                                           */
/*                    Internally used joystick functions                     */
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



#ifndef _JOY_KERNEL_H
#define _JOY_KERNEL_H



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* A structure that describes the header of a joystick driver loaded into
 * memory.
 */
typedef struct {

    /* Driver header */
    char                id[3];          /* Contains 0x65, 0x6d, 0x64 ("emd") */
    unsigned char       version;        /* Interface version */

    /* Bitmasks for the joystick states. See joystick.h for indices */
    unsigned char       masks[8];

    /* Jump vectors. Note that these are not C callable */
    void*               install;        /* INSTALL routine */
    void*               deinstall;      /* DEINSTALL routine */
    void*               count;          /* COUNT routine */
    void*               read;           /* READ routine */

} joy_drv_header;



/* EM kernel variables */
extern joy_drv_header* 	joy_drv;       	/* Pointer to driver */




/*****************************************************************************/
/* 	      	       	      	   Functions	     			     */
/*****************************************************************************/



unsigned char __fastcall__ joy_install (void* driver);
/* Install the driver once it is loaded, return an error code. */

void __fastcall__ joy_deinstall (void);
/* Deinstall the driver before unloading it */



/* End of joy-kernel.h */
#endif




