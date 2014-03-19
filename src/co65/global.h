/*****************************************************************************/
/*                                                                           */
/*                                 global.h                                  */
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



#ifndef GLOBAL_H
#define GLOBAL_H



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* File names */
extern const char*      InputName;          /* Name of input file */
extern const char*      OutputName;         /* Name of output file */

/* Default extensions */
extern const char       AsmExt[];           /* Default assembler extension */

/* Segment names */
extern const char*      CodeSeg;            /* Name of the code segment */
extern const char*      DataSeg;            /* Name of the data segment */
extern const char*      BssSeg;             /* Name of the bss segment */
extern const char*      ZeropageSeg;        /* Name of the zeropage segment */

/* Labels */
extern const char*      CodeLabel;          /* Label for the code segment */
extern const char*      DataLabel;          /* Label for the data segment */
extern const char*      BssLabel;           /* Label for the bss segment */
extern const char*      ZeropageLabel;      /* Label for the zeropage segment */

/* Flags */
extern unsigned char    DebugInfo;          /* Enable debug info */
extern unsigned char    NoOutput;           /* Suppress the actual conversion */



/* End of global.h */

#endif
