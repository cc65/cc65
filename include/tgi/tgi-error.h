/*****************************************************************************/
/*                                                                           */
/*                                  tgi-error.h                              */
/*                                                                           */
/*                                TGI error codes                            */
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



#ifndef _TGI_ERROR_H
#define _TGI_ERROR_H



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



#define TGI_ERR_OK              0       /* No error */
#define TGI_ERR_NO_DRIVER       1       /* No driver available */
#define TGI_ERR_CANNOT_LOAD     2       /* Error loading driver or font */
#define TGI_ERR_INV_DRIVER      3       /* Invalid driver */
#define TGI_ERR_INV_MODE        4       /* Mode not supported by driver */
#define TGI_ERR_INV_ARG         5       /* Invalid function argument */
#define TGI_ERR_INV_FUNC        6       /* Function not supported */
#define TGI_ERR_INV_FONT        7       /* Font file is invalid */
#define TGI_ERR_NO_RES          8       /* Out of resources */
#define TGI_ERR_INSTALLED       9       /* A driver is already installed */



/* End of tgi-error.h */
#endif



