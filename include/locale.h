/*****************************************************************************/
/*                                                                           */
/*                                 locale.h                                  */
/*                                                                           */
/*                          Localization <locale.h>                          */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/* (C) 1998-2005 Ullrich von Bassewitz                                       */
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



#ifndef _LOCALE_H
#define _LOCALE_H



/* NULL pointer */
#ifndef _HAVE_NULL
#define NULL    0
#define _HAVE_NULL
#endif

/* Locale information constants */
#define LC_ALL          0
#define LC_COLLATE      1
#define LC_CTYPE        2
#define LC_MONETARY     3
#define LC_NUMERIC      4
#define LC_TIME         5

/* Struct containing locale settings */
struct lconv {
    char*       currency_symbol;
    char*       decimal_point;
    char*       grouping;
    char*       int_curr_symbol;
    char*       mon_decimal_point;
    char*       mon_grouping;
    char*       mon_thousands_sep;
    char*       negative_sign;
    char*       positive_sign;
    char*       thousands_sep;
    char        frac_digits;
    char        int_frac_digits;
    char        n_cs_precedes;
    char        n_sep_by_space;
    char        n_sign_posn;
    char        p_cs_precedes;
    char        p_sep_by_space;
    char        p_sign_posn;
};

/* Function prototypes */
struct lconv* localeconv (void);
char* __fastcall__ setlocale (int category, const char* locale);



/* End of locale.h */
#endif



