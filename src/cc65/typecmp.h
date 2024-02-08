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



/* Degree of type compatibility affected. Must be in ascending order */
typedef enum {
    TC_INCOMPATIBLE,        /* Distinct types */
    TC_SIGN_DIFF,           /* Signedness differs */
    TC_PTR_SIGN_DIFF,       /* Pointee signedness differs */
    TC_PTR_INCOMPATIBLE,    /* Distinct pointer types */
    TC_VOID_PTR,            /* Non-void and void pointers */
    TC_STRICT_COMPATIBLE,   /* Strict compatibility according to the C Standard */
    TC_PTR_DECAY,           /* rhs is an array and lhs is a pointer */
    TC_EQUAL,               /* Array types with unspecified lengths */
    TC_UNSPECIFY,           /* lhs has unspecified length while rhs has specified length */
    TC_IDENTICAL            /* Types are identical */
} typecmpcode_t;

/* Degree of type compatibility affected by qualifiers as well as some extra info */
typedef enum {
    TCF_NONE                = 0x00, /* None of the below */
    TCF_VOID_PTR_ON_LEFT    = 0x01, /* lhs is a void pointer */
    TCF_VOID_PTR_ON_RIGHT   = 0x02, /* rhs is a void pointer */
    TCF_MASK_VOID_PTR       = TCF_VOID_PTR_ON_LEFT | TCF_VOID_PTR_ON_RIGHT,
    TCF_QUAL_DIFF           = 0x04, /* lhs doesn't have all of CVR qualifiers of rhs */
    TCF_QUAL_IMPLICIT       = 0x08, /* CVR qualifiers of lhs are stricter than those of rhs */
    TCF_MASK_CVR_DIFF       = 0x0C, /* All CVR qualifiers */
    TCF_PTR_QUAL_DIFF       = 0x10, /* lhs pointee doesn't have all of CVR qualifiers of rhs pointee */
    TCF_PTR_QUAL_IMPLICIT   = 0x20, /* CVR qualifiers of pointees are stricter on lhs than those on rhs */
    TCF_MASK_PTR_QUAL_DIFF  = 0x30, /* All CVR qualifiers of pointees */
    TCF_ADDRSIZE_QUAL_DIFF  = 0x40, /* Address size qualifiers differ */
    TCF_CCONV_QUAL_DIFF     = 0x80, /* Function calling conventions differ. Unused now */
    TCF_INCOMPATIBLE_QUAL   = TCF_ADDRSIZE_QUAL_DIFF | TCF_CCONV_QUAL_DIFF,
    TCF_MASK_PARAM_DIFF     = TCF_MASK_PTR_QUAL_DIFF | TCF_INCOMPATIBLE_QUAL,
    TCF_MASK_QUAL           = TCF_MASK_CVR_DIFF | TCF_MASK_PTR_QUAL_DIFF | TCF_INCOMPATIBLE_QUAL,
} typecmpflag_t;

typedef struct {
    typecmpcode_t C;
    typecmpflag_t F;
    int Indirections;
} typecmp_t;

#define TYPECMP_INITIALIZER { TC_IDENTICAL, TCF_NONE, 0 }

/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



typecmp_t TypeCmp (const Type* lhs, const Type* rhs);
/* Compare two types and return the result */

void TypeCompatibilityDiagnostic (const Type* NewType, const Type* OldType, int IsError, const char* Msg);
/* Print error or warning message about type compatibility with proper type
** names. The format string shall contain two '%s' specifiers for the names of
** the two types.
*/



/* End of typecmp.h */

#endif
