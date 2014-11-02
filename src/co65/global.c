/*****************************************************************************/
/*                                                                           */
/*                                 global.c                                  */
/*                                                                           */
/*            Global variables for the co65 object file converter            */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2003      Ullrich von Bassewitz                                       */
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
#include "segnames.h"

/* co65 */
#include "global.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* File names */
const char* InputName       = 0;                /* Name of input file */
const char* OutputName      = 0;                /* Name of output file */

/* Default extensions */
const char AsmExt[]         = ".s";             /* Default assembler extension */

/* Segment names */
const char* CodeSeg         = SEGNAME_CODE;     /* Name of the code segment */
const char* DataSeg         = SEGNAME_DATA;     /* Name of the data segment */
const char* BssSeg          = SEGNAME_BSS;      /* Name of the bss segment */
const char* ZeropageSeg     = SEGNAME_ZEROPAGE; /* Name of the zeropage segment */

/* Labels */
const char* CodeLabel       = 0;                /* Label for the code segment */
const char* DataLabel       = 0;                /* Label for the data segment */
const char* BssLabel        = 0;                /* Label for the bss segment */
const char* ZeropageLabel   = 0;                /* Label for the zeropage segment */

/* Flags */
unsigned char DebugInfo     = 0;                /* Enable debug info */
unsigned char NoOutput      = 0;                /* Suppress the actual conversion */
