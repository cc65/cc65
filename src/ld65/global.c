/*****************************************************************************/
/*                                                                           */
/*                                 global.c                                  */
/*                                                                           */
/*                   Global variables for the ld65 linker                    */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2010, Ullrich von Bassewitz                                      */
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



#include "global.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



const char* OutputName      = "a.out";  /* Name of output file */
unsigned    OutputNameUsed  = 0;        /* Output name was used by %O */

unsigned ModuleId           = 0;        /* Id for o65 module */

/* Start address */
unsigned char HaveStartAddr = 0;        /* Start address not given */
unsigned long StartAddr     = 0x200;    /* Start address */

unsigned char VerboseMap    = 0;        /* Verbose map file */
const char* MapFileName     = 0;        /* Name of the map file */
const char* LabelFileName   = 0;        /* Name of the label file */
const char* DbgFileName     = 0;        /* Name of the debug file */
