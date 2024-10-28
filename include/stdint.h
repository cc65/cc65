/*****************************************************************************/
/*                                                                           */
/*                                 stdint.h                                  */
/*                                                                           */
/*                          Standard integer types                           */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2002      Ullrich von Bassewitz                                       */
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



/* Note: This file is not fully ISO 9899-1999 compliant because cc65 lacks
** a 64 bit data types. The declarations have been adjusted accordingly.
*/



#ifndef _STDINT_H
#define _STDINT_H



/* Exact-width integer types */
typedef signed char         int8_t;
typedef int                 int16_t;
typedef long                int32_t;
typedef unsigned char       uint8_t;
typedef unsigned            uint16_t;
typedef unsigned long       uint32_t;

#define INT8_MIN            -128
#define INT8_MAX            127
#define INT16_MIN           (-32767 - 1)
#define INT16_MAX           32767
#define INT32_MIN           (-2147483647L - 1L)
#define INT32_MAX           2147483647L
#define UINT8_MAX           255
#define UINT16_MAX          65535U
#define UINT32_MAX          4294967295UL

/* Minimum-width integer types */
typedef signed char         int_least8_t;
typedef int                 int_least16_t;
typedef long                int_least32_t;
typedef unsigned char       uint_least8_t;
typedef unsigned            uint_least16_t;
typedef unsigned long       uint_least32_t;

#define INT_LEAST8_MIN      INT8_MIN
#define INT_LEAST8_MAX      INT8_MAX
#define INT_LEAST16_MIN     INT16_MIN
#define INT_LEAST16_MAX     INT16_MAX
#define INT_LEAST32_MIN     INT32_MIN
#define INT_LEAST32_MAX     INT32_MAX
#define UINT_LEAST8_MAX     UINT8_MAX
#define UINT_LEAST16_MAX    UINT16_MAX
#define UINT_LEAST32_MAX    UINT32_MAX

/* Fastest minimum-width integer types */
typedef signed char         int_fast8_t;
typedef int                 int_fast16_t;
typedef long                int_fast32_t;
typedef unsigned char       uint_fast8_t;
typedef unsigned            uint_fast16_t;
typedef unsigned long       uint_fast32_t;

#define INT_FAST8_MIN       INT8_MIN
#define INT_FAST8_MAX       INT8_MAX
#define INT_FAST16_MIN      INT16_MIN
#define INT_FAST16_MAX      INT16_MAX
#define INT_FAST32_MIN      INT32_MIN
#define INT_FAST32_MAX      INT32_MAX
#define UINT_FAST8_MAX      UINT8_MAX
#define UINT_FAST16_MAX     UINT16_MAX
#define UINT_FAST32_MAX     UINT32_MAX

/* Integer types capable of holding object pointers */
typedef int                 intptr_t;
typedef unsigned            uintptr_t;

#define INTPTR_MIN          INT16_MIN
#define INTPTR_MAX          INT16_MAX
#define UINTPTR_MAX         UINT16_MAX

/* Greatest width integer types */
typedef long                intmax_t;
typedef unsigned long       uintmax_t;

#define INTMAX_MIN          INT32_MIN
#define INTMAX_MAX          INT32_MAX
#define UINTMAX_MAX         UINT32_MAX

/* Limits of other integer types */
#define PTRDIFF_MIN         INT16_MIN
#define PTRDIFF_MAX         INT16_MAX

#define SIG_ATOMIC_MIN      0
#define SIG_ATOMIC_MAX      UINT8_MAX

#define SIZE_MAX            UINT16_MAX

/* Macros for minimum width integer constants */
#define INT8_C(c)           c
#define INT16_C(c)          c
#define INT32_C(c)          c##L
#define UINT8_C(c)          c##U
#define UINT16_C(c)         c##U
#define UINT32_C(c)         c##UL

/* Macros for greatest width integer constants */
#define INTMAX_C(c)         c##L
#define UINTMAX_C(c)        c##UL



/* End of stdint.h */
#endif



