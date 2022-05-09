/*****************************************************************************/
/*                                                                           */
/*                                 version.c                                 */
/*                                                                           */
/*             Version information for the cc65 compiler package             */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2009, Ullrich von Bassewitz                                      */
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



/* common */
#include "xsprintf.h"
#include "searchpath.h"
#include "version.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



#define VER_MAJOR       2U
#define VER_MINOR       19U

/*
 * Note: until 2.19 the __CC65__ macro was defined as (VER_MAJOR * 0x100) + VER_MINOR * 0x10
 * which resulted in broken values starting at version 2.16 of the compiler:
 *
 * version  __CC65__    identifies as
 *
 * 2.0      0x0200      2.0
 * 2.1      0x0210      2.16
 * 2.2      0x0220      2.32
 * 2.3      0x0230      2.48
 * 2.4      0x0240      2.64
 * 2.5      0x0250      2.80
 * 2.6      0x0260      2.96
 * 2.7      0x0270      2.112
 * 2.8      0x0280      2.128
 * 2.9      0x0290      2.144
 * 2.10     0x02a0      2.160
 * 2.11     0x02b0      2.176
 * 2.12     0x02c0      2.192
 * 2.13     0x02d0      2.208
 * 2.14     0x02e0      2.224
 * 2.15     0x02f0      2.240
 * 2.16     0x0300      3.0
 * 2.17     0x0310      3.16
 * 2.18     0x0320      3.32
 * 2.19     0x0330      3.48
 * 2.19-git 0x0213      2.19
 *
 * to keep damage low(er), we should do the following:
 *
 * - bump to 3.1 (skip 3.0) before 2.32
 * - bump to 4.0 before 3.16
 *
 * That way at least each value is unique, and checking compiler version(s) can
 * still work to some degree, should it really be necessary.
 *
 * Some preprocessor kludges can be used to still check for greater or lesser
 * versions - see the checkversion program in the samples directory.
 *
 */

/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



const char* GetVersionAsString (void)
/* Returns the version number as a string in a static buffer */
{
    static char Buf[60];
#if defined(BUILD_ID)
    xsnprintf (Buf, sizeof (Buf), "%u.%u - %s", VER_MAJOR, VER_MINOR, BUILD_ID);
#else
    xsnprintf (Buf, sizeof (Buf), "%u.%u", VER_MAJOR, VER_MINOR);
#endif
    return Buf;
}



unsigned GetVersionAsNumber (void)
/* Returns the version number as a combined unsigned for use in a #define */
{
#if VER_MINOR >= 0x100
#error "VER_MINOR must be smaller than 0x100 - time to bump the major version!"
#endif
    return ((VER_MAJOR * 0x100) + VER_MINOR);
}
