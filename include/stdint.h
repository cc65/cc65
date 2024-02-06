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

#define INT8_MIN            ((int8_t) 0x80)
#define INT8_MAX            ((int8_t) 0x7F)
#define INT16_MIN           ((int16_t) 0x8000)
#define INT16_MAX           ((int16_t) 0x7FFF)
#define INT32_MIN           ((int32_t) 0x80000000)
#define INT32_MAX           ((int32_t) 0x7FFFFFFF)
#define UINT8_MAX           ((uint8_t) 0xFF)
#define UINT16_MAX          ((uint16_t) 0xFFFF)
#define UINT32_MAX          ((uint32_t) 0xFFFFFFFF)

/* Minimum-width integer types */
typedef signed char         int_least8_t;
typedef int                 int_least16_t;
typedef long                int_least32_t;
typedef unsigned char       uint_least8_t;
typedef unsigned            uint_least16_t;
typedef unsigned long       uint_least32_t;

#define INT_LEAST8_MIN      ((int_least8_t) 0x80)
#define INT_LEAST8_MAX      ((int_least8_t) 0x7F)
#define INT_LEAST16_MIN     ((int_least16_t) 0x8000)
#define INT_LEAST16_MAX     ((int_least16_t) 0x7FFF)
#define INT_LEAST32_MIN     ((int_least32_t) 0x80000000)
#define INT_LEAST32_MAX     ((int_least32_t) 0x7FFFFFFF)
#define UINT_LEAST8_MAX     ((uint_least8_t) 0xFF)
#define UINT_LEAST16_MAX    ((uint_least16_t) 0xFFFF)
#define UINT_LEAST32_MAX    ((uint_least32_t) 0xFFFFFFFF)

/* Fastest minimum-width integer types */
typedef signed char         int_fast8_t;
typedef int                 int_fast16_t;
typedef long                int_fast32_t;
typedef unsigned char       uint_fast8_t;
typedef unsigned            uint_fast16_t;
typedef unsigned long       uint_fast32_t;

#define INT_FAST8_MIN       ((int_fast8_t) 0x80)
#define INT_FAST8_MAX       ((int_fast8_t) 0x7F)
#define INT_FAST16_MIN      ((int_fast16_t) 0x8000)
#define INT_FAST16_MAX      ((int_fast16_t) 0x7FFF)
#define INT_FAST32_MIN      ((int_fast32_t) 0x80000000)
#define INT_FAST32_MAX      ((int_fast32_t) 0x7FFFFFFF)
#define UINT_FAST8_MAX      ((uint_fast8_t) 0xFF)
#define UINT_FAST16_MAX     ((uint_fast16_t) 0xFFFF)
#define UINT_FAST32_MAX     ((uint_fast32_t) 0xFFFFFFFF)

/* Integer types capable of holding object pointers */
typedef int                 intptr_t;
typedef unsigned            uintptr_t;

#define INTPTR_MIN          ((intptr_t)0x8000)
#define INTPTR_MAX          ((intptr_t)0x7FFF)
#define UINTPTR_MAX         ((uintptr_t) 0xFFFF)

/* Greatest width integer types */
typedef long                intmax_t;
typedef unsigned long       uintmax_t;

#define INTMAX_MIN          ((intmax_t) 0x80000000)
#define INTMAX_MAX          ((intmax_t) 0x7FFFFFFF)
#define UINTMAX_MAX         ((uintmax_t) 0xFFFFFFFF)

/* Limits of other integer types */
#define PTRDIFF_MIN         ((int) 0x8000)
#define PTRDIFF_MAX         ((int) 0x7FFF)

#define SIG_ATOMIC_MIN      ((unsigned char) 0x00)
#define SIG_ATOMIC_MAX      ((unsigned char) 0xFF)

#define SIZE_MAX            0xFFFF

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



