/*
 * locale.h
 *
 * Ullrich von Bassewitz, 11.12.1998
 *
 */



#ifndef _LOCALE_H
#define _LOCALE_H



/* NULL pointer */
#ifdef NULL
#  undef NULL
#endif
#define NULL	0

/* Locale information constants */
#define LC_ALL		0
#define LC_COLLATE	1
#define LC_CTYPE	2
#define LC_MONETARY	3
#define LC_NUMERIC	4
#define LC_TIME		5

/* Struct containing locale settings */
struct lconv {
    char*	currency_symbol;
    char*	decimal_point;
    char*	grouping;
    char*	int_curr_symbol;
    char*	mon_decimal_point;
    char*	mon_grouping;
    char*	mon_thousands_sep;
    char*	negative_sign;
    char*	positive_sign;
    char*	thousands_sep;
    char	frac_digits;
    char	int_frac_digits;
    char	n_cs_precedes;
    char	n_sep_by_space;
    char	n_sign_posn;
    char	p_cs_precedes;
    char	p_sep_by_space;
    char	p_sign_posn;
};

/* Function prototypes */
struct lconv* localeconv (void);
char* setlocale (int category, const char* locale);



/* End of locale.h */
#endif



