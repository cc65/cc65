/*****************************************************************************/
/*                                                                           */
/*                                 fragdefs.h                                */
/*                                                                           */
/*              Fragment definitions for the bin65 binary utils              */
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



#ifndef FRAGDEFS_H
#define FRAGDEFS_H



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Masks for the fragment type byte */
#define FRAG_TYPEMASK   0x38            /* Mask the type of the fragment */
#define FRAG_BYTEMASK   0x07            /* Mask for byte count */

/* Fragment types */
#define FRAG_LITERAL    0x00            /* Literal data */

#define FRAG_EXPR       0x08            /* Expression */
#define FRAG_EXPR8      (FRAG_EXPR | 1) /* 8 bit expression */
#define FRAG_EXPR16     (FRAG_EXPR | 2) /* 16 bit expression */
#define FRAG_EXPR24     (FRAG_EXPR | 3) /* 24 bit expression */
#define FRAG_EXPR32     (FRAG_EXPR | 4) /* 32 bit expression */

#define FRAG_SEXPR      0x10            /* Signed expression */
#define FRAG_SEXPR8     (FRAG_SEXPR | 1)/* 8 bit signed expression */
#define FRAG_SEXPR16    (FRAG_SEXPR | 2)/* 16 bit signed expression */
#define FRAG_SEXPR24    (FRAG_SEXPR | 3)/* 24 bit signed expression */
#define FRAG_SEXPR32    (FRAG_SEXPR | 4)/* 32 bit signed expression */

#define FRAG_FILL       0x20            /* Fill bytes */



/* End of fragdefs.h */

#endif
