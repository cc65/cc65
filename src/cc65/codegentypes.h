/*****************************************************************************/
/*                                                                           */
/*                                 codegen.h                                 */
/*                                                                           */
/*                            6502 code generator                            */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2013, Ullrich von Bassewitz                                      */
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

#ifndef CODEGENTYPES_H
#define CODEGENTYPES_H

/* Code generator flags.
** Note: The type flags are designed so that a smaller type may override a
** larger one by or'ing it into the existing one.
** Note^2: The actual type including the sign flag is in the lower bits, so
** we can mask the information and use them as a table index.
*/
#define CF_NONE         0x0000  /* No special flags */

/* Values for the actual type */
#define CF_CHAR         0x0007  /* Operation on characters */
#define CF_INT          0x0003  /* Operation on ints */
#define CF_SHORT        CF_INT  /* Alias */
#define CF_PTR          CF_INT  /* Alias for readability */
#define CF_LONG         0x0001  /* Operation on longs */
#define CF_FLOAT        0x0010  /* Operation on a float */

/* Signedness */
#define CF_UNSIGNED     0x0008  /* Value is unsigned */

/* Masks for retrieving type information */
#define CF_TYPEMASK     0x0017  /* Type information */
#define CF_STYPEMASK    0x001F  /* Includes signedness */

#define CF_CONST        0x0040  /* Constant value available */
#define CF_TEST         0x0080  /* Test value */
#define CF_FIXARGC      0x0100  /* Function has fixed arg count */
#define CF_FORCECHAR    0x0200  /* Handle chars as chars, not ints */
#define CF_NOKEEP       0x0400  /* Value may get destroyed when storing */

/* Type of address */
#define CF_ADDRMASK     0xF000  /* Bit mask of address type */
#define CF_IMM          0x0000  /* Value is pure rvalue and has no storage */
#define CF_ABSOLUTE     0x1000  /* Numeric absolute address */
#define CF_EXTERNAL     0x2000  /* External */
#define CF_REGVAR       0x4000  /* Register variable */
#define CF_LITERAL      0x7000  /* Literal */
#define CF_PRIMARY      0x8000  /* Value is in primary register */
#define CF_EXPR         0x9000  /* Value is addressed by primary register */
#define CF_STATIC       0xA000  /* Local static */
#define CF_CODE         0xB000  /* C code label location */
#define CF_STACK        0xC000  /* Function-local auto on stack */

#endif
