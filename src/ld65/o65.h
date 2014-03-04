/*****************************************************************************/
/*                                                                           */
/*                                   o65.h                                   */
/*                                                                           */
/*                  Module to handle the o65 binary format                   */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1999-2005 Ullrich von Bassewitz                                       */
/*               Römerstrasse 52                                             */
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



#ifndef O65_H
#define O65_H



#include <stdio.h>

#include "extsyms.h"
#include "config.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Structure describing the format */
typedef struct O65Desc O65Desc;

/* Option tags */
#define O65OPT_FILENAME         0
#define O65OPT_OS               1
#define O65OPT_ASM              2
#define O65OPT_AUTHOR           3
#define O65OPT_TIMESTAMP        4

/* Operating system codes for O65OPT_OS */
#define O65OS_MIN               1
#define O65OS_OSA65             1
#define O65OS_LUNIX             2
#define O65OS_CC65              3
#define O65OS_OPENCBM           4
#define O65OS_MAX               255



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



O65Desc* NewO65Desc (void);
/* Create, initialize and return a new O65 descriptor struct */

void FreeO65Desc (O65Desc* D);
/* Delete the descriptor struct with cleanup */

void O65Set6502 (O65Desc* D);
/* Enable 6502 mode */

void O65Set65816 (O65Desc* D);
/* Enable 816 mode */

void O65SetSmallModel (O65Desc* D);
/* Enable a small memory model executable */

void O65SetLargeModel (O65Desc* D);
/* Enable a large memory model executable */

void O65SetAlignment (O65Desc* D, unsigned Align);
/* Set the executable alignment */

void O65SetOption (O65Desc* D, unsigned Type, const void* Data, unsigned DataLen);
/* Set an o65 header option */

void O65SetOS (O65Desc* D, unsigned OS, unsigned Version, unsigned Id);
/* Set an option describing the target operating system */

ExtSym* O65GetImport (O65Desc* D, unsigned Ident);
/* Return the imported symbol or NULL if not found */

void O65SetImport (O65Desc* D, unsigned Ident);
/* Set an imported identifier */

ExtSym* O65GetExport (O65Desc* D, unsigned Ident);
/* Return the exported symbol or NULL if not found */

void O65SetExport (O65Desc* D, unsigned Ident);
/* Set an exported identifier */

void O65WriteTarget (O65Desc* D, File* F);
/* Write an o65 output file */



/* End of o65.h */

#endif
