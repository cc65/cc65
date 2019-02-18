/*****************************************************************************/
/*                                                                           */
/*                                   xex.h                                   */
/*                                                                           */
/*               Module to handle the Atari EXE binary format                */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2018 Daniel Serpell                                                   */
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



#ifndef XEX_H
#define XEX_H



#include "config.h"
#include "exports.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Structure describing the format */
typedef struct XexDesc XexDesc;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



XexDesc* NewXexDesc (void);
/* Create a new XEX format descriptor */

void FreeXexDesc (XexDesc* D);
/* Free a XEX format descriptor */

void XexWriteTarget (XexDesc* D, File* F);
/* Write a XEX output file */

void XexSetRunAd (XexDesc* D, Import *RunAd);
/* Set the RUNAD export */

int XexAddInitAd (XexDesc* D, MemoryArea *InitMem, Import *InitAd);
/* Sets and INITAD for the given memory area */

/* End of xex.h */

#endif
