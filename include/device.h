/*****************************************************************************/
/*                                                                           */
/*                                 device.h                                  */
/*                                                                           */
/*                              Device handling                              */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2012  Oliver Schmidt, <ol.sc@web.de>                                  */
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



#ifndef _DEVICE_H
#define _DEVICE_H



#ifndef _HAVE_size_t
typedef unsigned size_t;
#define _HAVE_size_t
#endif



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



#define INVALID_DEVICE  255



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



unsigned char getfirstdevice (void);

unsigned char __fastcall__ getnextdevice (unsigned char device);

unsigned char getcurrentdevice (void);

char* __fastcall__ getdevicedir (unsigned char device, char* buf, size_t size);



/* End of device.h */
#endif
