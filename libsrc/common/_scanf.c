/*
 * _scanf.c
 *
 * (C) Copyright 2001 Ullrich von Bassewitz (uz@cc65.org)
 *
 * This is the basic layer for all scanf type functions.
 */



#include <stdio.h>
#include <stdarg.h>
#include <setjmp.h>
#include <ctype.h>
#include <limits.h>

#include "_scanf.h"



/*****************************************************************************/
/*			      SetJmp return codes			     */
/*****************************************************************************/



#define RC_OK		0	     	/* Regular call */
#define RC_EOF	  	1		/* EOF reached */
#define RC_NOCONV 	2		/* No conversion possible */



/*****************************************************************************/
/*  		  		     Data				     */
/*****************************************************************************/



static jmp_buf	JumpBuf;	/* Label that is used in case of EOF */
static char    	C;     	      	/* Character from input */
static unsigned	Width;      	/* Maximum field width */



/*****************************************************************************/
/*  		       		Character sets				     */
/*****************************************************************************/



/*****************************************************************************/
/*  		       		     Code				     */
/*****************************************************************************/



static void ReadChar (struct indesc* d)
/* Get an input character, count characters */
{
    C = d->fin (d);
    ++d->ccount;
}



static void SkipWhite (struct indesc* d)
/* Skip white space in the input and return the first non white character */
{
    while (isspace (C)) {
	ReadChar (d);
    }
}



static unsigned char ReadSign (struct indesc* d)
/* Read an optional sign and skip it. Return 1 if the value is positive,
 * return 0 otherwise.
 */
{
    switch (C) {
      	case '-':
       	    ReadChar (d);
      	    return 0;
      	case '+':
      	    ReadChar (d);
      	    /* FALLTHROUGH */
      	default:
      	    return 1;
    }
}



static unsigned char HexVal (char C)
/* Convert a digit to a value */
{

    if (isdigit (C)) {
    	return C - '0';
    } else {
	return C - toupper (C) + ('A' + 10);
    }
}



static unsigned long ReadInt (struct indesc* d, unsigned char Base)
/* Read an integer */
{
    unsigned long V = 0;

    /* Value must start with a digit */
    if (!isdigit (C)) {
	longjmp (JumpBuf, RC_NOCONV);
    }

    /* Read the value */
    while (isxdigit (C) && Width-- > 0) {
	printf ("ReadInt: '%c'\n", C);
       	V = V * Base + HexVal (C);
	ReadChar (d);
    }

    /* Return the read value */
    return V;
}



int _scanf (struct indesc* d, const char* format, va_list ap)
/* This is the routine used to do the actual work. It is called from several
 * types of wrappers to implement the actual ISO xxscanf functions.
 */
{
    unsigned	Conversions;	/* Number of conversions */

    char 	  F;   		/* Character from format string */
    unsigned char NoAssign;   	/* Supppress assigment */
    unsigned char IsShort;    	/* Short type */
    unsigned char IsLong;     	/* Long type */
    unsigned char Positive;	/* Flag for positive value */
    unsigned char Result;	/* setjmp result */

    /* Variables that hold intermediate values */
    void*	  P;
    long	  L;


    /* Initialize variables */
    Conversions = 0;
    d->ccount   = 0;

    /* Set up the jump label. The get() routine will use this label when EOF
     * is reached.
     */
    Result = setjmp (JumpBuf);
    printf ("Result = %u\n", Result);
    if (Result == RC_OK) {

Again:
       	/* Get the next input character */
	ReadChar (d);

	/* Walk over the format string */
      	while (F = *format++) {

	    /* Check for a conversion */
	    if (F != '%' || *format == '%') {

		/* %% or any char other than % */
		if (F == '%') {
		    ++format;
    		}

		/* Check for a match */
		if (isspace (F)) {

		    /* Special white space handling: Any whitespace matches
		     * any amount of whitespace including none(!). So this
		     * match will never fail.
		     */
		    SkipWhite (d);
		    continue;

		} else if (F != C) {

		    /* A mismatch. We will stop scanning the input and return
		     * the number of conversions.
		     */
		    printf ("F = '%c', C = '%c' --> mismatch\n", F, C);
		    return Conversions;

		} else {

		    /* A match. Read the next input character and start over */
		    goto Again;

		}

	    } else {

		/* A conversion. Skip the percent sign. */
		F = *format++;

	        /* Initialize variables */
		NoAssign    = 0;
		IsShort	    = 0;
		IsLong	    = 0;
		Width	    = UINT_MAX;

      		/* Check for flags. */
		while (1) {
		    if (isdigit (F)) {
			Width =	0;
			do {
			    /* ### Non portable ### */
		   	    Width = Width * 10 + (F & 0x0F);
    			    F = *format++;
    		     	} while (isdigit (F));
    		    } else {
    		     	switch (F) {
    		     	    case '*':	NoAssign = 1;	break;
    		     	    case 'h':	IsShort = 1;	break;
    		     	    case 'l':
    		     	    case 'L':	IsLong = 1;	break;
    		     	    default: 	goto FlagsDone;
    		     	}
    			F = *format++;
    		    }
    		}
FlagsDone:

    		/* Check for the actual conversion character */
		printf ("F = '%c'\n", F);
    		switch (F) {

    		    case 'D':
    		    	IsLong = 1;
    	      	    case 'd':
      	      	    	/* Optionally signed decimal integer */
			SkipWhite (d);
      	      	    	Positive = ReadSign (d);
      	      	    	L = ReadInt (d, 10);
	      	    	if (!Positive) {
	      	    	    L = -L;
	      	    	}
	      	    	if (!NoAssign) {
	      	    	    /* All pointers have the same size, so save some
	      	    	     * code here.
	      	    	     */
	      	    	    P =	va_arg (ap, void*);
	      	    	    if (IsLong) {
	      			*(long*)P = L;
	      		    } else {
	      			*(int*)P = (int) L;
	      		    }
	      		}
      	      		break;

      	      	    case 'i':
      	      	   	/* Optionally signed integer with a base */
      	      		break;

      	      	    case 'o':
      	      		/* Unsigned octal integer */
      	      	    	L = ReadInt (d, 8);
	      	    	if (!NoAssign) {
	      	    	    /* All pointers have the same size, so save some
	      	    	     * code here.
	      	    	     */
	      	    	    P =	va_arg (ap, void*);
	      	    	    if (IsLong) {
	      			*(long*)P = L;
	      		    } else {
	      			*(int*)P = (int) L;
	      		    }
	      		}
      	      		break;

      	      	    case 'u':
    	      		/* Unsigned decimal integer */
      	      	    	L = ReadInt (d, 10);
	      	    	if (!NoAssign) {
	      	    	    /* All pointers have the same size, so save some
	      	    	     * code here.
	      	    	     */
	      	    	    P =	va_arg (ap, void*);
	      	    	    if (IsLong) {
	      			*(long*)P = L;
	      		    } else {
	      			*(int*)P = (int) L;
	      		    }
	      		}
    	      		break;

	      	    case 'x':
	      	    case 'X':
	      		/* Unsigned hexadecimal integer */
      	      	    	L = ReadInt (d, 16);
	      	    	if (!NoAssign) {
	      	    	    /* All pointers have the same size, so save some
	      	    	     * code here.
	      	    	     */
	      	    	    P =	va_arg (ap, void*);
	      	    	    if (IsLong) {
	      			*(long*)P = L;
	      	     	    } else {
	      			*(int*)P = (int) L;
	      		    }
	      		}
	      		break;

	      	    case 'E':
	      	    case 'e':
    	      	    case 'f':
	      	    case 'g':
	      		/* Optionally signed float */
	      		break;

	      	    case 's':
	      		/* Whitespace terminated string */
			break;

		    case 'c':
			/* Fixed length string */
			break;

		    case '[':
			/* String using characters from a set */
			break;

		    case 'p':
			/* Pointer */
			break;

		    case 'n':
			/* Store characters consumed so far */
			break;

		    default:
			/* Invalid conversion */
			break;

		}

		/* Skip the format char */
		goto Again;

	    }

     	}

    } else if (Result == RC_EOF) {

	/* Jump via JumpBuf means EOF on input */
	if (d->ccount == 0) {
	    /* Special case: error */
	    return -1;
	}

    }

    /* Return the number of conversions */
    return Conversions;
}



