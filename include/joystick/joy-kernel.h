/*****************************************************************************/
/*                                                                           */
/*                               joy-kernel.h                                */
/*                                                                           */
/*                    Internally used joystick functions                     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2002-2006, Ullrich von Bassewitz                                      */
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



#ifndef _JOY_KERNEL_H
#define _JOY_KERNEL_H



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* A structure that describes the header of a joystick driver loaded into
** memory.
*/
typedef struct {

    /* Driver header */
    char                id[3];          /* Contains 0x6a, 0x6f, 0x79 ("joy") */
    unsigned char       version;        /* Interface version */
    void*               libreference;   /* Library reference */

    /* Jump vectors. Note that these are not C callable */
    void*               install;        /* INSTALL routine */
    void*               uninstall;      /* UNINSTALL routine */
    void*               count;          /* COUNT routine */
    void*               read;           /* READ routine */

} joy_drv_header;



/* JOY kernel variables */
extern joy_drv_header*  joy_drv;        /* Pointer to driver */



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void joy_clear_ptr (void);
/* Clear the joy_drv pointer */



/* End of joy-kernel.h */
#endif
