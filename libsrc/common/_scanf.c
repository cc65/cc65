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
/*		      	      SetJmp return codes			     */
/*****************************************************************************/



#define RC_OK	      	0	     	/* Regular call */
#define RC_EOF	      	1	 	/* EOF reached */
#define RC_NOCONV     	2	 	/* No conversion possible */



/*****************************************************************************/
/*  		      		     Data				     */
/*****************************************************************************/



static struct indesc*	D;		/* Copy of function argument */
static va_list		ap;	 	/* Copy of function argument */
static jmp_buf 		JumpBuf; 	/* Label that is used in case of EOF */
static char    		C;     	      	/* Character from input */
static unsigned		Width;      	/* Maximum field width */
static long    	  	IntVal;	 	/* Converted int value */
static unsigned		Conversions;	/* Number of conversions */

/* Flags */
static unsigned char	Positive;  	/* Flag for positive value */
static unsigned char 	NoAssign;	/* Supppress assigment */
static unsigned char	IsShort;    	/* Short type */
static unsigned char	IsLong;     	/* Long type */



/*****************************************************************************/
/*  		       		Character sets				     */
/*****************************************************************************/



/*****************************************************************************/
/*  		       		     Code				     */
/*****************************************************************************/



static void ReadChar (void)
/* Get an input character, count characters */
{
    C = D->fin (D);
    ++D->ccount;
}



static void SkipWhite (void)
/* Skip white space in the input and return the first non white character */
{
    while (isspace (C)) {
	ReadChar ();
    }
}



static void ReadSign (void)
/* Read an optional sign and skip it. Store 1 in Positive if the value is
 * positive, store 0 otherwise.
 */
{
    switch (C) {
      	case '-':
       	    ReadChar ();
      	    Positive = 0;
      	case '+':
      	    ReadChar ();
      	    /* FALLTHROUGH */
      	default:
      	    Positive = 1;
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



static void ReadInt (unsigned char Base)
/* Read an integer and store it into IntVal */
{
    /* Value must start with a digit */
    if (!isdigit (C)) {
	longjmp (JumpBuf, RC_NOCONV);
    }

    /* Read the value */
    IntVal = 0;
    while (isxdigit (C) && Width-- > 0) {
	printf ("ReadInt: '%c'\n", C);
       	IntVal = IntVal * Base + HexVal (C);
	ReadChar ();
    }

    /* One more conversion */
    ++Conversions;
}



static void AssignInt (void)
/* Assign the integer value in Val to the next argument. The function makes
 * several non portable assumptions to reduce code size:
 *   - int and unsigned types have the same representation
 *   - short and int have the same representation.
 *   - all pointer types have the same representation.
 */
{
    if (!NoAssign) {
	/* Get the next argument pointer */
	void* P = va_arg (ap, void*);

	/* Assign to the converted value */
	if (IsLong) {
	    *(long*)P = IntVal;
	} else {
	    *(int*)P = (int) IntVal;
	}
    }
}



int _scanf (struct indesc* D_, const char* format, va_list ap_)
/* This is the routine used to do the actual work. It is called from several
 * types of wrappers to implement the actual ISO xxscanf functions.
 */
{
    char   	  F;   	     	/* Character from format string */
    unsigned char Result;    	/* setjmp result */
    char*	  S;
    unsigned char Base;	      	/* Integer base in %i */
    unsigned char HaveWidth;	/* True if a width was given */

    /* Place copies of the arguments into global variables. This is not very
     * nice, but on a 6502 platform it gives better code, since the values
     * do not have to be passed as parameters.
     */
    D 	= D_;
    ap	= ap_;

    /* Initialize variables */
    Conversions = 0;
    D->ccount   = 0;

    /* Set up the jump label. The get() routine will use this label when EOF
     * is reached.
     */
    Result = setjmp (JumpBuf);
    printf ("Result = %u\n", Result);
    if (Result == RC_OK) {

Again:
       	/* Get the next input character */
	ReadChar ();

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
	    	    SkipWhite ();
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
		HaveWidth   = 0;

      	    	/* Check for flags. */
	    	while (1) {
	    	    if (isdigit (F)) {
			HaveWidth = 1;
	    	 	Width     = 0;
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
		  	SkipWhite ();
      	      	    	ReadSign ();
      	      	    	ReadInt (10);
	      	    	if (!Positive) {
	      	    	    IntVal = -IntVal;
	      	    	}
			AssignInt ();
      	      	  	break;

      	      	    case 'i':
      	      	   	/* Optionally signed integer with a base */
			SkipWhite ();
			ReadSign ();
			if (C == '0') {
			    ReadChar ();
			    switch (C) {
			 	case 'x':
			 	case 'X':
			 	    Base = 16;
			 	    ReadChar();
			 	    break;
			 	default:
			 	    Base = 8;
			    }
			} else {
			    Base = 10;
			}
			ReadInt (Base);
			if (!Positive) {
			    IntVal = -IntVal;
			}
			AssignInt ();
      	      	  	break;

      	      	    case 'o':
      	      	  	/* Unsigned octal integer */
			SkipWhite ();
      	      	    	ReadInt (8);
			AssignInt ();
      	      		break;

      	      	    case 'u':
    	      		/* Unsigned decimal integer */
			SkipWhite ();
      	      	    	ReadInt (10);
			AssignInt ();
    	      		break;

	      	    case 'x':
	      	    case 'X':
	      		/* Unsigned hexadecimal integer */
			SkipWhite ();
      	      	    	ReadInt (16);
			AssignInt ();
	      		break;

	      	    case 'E':
	      	    case 'e':
    	      	    case 'f':
	      	    case 'g':
	      		/* Optionally signed float */
			longjmp (JumpBuf, RC_NOCONV);
	      		break;

	      	    case 's':
	      		/* Whitespace terminated string */
			SkipWhite ();
			if (!NoAssign) {
			    S = va_arg (ap, char*);
			}
       	       	       	while (!isspace (C) && Width--) {
			    if (!NoAssign) {
			       	*S++ = C;
			    }
			    ReadChar ();
			}
			/* Terminate the string just read */
			if (!NoAssign) {
			    *S = '\0';
			}
			break;

		    case 'c':
			/* Fixed length string, NOT zero terminated */
			if (!HaveWidth) {
			    /* No width given, default is 1 */
			    Width = 1;
			}
			if (!NoAssign) {
			    S = va_arg (ap, char*);
			}
			while (Width--) {
			    if (!NoAssign) {
			     	*S++ = C;
			    }
			    ReadChar ();
			}
			++Conversions;
			break;

		    case '[':
			/* String using characters from a set */
			break;

     		    case 'p':
     			/* Pointer */
     			break;

     		    case 'n':
     			/* Store characters consumed so far */
			IntVal = D->ccount;
			IsLong = 0;
			AssignInt ();
			break;

		    default:
			/* Invalid conversion */
			longjmp (JumpBuf, RC_NOCONV);
			break;

		}

		/* Skip the format char */
		goto Again;

	    }

     	}

    } else if (Result == RC_EOF) {

	/* Jump via JumpBuf means EOF on input */
	if (D->ccount == 0) {
	    /* Special case: error */
	    return -1;
	}

    }

    /* Return the number of conversions */
    return Conversions;
}



