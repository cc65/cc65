/*****************************************************************************/
/*                                                                           */
/*                                 gentype.h                                 */
/*                                                                           */
/*                        Generic data type encoding                         */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2011,      Ullrich von Bassewitz                                      */
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



/* This module implements a specification for a "generic data type". It is
** called generic, because it doesn't mimic a high level language. Instead it
** tries to desrcibe the type as representation on the machine.
** The reasoning behing this type is to allow a debugger to represent the
** data to the user, independent of the actual source.
** C data types may be mapped to generic ones, but attributes like const or
** volatile are (of course) lost.
**
** The data type is stored in a char array; and, can be terminated by a zero
** (see definition of GT_END below). The latter is not really necessary; but,
** allows handling of types using the usual string functions. This is in fact
** one of the advantages of the chosen implementation:
** String buffers may be used to dynamically build types. Types may be stored
** as strings in the string pool of an object file. And a string pool may be
** used to remove duplicates and reference types using unique ids.
*/



#ifndef GENTYPE_H
#define GENTYPE_H



/* common */
#include "strbuf.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Size of a data type */
#define GT_SIZE_1               0x00U
#define GT_SIZE_2               0x01U
#define GT_SIZE_3               0x02U
#define GT_SIZE_4               0x03U
#define GT_SIZE_MASK            0x07U

#define GT_GET_SIZE(x)          (((x) & GT_SIZE_MASK) + 1U)

/* Sign of the data type */
#define GT_UNSIGNED             0x00U
#define GT_SIGNED               0x08U
#define GT_SIGN_MASK            0x08U

#define GT_HAS_SIGN(x)          (((x) & GT_SIZE_MASK) == GT_SIGNED)

/* Byte order */
#define GT_LITTLE_ENDIAN        0x00U
#define GT_BIG_ENDIAN           0x10U
#define GT_BYTEORDER_MASK       0x10U

#define GT_IS_LITTLE_ENDIAN(x)  (((x) & GT_BYTEORDER_MASK) == GT_LITTLE_ENDIAN)
#define GT_IS_BIG_ENDIAN(x)     (((x) & GT_BYTEORDER_MASK) == GT_BIG_ENDIAN)

/* Type of the data. */
#define GT_TYPE_VOID            0x00U
#define GT_TYPE_INT             0x20U
#define GT_TYPE_PTR             0x40U
#define GT_TYPE_FLOAT           0x60U
#define GT_TYPE_ARRAY           0x80U
#define GT_TYPE_FUNC            0xA0U
#define GT_TYPE_STRUCT          0xC0U
#define GT_TYPE_UNION           0xE0U
#define GT_TYPE_MASK            0xE0U

#define GT_GET_TYPE(x)          ((x) & GT_TYPE_MASK)
#define GT_IS_INTEGER(x)        (GT_GET_TYPE(x) == GT_TYPE_INTEGER)
#define GT_IS_POINTER(x)        (GT_GET_TYPE(x) == GT_TYPE_POINTER)
#define GT_IS_FLOAT(x)          (GT_GET_TYPE(x) == GT_TYPE_FLOAT)
#define GT_IS_ARRAY(x)          (GT_GET_TYPE(x) == GT_TYPE_ARRAY)
#define GT_IS_FUNCTION(x)       (GT_GET_TYPE(x) == GT_TYPE_FUNCTION)
#define GT_IS_STRUCT(x)         (GT_GET_TYPE(x) == GT_TYPE_STRUCT)
#define GT_IS_UNION(x)          (GT_GET_TYPE(x) == GT_TYPE_UNION)

/* Combined values for the 6502 family */
#define GT_VOID         (GT_TYPE_VOID)
#define GT_BYTE         (GT_TYPE_INT | GT_LITTLE_ENDIAN | GT_UNSIGNED | GT_SIZE_1)
#define GT_WORD         (GT_TYPE_INT | GT_LITTLE_ENDIAN | GT_UNSIGNED | GT_SIZE_2)
#define GT_DWORD        (GT_TYPE_INT | GT_LITTLE_ENDIAN | GT_UNSIGNED | GT_SIZE_4)
#define GT_DBYTE        (GT_TYPE_PTR | GT_BIG_ENDIAN    | GT_UNSIGNED | GT_SIZE_2)
#define GT_PTR          (GT_TYPE_PTR | GT_LITTLE_ENDIAN | GT_UNSIGNED | GT_SIZE_2)
#define GT_FAR_PTR      (GT_TYPE_PTR | GT_LITTLE_ENDIAN | GT_UNSIGNED | GT_SIZE_3)
#define GT_ARRAY(size)  (GT_TYPE_ARRAY | ((size) - 1))



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



void GT_AddArray (StrBuf* Type, unsigned ArraySize);
/* Add an array with the given size to the type string in Type. This will
** NOT add the element type!
*/

unsigned GT_GetElementCount (StrBuf* Type);
/* Retrieve the element count of an array stored in Type at the current index
** position. Note: Index must point to the array token itself, since the size
** of the element count is encoded there. The index position will get moved
** past the array.
*/

const char* GT_AsString (const StrBuf* Type, StrBuf* String);
/* Convert the type into a readable representation. The target string buffer
** will be zero terminated and a pointer to the contents are returned.
*/



/* End of gentype.h */

#endif
