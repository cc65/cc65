/*****************************************************************************/
/*                                                                           */
/*                                 typecmp.h                                 */
/*                                                                           */
/*               Type compare function for the cc65 C compiler               */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2000 Ullrich von Bassewitz                                       */
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



#ifndef TYPECMP_H
#define TYPECMP_H



#include "datatype.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Degree of type compatibility. Must be in ascending order */
typedef enum {
    TC_INCOMPATIBLE,              /* Distinct types */
    TC_SIGN_DIFF,                 /* Signedness differs */
    TC_COMPATIBLE = TC_SIGN_DIFF, /* Compatible types */
    TC_QUAL_DIFF,                 /* Types differ in qualifier of pointer */
    TC_STRICT_COMPATIBLE,         /* Strict compatibility */
    TC_EQUAL,                     /* Types are equal */
    TC_IDENTICAL                  /* Types are identical */
} typecmp_t;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



typecmp_t TypeCmp (const Type* lhs, const Type* rhs);
/* Compare two types and return the result */



/* End of typecmp.h */

#endif
