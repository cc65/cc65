/*****************************************************************************/
/*                                                                           */
/*                                 feature.c                                 */
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



#include <string.h>

/* ca65 */
#include "global.h"
#include "feature.h"



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Names of the features */
static const char* const FeatureKeys[FEAT_COUNT] = {
    "dollar_is_pc",
    "labels_without_colons",
    "loose_string_term",
    "loose_char_term",
    "at_in_identifiers",
    "dollar_in_identifiers",
    "leading_dot_in_identifiers",
    "org_per_seg",
    "pc_assignment",
    "missing_char_term",
    "ubiquitous_idents",
    "c_comments",
    "force_range",
    "underline_in_numbers",
    "addrsize",
    "bracket_as_indirect",
    "string_escapes",
    "long_jsr_jmp_rts",
};



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



feature_t FindFeature (const StrBuf* Key)
/* Find the feature in a table and return the corresponding enum value. If the
** feature is invalid, return FEAT_UNKNOWN.
*/
{
    feature_t F;

    /* This is not time critical, so do a linear search */
    for (F = (feature_t) 0; F < FEAT_COUNT; ++F) {
        if (SB_CompareStr (Key, FeatureKeys[F]) == 0) {
            /* Found, index is enum value */
            return F;
        }
    }

    /* Not found */
    return FEAT_UNKNOWN;
}



feature_t SetFeature (const StrBuf* Key)
/* Find the feature and set the corresponding flag if the feature is known.
** In any case, return the feature found. An invalid Key will return
** FEAT_UNKNOWN.
*/
{
    /* Map the string to an enum value */
    feature_t Feature = FindFeature (Key);

    /* Set the flags */
    switch (Feature) {
        case FEAT_DOLLAR_IS_PC:               DollarIsPC        = 1;    break;
        case FEAT_LABELS_WITHOUT_COLONS:      NoColonLabels     = 1;    break;
        case FEAT_LOOSE_STRING_TERM:          LooseStringTerm   = 1;    break;
        case FEAT_LOOSE_CHAR_TERM:            LooseCharTerm     = 1;    break;
        case FEAT_AT_IN_IDENTIFIERS:          AtInIdents        = 1;    break;
        case FEAT_DOLLAR_IN_IDENTIFIERS:      DollarInIdents    = 1;    break;
        case FEAT_LEADING_DOT_IN_IDENTIFIERS: LeadingDotInIdents= 1;    break;
        case FEAT_ORG_PER_SEG:                OrgPerSeg         = 1;    break;
        case FEAT_PC_ASSIGNMENT:              PCAssignment      = 1;    break;
        case FEAT_MISSING_CHAR_TERM:          MissingCharTerm   = 1;    break;
        case FEAT_UBIQUITOUS_IDENTS:          UbiquitousIdents  = 1;    break;
        case FEAT_C_COMMENTS:                 CComments         = 1;    break;
        case FEAT_FORCE_RANGE:                ForceRange        = 1;    break;
        case FEAT_UNDERLINE_IN_NUMBERS:       UnderlineInNumbers= 1;    break;
        case FEAT_ADDRSIZE:                   AddrSize          = 1;    break;
        case FEAT_BRACKET_AS_INDIRECT:        BracketAsIndirect = 1;    break;
        case FEAT_STRING_ESCAPES:             StringEscapes     = 1;    break;
        case FEAT_LONG_JSR_JMP_RTS:           LongJsrJmpRts     = 1;    break;
        default:                         /* Keep gcc silent */          break;
    }

    /* Return the value found */
    return Feature;
}
