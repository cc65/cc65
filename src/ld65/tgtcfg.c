/*****************************************************************************/
/*                                                                           */
/*				 tgtcfg.c				     */
/*                                                                           */
/*		 Target machine configurations the ld65 linker		     */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2004 Ullrich von Bassewitz                                       */
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



/* common */
#include "check.h"

/* ld65 */
#include "binfmt.h"
#include "tgtcfg.h"



/*****************************************************************************/
/*			     Target configurations			     */
/*****************************************************************************/



/* An empty config */
static const char CfgEmpty[] = "";

/* Actual target configurations, converted into C strings by a perl script */
#include "apple2.inc"
#include "apple2enh.inc"
#include "atari.inc"
#include "atmos.inc"
#include "bbc.inc"
#include "c128.inc"
#include "c16.inc"
#include "c64.inc"
#include "cbm510.inc"
#include "cbm610.inc"
#include "geos.inc"
#include "lunix.inc"
#include "lynx.inc"
#include "module.inc"
#include "nes.inc"
#include "none.inc"
#include "pet.inc"
#include "plus4.inc"
#include "supervision.inc"
#include "vic20.inc"



/*****************************************************************************/
/*     	      	     	       	     Data     	 			     */
/*****************************************************************************/



/* Target configurations for all systems */
const TargetDesc Targets[TGT_COUNT] = {
    {  	BINFMT_BINARY,  CfgNone         },
    {  	BINFMT_O65,     CfgModule       },
    {  	BINFMT_BINARY,	CfgAtari        },
    {	BINFMT_BINARY,	CfgVic20	},
    {   BINFMT_BINARY,  CfgC16          },
    {  	BINFMT_BINARY,	CfgC64          },
    {  	BINFMT_BINARY,	CfgC128         },
    {  	BINFMT_BINARY,	CfgEmpty	},    	/* Ace */
    {  	BINFMT_BINARY,	CfgPlus4        },
    {  	BINFMT_BINARY,  CfgCBM510       },
    {  	BINFMT_BINARY,  CfgCBM610       },
    {  	BINFMT_BINARY,	CfgPET          },
    {  	BINFMT_BINARY, 	CfgBBC          },
    {   BINFMT_BINARY,	CfgApple2       },
    {   BINFMT_BINARY,  CfgApple2Enh    },
    {   BINFMT_BINARY,  CfgGeos         },
    {   BINFMT_O65,     CfgLunix        },
    {  	BINFMT_BINARY,	CfgAtmos        },
    {  	BINFMT_BINARY, 	CfgNES          },
    {  	BINFMT_BINARY, 	CfgSupervision  },
    {  	BINFMT_BINARY,	CfgLynx         },
};



/*****************************************************************************/
/*     	      	     	       	     Code     	 			     */
/*****************************************************************************/



void DumpBuiltinConfig (FILE* F, target_t T)
/* Dump a builtin linker configuration */
{
    /* Check the given parameter */
    PRECONDITION (T > TGT_UNKNOWN && T < TGT_COUNT);

    /* Dump the config */
    fprintf (F, "%s\n", Targets[T].Cfg);
}




