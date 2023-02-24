/*****************************************************************************/
/*                                                                           */
/*                                 feature.h                                 */
/*                                                                           */
/*                  Subroutines for the emulation features                   */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 2000-2013, Ullrich von Bassewitz                                      */
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



#ifndef FEATURE_H
#define FEATURE_H



/* common */
#include "strbuf.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



typedef enum {
    FEAT_UNKNOWN                = -1,
    FEAT_DOLLAR_IS_PC,
    FEAT_LABELS_WITHOUT_COLONS,
    FEAT_LOOSE_STRING_TERM,
    FEAT_LOOSE_CHAR_TERM,
    FEAT_AT_IN_IDENTIFIERS,
    FEAT_DOLLAR_IN_IDENTIFIERS,
    FEAT_LEADING_DOT_IN_IDENTIFIERS,
    FEAT_ORG_PER_SEG,
    FEAT_PC_ASSIGNMENT,
    FEAT_MISSING_CHAR_TERM,
    FEAT_UBIQUITOUS_IDENTS,
    FEAT_C_COMMENTS,
    FEAT_FORCE_RANGE,
    FEAT_UNDERLINE_IN_NUMBERS,
    FEAT_ADDRSIZE,
    FEAT_BRACKET_AS_INDIRECT,
    FEAT_STRING_ESCAPES,
    FEAT_LONG_JSR_JMP_RTS,

    /* Special value: Number of features available */
    FEAT_COUNT
} feature_t;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



feature_t FindFeature (const StrBuf* Key);
/* Find the feature in a table and return the corresponding enum value. If the
** feature is invalid, return FEAT_UNKNOWN.
*/

feature_t SetFeature (const StrBuf* Key);
/* Find the feature and set the corresponding flag if the feature is known.
** In any case, return the feature found. An invalid Key will return
** FEAT_UNKNOWN.
*/



/* End of feature.h */

#endif
