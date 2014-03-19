/*****************************************************************************/
/*                                                                           */
/*                                 global.h                                  */
/*                                                                           */
/*                Global variables for the da65 disassembler                 */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2011, Ullrich von Bassewitz                                      */
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



#ifndef GLOBAL_H
#define GLOBAL_H



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* File stuff */
extern const char*      InFile;         /* Name of input file */
extern const char*      OutFile;        /* Name of output file */

/* Default extensions */
extern const char       OutExt[];       /* Output file extension */
extern const char       CfgExt[];       /* Config file extension */

/* Flags and other command line stuff */
extern unsigned char    DebugInfo;      /* Add debug info to the object file */
extern unsigned char    FormFeeds;      /* Add form feeds to the output? */
extern unsigned char    UseHexOffs;     /* Use hexadecimal label offsets */
extern unsigned char    PassCount;      /* How many passed do we do? */
extern signed char      NewlineAfterJMP;/* Add a newline after a JMP insn? */
extern signed char      NewlineAfterRTS;/* Add a newline after a RTS insn? */
extern long             StartAddr;      /* Start/load address of the program */
extern long             InputOffs;      /* Offset into input file */
extern long             InputSize;      /* Number of bytes to read from input */

/* Stuff needed by many routines */
extern unsigned         Pass;           /* Disassembler pass */
extern char             Now[128];       /* Current time as string */

/* Comments */
#define MIN_COMMENTS    0
#define MAX_COMMENTS    4
extern unsigned         Comments;       /* Add which comments to the output? */

/* Page formatting */
#define MIN_PAGE_LEN    32
#define MAX_PAGE_LEN    127
extern unsigned         PageLength;     /* Length of a listing page */

/* Linefeed if labels exceed this limit */
#define MIN_LABELBREAK  1
#define MAX_LABELBREAK  128
extern unsigned         LBreak;

/* Mnemonic column */
#define MIN_MCOL        1
#define MAX_MCOL        127
extern unsigned         MCol;

/* Argument column */
#define MIN_ACOL        1
#define MAX_ACOL        127
extern unsigned         ACol;

/* Comment column */
#define MIN_CCOL        1
#define MAX_CCOL        127
extern unsigned         CCol;

/* Text bytes column */
#define MIN_TCOL        1
#define MAX_TCOL        127
extern unsigned         TCol;

/* Max. number of data bytes per line */
#define MIN_BYTESPERLINE        1
#define MAX_BYTESPERLINE        127
extern unsigned         BytesPerLine;



/* End of global.h */

#endif
