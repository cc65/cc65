/*
** locale.c
**
** Ullrich von Bassewitz, 11.12.1998
*/



#include <locale.h>
#include <limits.h>



/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* For memory efficiency use a separate empty string */
static char EmptyString [] = "";

static struct lconv lc = {
    EmptyString,        /* currency_symbol */
    ".",                /* decimal_point */
    EmptyString,        /* grouping */
    EmptyString,        /* int_curr_symbol */
    EmptyString,        /* mon_decimal_point */
    EmptyString,        /* mon_grouping */
    EmptyString,        /* mon_thousands_sep */
    EmptyString,        /* negative_sign */
    EmptyString,        /* positive_sign */
    EmptyString,        /* thousands_sep */
    CHAR_MAX,           /* frac_digits */
    CHAR_MAX,           /* int_frac_digits */
    CHAR_MAX,           /* n_cs_precedes */
    CHAR_MAX,           /* n_sep_by_space */
    CHAR_MAX,           /* n_sign_posn */
    CHAR_MAX,           /* p_cs_precedes */
    CHAR_MAX,           /* p_sep_by_space */
    CHAR_MAX,           /* p_sign_posn */
};



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



struct lconv* localeconv (void)
{
    return &lc;
}



char* __fastcall__ setlocale (int, const char* locale)
{
    if (locale == 0 || (locale [0] == 'C' && locale [1] == '\0') || locale [0] == '\0') {
        /* No change, or value already set, our locale is the "C" locale */
        return "C";
    } else {
        /* Cannot set this one */
        return 0;
    }
}




