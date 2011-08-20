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



#ifndef GENTYPE_H
#define GENTYPE_H



/*****************************************************************************/
/*                                 Forwards                                  */
/*****************************************************************************/



struct StrBuf;



/*****************************************************************************/
/*     	      	       		     Data		   		     */
/*****************************************************************************/



/* The data type used to encode a generic type */
typedef unsigned char* gt_string;



/* Termination, so we can use string functions to handle type strings */
#define GT_END                  0x00U

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

/* Type of the data. Since we want to have zero as a terminator, we must
 * introduce one thing that cannot be zero for normal data. This is the
 * type.
 */
#define GT_INTEGER              0x20U
#define GT_POINTER              0x40U
#define GT_FLOAT                0x60U
#define GT_ARRAY                0x80U
#define GT_FUNCTION             0xA0U
#define GT_STRUCT               0xC0U
#define GT_UNION                0xE0U
#define GT_MASK                 0xE0U
#define GT_GET_TYPE(x)          ((x) & GT_TYPE_MASK)
#define GT_IS_INTEGER(x)        (GT_GET_TYPE(x) == GT_INTEGER)
#define GT_IS_POINTER(x)        (GT_GET_TYPE(x) == GT_POINTER)
#define GT_IS_FLOAT(x)          (GT_GET_TYPE(x) == GT_FLOAT)
#define GT_IS_ARRAY(x)          (GT_GET_TYPE(x) == GT_ARRAY)
#define GT_IS_FUNCTION(x)       (GT_GET_TYPE(x) == GT_FUNCTION)
#define GT_IS_STRUCT(x)         (GT_GET_TYPE(x) == GT_STRUCT)
#define GT_IS_UNION(x)          (GT_GET_TYPE(x) == GT_UNION)

/* Combined values for the 6502 family */
#define GT_BYTE         (GT_INTEGER | GT_LITTLE_ENDIAN | GT_UNSIGNED | GT_SIZE_1)
#define GT_WORD         (GT_INTEGER | GT_LITTLE_ENDIAN | GT_UNSIGNED | GT_SIZE_2)
#define GT_DWORD        (GT_INTEGER | GT_LITTLE_ENDIAN | GT_UNSIGNED | GT_SIZE_4)
#define GT_DBYTE        (GT_POINTER | GT_BIG_ENDIAN    | GT_UNSIGNED | GT_SIZE_2)
#define GT_PTR          (GT_POINTER | GT_LITTLE_ENDIAN | GT_UNSIGNED | GT_SIZE_2)
#define GT_FAR_PTR      (GT_POINTER | GT_LITTLE_ENDIAN | GT_UNSIGNED | GT_SIZE_3)



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



gt_string GT_FromStrBuf (const struct StrBuf* S);
/* Create a dynamically allocated type string from a string buffer */



/* End of gentype.h */

#endif




