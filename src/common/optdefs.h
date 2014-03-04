/*****************************************************************************/
/*                                                                           */
/*                                 optdefs.h                                 */
/*                                                                           */
/*                    Definitions for object file options                    */
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



#ifndef OPTDEFS_H
#define OPTDEFS_H



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Type of options */
#define OPT_ARGMASK     0xC0            /* Mask for argument */
#define OPT_ARGSTR      0x00            /* String argument */
#define OPT_ARGNUM      0x40            /* Numerical argument */

#define OPT_COMMENT     (OPT_ARGSTR+0)  /* Generic comment */
#define OPT_AUTHOR      (OPT_ARGSTR+1)  /* Author specification */
#define OPT_TRANSLATOR  (OPT_ARGSTR+2)  /* Translator specification */
#define OPT_COMPILER    (OPT_ARGSTR+3)  /* Compiler specification */
#define OPT_OS          (OPT_ARGSTR+4)  /* Operating system specification */

#define OPT_DATETIME    (OPT_ARGNUM+0)  /* Date/time of translation */



/* Structure to encode options */
typedef struct Option Option;
struct Option {
    Option*             Next;           /* For list of options */
    unsigned char       Type;           /* Type of option */
    unsigned long       Val;            /* Value attribute or string index */
};



/* End of optdefs.h */

#endif
