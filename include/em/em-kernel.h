/*****************************************************************************/
/*                                                                           */
/*                                em-kernel.h                                */
/*                                                                           */
/*                       Internally used EM functions                        */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2002-2003 Ullrich von Bassewitz                                       */
/*               Roemerstrasse 52                                            */
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



#ifndef _EM_KERNEL_H
#define _EM_KERNEL_H



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* A structure that describes the header of an extended memory driver loaded
** into memory.
*/
typedef struct {

    /* Driver header */
    char                id[3];          /* Contains 0x65, 0x6d, 0x64 ("emd") */
    unsigned char       version;        /* Interface version */
    void*               libreference;   /* Library reference */

    /* Jump vectors. Note that these are not C callable */
    void*               install;        /* INSTALL routine */
    void*               deinstall;      /* DEINSTALL routine */
    void*               pagecount;      /* PAGECOUNT routine */
    void*               map;            /* MAP routine */
    void*               use;            /* USE routine */
    void*               mapclean;       /* MAPCLEAN routine */
    void*               copyfrom;       /* COPYFROM routine */
    void*               copyto;         /* COPYTO routine */

} em_drv_header;



/* EM kernel variables */
extern em_drv_header*   em_drv;         /* Pointer to driver */



/* End of em-kernel.h */
#endif
