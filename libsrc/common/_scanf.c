/*
 * _scanf.c
 *
 * (C) Copyright 2001-2002 Ullrich von Bassewitz (uz@cc65.org)
 *
 * This is the basic layer for all scanf type functions. It should get
 * rewritten in assembler at some time in the future, so most of the code
 * is not as elegant as it could be.
 */



#include <stdio.h>
#include <stdarg.h>
#include <string.h>
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



static struct scanfdata*  D;		/* Copy of function argument */
static va_list		  ap;	 	/* Copy of function argument */
static jmp_buf 		  JumpBuf; 	/* Label that is used in case of EOF */
static int                C;           	/* Character from input */
static unsigned		  Width;      	/* Maximum field width */
static long    	  	  IntVal;	/* Converted int value */
static unsigned		  Conversions;	/* Number of conversions */
static unsigned char      IntBytes;     /* Number of bytes-1 for int conversions */

/* Flags */
static unsigned char   	  Positive;  	/* Flag for positive value */
static unsigned char   	  NoAssign;	/* Supppress assigment */
static unsigned char      Invert;       /* Do we need to invert the charset? */
static unsigned char      CharSet[32];  /* 32 * 8 bits = 256 bits */
static const unsigned char Bits[8] = {
    0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80
};



/*****************************************************************************/
/*  		       	    	Character sets				     */
/*****************************************************************************/



static void AddCharToSet (unsigned char C)
/* Set the given bit in the character set */
{
    asm ("ldy #%o", C);
    asm ("lda (sp),y");
    asm ("lsr a");
    asm ("lsr a");
    asm ("lsr a");
    asm ("tax");
    asm ("lda (sp),y");
    asm ("and #$07");
    asm ("tay");
    asm ("lda %v,y", Bits);
    asm ("ora %v,x", CharSet);
    asm ("sta %v,x", CharSet);
}



static unsigned char IsCharInSet (unsigned char C)
/* Check if the given char is part of the character set */
{
    asm ("ldy #%o", C);
    asm ("lda (sp),y");
    asm ("lsr a");
    asm ("lsr a");
    asm ("lsr a");
    asm ("tax");
    asm ("lda (sp),y");
    asm ("and #$07");
    asm ("tay");
    asm ("lda %v,y", Bits);
    asm ("and %v,x", CharSet);
    asm ("ldx #$00");
    return __AX__;
}



static void InvertCharSet (void)
/* Invert the character set */
{
    asm ("ldy #%b", sizeof (CharSet) - 1);
    asm ("L1:");
    asm ("lda %v,y", CharSet);
    asm ("eor #$FF");
    asm ("sta %v,y", CharSet);
    asm ("dey");
    asm ("bpl L1");
}



/*****************************************************************************/
/*  		       	 	     Code				     */
/*****************************************************************************/



static void ReadChar (void)
/* Get an input character, count characters */
{
    C = D->get (D->data);
    if (C != EOF) {
        ++D->ccount;
    }
}



static void ReadCharWithCheck (void)
/* Get an input char, use longjmp in case of EOF */
{
    ReadChar ();
    if (C == EOF) {
     	longjmp (JumpBuf, RC_EOF);
    }
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
            break;
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
	return toupper (C) - ('A' - 10);
    }
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
        __AX__ = (unsigned) va_arg (ap, void*);

        /* Store the argument pointer into ptr1 */
        asm ("sta ptr1");
        asm ("stx ptr1+1");

        /* Get the number of bytes-1 to copy */
        asm ("ldy %v", IntBytes);

        /* Assign the integer value */
        asm ("L1: lda %v,y", IntVal);
        asm ("sta (ptr1),y");
        asm ("dey");
        asm ("bpl L1");

    }
}



static unsigned char ReadInt (unsigned char Base)
/* Read an integer and store it into IntVal. Returns the number of chars
 * converted. Does NOT bump Conversions.
 */
{
    unsigned char Val;
    unsigned char CharCount = 0;

    /* Read the integer value */
    IntVal = 0;
    while (isxdigit (C) && Width-- > 0 && (Val = HexVal (C)) < Base) {
        ++CharCount;
       	IntVal = IntVal * Base + Val;
     	ReadChar ();
    }

    /* If we didn't convert anything, it's an error */
    if (CharCount == 0) {
     	longjmp (JumpBuf, RC_NOCONV);
    }

    /* Return the number of characters converted */
    return CharCount;
}



static void ScanInt (unsigned char Base)
/* Scan an integer including white space, sign and optional base spec,
 * and store it into IntVal.
 */
{
    /* Skip whitespace */
    SkipWhite ();

    /* Read an optional sign */
    ReadSign ();

    /* If Base is unknown (zero), figure it out */
    if (Base == 0) {
        if (C == '0') {
            ReadChar ();
            switch (C) {
                case 'x':
                case 'X':
                    Base = 16;
                    ReadChar ();
                    break;
                default:
                    Base = 8;
            }
        } else {
            Base = 10;
        }
    }

    /* Read the integer value */
    ReadInt (Base);

    /* Apply the sign */
    if (!Positive) {
        IntVal = -IntVal;
    }

    /* Assign the value to the next argument unless suppressed */
    AssignInt ();

    /* One more conversion */
    ++Conversions;
}



int _scanf (struct scanfdata* D_, register const char* format, va_list ap_)
/* This is the routine used to do the actual work. It is called from several
 * types of wrappers to implement the actual ISO xxscanf functions.
 */
{
    char   	  F;   	     	/* Character from format string */
    unsigned char Result;    	/* setjmp result */
    char*	  S;
    unsigned char HaveWidth;	/* True if a width was given */
    char          Start;        /* Start of range */

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

	    	    /* Special white space handling: Any whitespace in the
                     * format string matches any amount of whitespace including
                     * none(!). So this match will never fail.
	    	     */
	    	    SkipWhite ();
	    	    continue;

	    	} else if (F == C) {

	    	    /* A match. Read the next input character and start over */
	    	    goto Again;

    	    	} else {

	    	    /* A mismatch. We will stop scanning the input and return
	    	     * the number of conversions.
	    	     */
	    	    return Conversions;

	    	}

	    } else {

	    	/* A conversion. Skip the percent sign. */
	    	F = *format++;

      	    	/* 1. Assignment suppression */
                if (F == '*') {
                    F = *format++;
                    NoAssign = 1;
                } else {
                    NoAssign = 0;
                }

                /* 2. Maximum field width */
       	       	Width  	  = UINT_MAX;
	    	HaveWidth = 0;
                if (isdigit (F)) {
                    HaveWidth = 1;
                    Width     = 0;
                    do {
                        /* ### Non portable ### */
                        Width = Width * 10 + (F & 0x0F);
                        F = *format++;
                    } while (isdigit (F));
                }

                /* 3. Length modifier */
                IntBytes = sizeof(int) - 1;
                switch (F) {
                    case 'h':
                        if (*format == 'h') {
                            IntBytes = sizeof(char) - 1;
                            ++format;
                        }
                        F = *format++;
                        break;

                    case 'l':
                        if (*format == 'l') {
                            /* Treat long long as long */
                            ++format;
                        }
                        /* FALLTHROUGH */
                    case 'j':   /* intmax_t */
                        IntBytes = sizeof(long) - 1;
                        F = *format++;
                        break;

                    case 'z':   /* size_t */
                    case 't':   /* ptrdiff_t */
                    case 'L':   /* long double - ignore this one */
                        F = *format++;
                        break;
                }

                /* 4. Conversion specifier */
    		switch (F) {

                    /* 'd' and 'u' conversions are actually the same, since the
                     * standard says that evene the 'u' modifier allows an
                     * optionally signed integer.
                     */
    	      	    case 'd':   /* Optionally signed decimal integer */
                    case 'u':
      	      	      	ScanInt (10);
      	      	      	break;

      	      	    case 'i':
      	      	   	/* Optionally signed integer with a base */
			ScanInt (0);
      	      	  	break;

      	      	    case 'o':
      	      	  	/* Optionally signed octal integer */
      	      	      	ScanInt (8);
      	      		break;

	      	    case 'x':
	      	    case 'X':
	      	 	/* Optionally signed hexadecimal integer */
      	      	    	ScanInt (16);
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
                        ++Conversions;
    			break;

		    case 'c':
			/* Fixed length string, NOT zero terminated */
			if (!HaveWidth) {
			    /* No width given, default is 1 */
			    Width = 1;
			}
			if (!NoAssign) {
			    S = va_arg (ap, char*);
                            while (Width--) {
                                *S++ = C;
                                ReadCharWithCheck ();
                            }
			} else {
                            /* Just skip as many chars as given */
                            while (Width--) {
                                ReadCharWithCheck ();
                            }
                        }
			++Conversions;
		   	break;

		    case '[':
			/* String using characters from a set */
                        Invert = 0;
                        /* Clear the set */
                        memset (CharSet, 0, sizeof (CharSet));
                        F = *format++;
                        if (F == '^') {
                            Invert = 1;
                            F = *format++;
                        }
                        if (F == ']') {
                            AddCharToSet (']');
                            F = *format++;
                        }
                        /* Read the characters that are part of the set */
                        while (F != ']' && F != '\0') {
                            if (*format == '-') {
                                /* A range. Get start and end, skip the '-' */
                                Start = F;
                                F = *++format;
                                ++format;
                                if (F == ']') {
                                    /* '-' as last char means: include '-' */
                                    AddCharToSet (Start);
                                    AddCharToSet ('-');
                                } else if (F != '\0') {
                                    /* Include all chars in the range */
                                    while (1) {
                                        AddCharToSet (Start);
                                        if (Start == F) {
                                            break;
                                        }
                                        ++Start;
                                    }
                                    /* Get next char after range */
                                    F = *format++;
                                }
                            } else {
                                /* Just a character */
                                AddCharToSet (F);
                                /* Get next char */
                                F = *format++;
                            }
                        }

                        /* Invert the set if requested */
                        if (Invert) {
                            InvertCharSet ();
                        }

                        /* We have the set in CharSet. Read characters and
                         * store them into a string while they are part of
                         * the set.
                         */
			if (!NoAssign) {
			    S = va_arg (ap, char*);
                            while (IsCharInSet (C) && Width--) {
                                *S++ = C;
                                ReadChar ();
                            }
                            *S = '\0';
                        } else {
                            while (IsCharInSet (C) && Width--) {
                                ReadChar ();
                            }
                        }
                        ++Conversions;
		   	break;

     		    case 'p':
     	    		/* Pointer, format is 0xABCD */
		   	SkipWhite ();
			if (C != '0') {
                            longjmp (JumpBuf, RC_NOCONV);
                        }
			ReadChar ();
                        if (C != 'x' && C != 'X') {
                            longjmp (JumpBuf, RC_NOCONV);
                        }
                        ReadChar ();
                        if (ReadInt (16) != 4) { /* 4 chars expected */
                            longjmp (JumpBuf, RC_NOCONV);
                        }
			AssignInt ();
                        ++Conversions;
     			break;

     		    case 'n':
     			/* Store characters consumed so far */
			IntVal = D->ccount;
			AssignInt ();
			break;

		    default:
			/* Invalid conversion */
			longjmp (JumpBuf, RC_NOCONV);
			break;

		}
	    }
        }

        /* Push back the last unused character, provided it is not EOF */
        if (C != EOF) {
            D->unget (C, D->data);
        }

    } else {

	/* Jump via JumpBuf means an error. If this happens at EOF with no
         * conversions, it is considered an error, otherwise the number
         * of conversions is returned (the default behaviour).
         */
       	if (C == EOF && D->ccount == 0) {
	    /* Special case: error */
     	    Conversions = EOF;
     	}

    }

    /* Return the number of conversions */
    return Conversions;
}



