/*****************************************************************************/
/*                                                                           */
/*				 tgtcfg.h				     */
/*                                                                           */
/*		 Target machine configurations the ld65 linker		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2003 Ullrich von Bassewitz                                       */
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



#ifndef TGTCFG_H
#define TGTCFG_H



#include <stdio.h>

/* common */
#include "target.h"



/*****************************************************************************/
/*			     Target configurations			     */
/*****************************************************************************/



/* Structure describing a target */
typedef struct TargetDesc TargetDesc;
struct TargetDesc {
    unsigned char      	BinFmt;	/* Default binary format for the target */
    const char*	   	Cfg;	/* Pointer to configuration */
};

/* Target configurations for all systems */
extern const TargetDesc Targets [TGT_COUNT];



/*****************************************************************************/
/*     	      	     	       	     Code     	 			     */
/*****************************************************************************/



void DumpBuiltinConfig (FILE* F, target_t T);
/* Dump a builtin linker configuration */



/* End of tgtcfg.h */
#endif



