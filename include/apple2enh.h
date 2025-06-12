/*****************************************************************************/
/*                                                                           */
/*                                apple2enh.h                                */
/*                                                                           */
/*               enhanced Apple //e system specific definitions              */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2004  Oliver Schmidt, <ol.sc@web.de>                                  */
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



#ifndef _APPLE2ENH_H
#define _APPLE2ENH_H



/* Check for errors */
#if !defined(__APPLE2ENH__)
#  error This module may only be used when compiling for the enhanced Apple //e!
#endif



#include <apple2.h>



/*****************************************************************************/
/*                                 Variables                                 */
/*****************************************************************************/



/* The addresses of the static drivers */
extern void a2e_auxmem_emd[];
extern void a2e_stdjoy_joy[];     /* Referred to by joy_static_stddrv[]   */
extern void a2e_stdmou_mou[];     /* Referred to by mouse_static_stddrv[] */
extern void a2e_ssc_ser[];        /* Referred to by ser_static_stddrv[]   */
extern void a2e_gs_ser[];         /* IIgs serial driver                   */
extern void a2e_hi_tgi[];         /* Referred to by tgi_static_stddrv[]   */
extern void a2e_lo_tgi[];



/* End of apple2enh.h */
#endif
