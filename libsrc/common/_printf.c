/*
 * Helper function for the printf family.
 */



#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <ctype.h>
#include "_printf.h"



/* Use static variables for locals */
#pragma staticlocals (1);



int _printf (struct outdesc* d, const char* f, va_list ap)
{
    outfunc fout;    		/* Output function */
    unsigned char type;	      	/* variable argument type */
    char str [20];    	      	/* string buffer */
    char c;    	     	 	/* Current format char */
    char leftjust;            	/* left justify string */
    char addsign;              	/* always add + or - */
    char addblank;   	      	/* add blank instead of + */
    char altform;      	      	/* alternate form? */
    char padchar;             	/* pad with space or zeroes? */
    char islong;       	        /* l modifier found */
    unsigned arglen;  		/* length of argument string */
    unsigned prec;		/* Precision */
    unsigned width;		/* Width of output field */
    int  i;			/* Integer value */
    long l;			/* Long value */
    char* sptr;	      	      	/* pointer to argument string */
    register char* s;          	/* work pointer to argument string */

    /* Remember the format string in a register variable for shorter code */
    register const char* format = f;

    /* Remember the output function in a local variable for speed and size */
    fout = d->fout;

    /* */
    d->ccount = 0;
    while (c = *format++) {

    	if (c != '%') {
       	    fout (d, &c, 1);
    	    continue;
      	}

	/* %%? */
	if (*format == '%') {
       	    fout (d, format, 1);
	    ++format;
	    continue;
	}

    	/* format is: %[flags][width][.precision][mod]type */

	/* flags */
	leftjust = addsign = addblank = altform = 0;
       	do {
	    switch (c = *format) {

       	        case '-':
	      	    leftjust = 1;
	    	    break;

	        case '+':
    	    	    addsign = 1;
	    	    break;

	        case '#':
	    	    altform = 1;
	    	    break;

	        case ' ':
	    	    addblank = 1;
	    	    break;

		default:
		    goto flags_done;

	    }
	    ++format;
	} while (1);
flags_done:

	/* width */
	padchar = ' ';
	if (*format == '0') {
	    padchar = '0';
	    ++format;
	}
 	if (*format == '*') {
	    width = va_arg (ap, int);
    	    ++format;
    	} else {
            width = 0;
    	    while (isdigit (c = *format)) {
    	    	width = width * 10 + (c - '0');
    	    	++format;
    	    }
    	}

    	/* precision */
        prec = 0;
    	if (*format == '.') {
    	    ++format;
    	    if (*format == '*') {
    	       	prec = va_arg (ap, int);
    	       	++format;
    	    } else {
    	      	while (isdigit (c = *format)) {
    	      	    prec = prec * 10 + (c - '0');
    	      	    ++format;
    	      	}
    	    }
    	}

    	/* modifiers */
	islong = 0;
       	while (strchr ("FNhlL", c = *format)) {
	    switch (c) {

	        case 'l':
	    	    islong = 1;
	   	    break;

	    }
    	    ++format;
    	}

    	/* Check the format specifier */
    	sptr = s = str;
	type = *format++;
       	switch (type) {

       	    case 'c':
    	      	str [0] = va_arg (ap, char);
    	      	str [1] = 0;
    	      	break;

       	    case 'd':
       	    case 'i':
		if (islong) {
		    l = va_arg (ap, long);
		    if (l >= 0) {
			if (addsign) {
			    *s++ = '+';
			} else if (addblank) {
			    *s++ = ' ';
			}
		    }
	   	    ltoa (l, s, 10);
		} else {
		    i = va_arg (ap, int);
		    if (i >= 0) {
			if (addsign) {
			    *s++ = '+';
			} else if (addblank) {
			    *s++ = ' ';
			}
		    }
    	   	    itoa (i, s, 10);
	   	}
    	    	break;

            case 'n':
    	    	*va_arg (ap, int*) = d->ccount;
    	   	continue;

       	    case 'o':
	   	if (islong) {
		    l = va_arg (ap, unsigned long);
		    if (altform && (l || prec)) {
		    	*s++ = '0';
		    }
       	       	    ultoa (l, s, 8);
	   	} else {
		    i = va_arg (ap, unsigned);
		    if (altform && (i || prec)) {
	    	    	*s++ = '0';
		    }
	       	    utoa (i, s, 8);
	   	}
	       	break;

       	    case 's':
	       	sptr = va_arg (ap, char*);
	       	break;

	    case 'u':
	    	if (islong) {
		    ultoa (va_arg (ap, unsigned long), str, 10);
		} else {
	       	    utoa (va_arg (ap, unsigned), str, 10);
		}
	       	break;

	    case 'x':
	    case 'X':
	       	if (altform) {
	       	    *s++ = '0';
	       	    *s++ = 'X';
	       	}
		if (islong) {
       	       	    ultoa (va_arg (ap, unsigned long), s, 16);
		} else {
	       	    utoa (va_arg (ap, unsigned), s, 16);
		}
	       	if (type == 'x') {
  	       	    strlower (str);
  	       	}
  	       	break;

 	    default:
	       	/* Unknown type char - skip it */
	       	continue;

	}

	/* Do argument string formatting */
	arglen = strlen (sptr);
	if (prec && prec < arglen) {
	    arglen = prec;
	}
       	if (width > arglen) {
   	    width -= arglen;  		/* padcount */
   	} else {
   	    width = 0;
   	}

   	/* Do padding on the left side if needed */
   	if (!leftjust) {
   	    /* argument right justified */
	    while (width) {
	    	fout (d, &padchar, 1);
		--width;
	    }
       	}

   	/* Output the argument string */
   	fout (d, sptr, arglen);

   	/* Output right padding bytes if needed */
   	if (leftjust) {
   	    /* argument left justified */
	    while (width) {
       	       	fout (d, &padchar, 1);
		--width;
	    }
   	}

    }
}



