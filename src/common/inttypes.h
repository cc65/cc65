/*****************************************************************************/
/*                                                                           */
/*                                inttypes.h                                 */
/*                                                                           */
/*                           Define integer types                            */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2004      Ullrich von Bassewitz                                       */
/*               Roemerstrasse 52                                            */
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



#ifndef INTTYPES_H
#define INTTYPES_H



/* If we have <stdint.h>, include it; otherwise, adapt types from <stddef.h>
** and define integer boundary constants.
** gcc and msvc don't define __STDC_VERSION__ without special flags, so check
** for them explicitly.  Undefined symbols are replaced by zero; so, checks for
** defined(__GNUC__) and defined(_MSC_VER) aren't necessary.
*/
#if (__STDC_VERSION__ >= 199901) || (__GNUC__ >= 3) || (_MSC_VER >= 1600)
#include <stdint.h>
#else

/* Assume that ptrdiff_t and size_t are wide enough to hold pointers.
** Assume that they are the widest type.
*/
#include <limits.h>
#include <stddef.h>

typedef ptrdiff_t intptr_t;
typedef size_t uintptr_t;
typedef ptrdiff_t intmax_t;
typedef size_t uintmax_t;

#define INT8_MAX (0x7F)
#define INT16_MAX (0x7FFF)
#define INT32_MAX (0x7FFFFFFF)

#define INT8_MIN (-INT8_MAX - 1)
#define INT16_MIN (-INT16_MAX - 1)
#define INT32_MIN (-INT32_MAX - 1)

#define UINT8_MAX (0xFF)
#define UINT16_MAX (0xFFFF)
#define UINT32_MAX (0xFFFFFFFF)

#if UCHAR_MAX == UINT8_MAX
typedef unsigned char uint8_t;
#else
#error "No suitable type for uint8_t found."
#endif

#if SCHAR_MIN == INT8_MIN && SCHAR_MAX == INT8_MAX
typedef signed char int8_t;
#else
#error "No suitable type for int8_t found."
#endif

#if UINT_MAX == UINT16_MAX
typedef unsigned int uint16_t;
#elif USHRT_MAX == UINT16_MAX
typedef unsigned short uint16_t;
#else
#error "No suitable type for uint16_t found."
#endif

#if INT_MIN == INT16_MIN && INT_MAX == INT16_MAX
typedef int int16_t;
#elif SHRT_MIN == INT16_MIN && SHRT_MAX == INT16_MAX
typedef short int16_t;
#else
#error "No suitable type for int16_t found."
#endif

#if UINT_MAX == UINT32_MAX
typedef unsigned int uint32_t;
#elif ULONG_MAX == UINT32_MAX
typedef unsigned long uint32_t;
#else
#error "No suitable type for uint32_t found."
#endif

#if INT_MIN == INT32_MIN && INT_MAX == INT32_MAX
typedef int int32_t;
#elif LONG_MIN == INT32_MIN && LONG_MAX == INT32_MAX
typedef long int32_t;
#else
#error "No suitable type for int32_t found."
#endif

#endif



/* End of inttypes.h */
#endif
