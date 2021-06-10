/*****************************************************************************/
/*                                                                           */
/*                                 limits.h                                  */
/*                                                                           */
/*                          Sizes of integer types                           */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2002 Ullrich von Bassewitz                                       */
/*               Wacholderweg 14                                             */
/*               D-70597 Stuttgart                                           */
/* EMail:        uz@musoftware.de                                            */
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



#ifndef _LIMITS_H
#define _LIMITS_H



#define CHAR_BIT        8

#define SCHAR_MIN       ((signed char) 0x80)
#define SCHAR_MAX       127

#define UCHAR_MAX       255

#define CHAR_MIN        0
#define CHAR_MAX        255

#define SHRT_MIN        ((short) 0x8000)
#define SHRT_MAX        32767

#define USHRT_MAX       65535U

#define INT_MIN         ((int) 0x8000)
#define INT_MAX         32767

#define UINT_MAX        65535U

#define LONG_MAX        2147483647L
#define LONG_MIN        ((long) 0x80000000)

#define ULONG_MAX       4294967295UL

/* These defines that are platform dependent */
#if defined(__APPLE2__)
#  define PATH_MAX      (64+1)
#elif defined(__ATARI__)
#  define PATH_MAX      (63+1)
#elif defined(__CBM__)
#  define PATH_MAX      (255)  /* should be 256+1, see libsrc/common/_cmd.s why it's not */
#elif defined(__LUNIX__)
#  define PATH_MAX      (80+1)
#elif defined(__TELESTRAT__)
#  define PATH_MAX      (50+1)
#else
#  define PATH_MAX      (16+1)
#endif


/* End of limits.h */
#endif



