#include <limits.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>



long __fastcall__ strtol (const char* nptr, char** endptr, int base)
/* Convert a string to a long int */
{
    register const char* S          = nptr;
    unsigned long        Val        = 0;
    unsigned char        Minus      = 0;
    unsigned char        Ovf        = 0;
    unsigned             CvtCount   = 0;
    unsigned char        DigitVal;
    unsigned long        MaxVal;
    unsigned char        MaxDigit;


    /* Skip white space */
    while (isspace (*S)) {
        ++S;
    }

    /* Check for leading + or - sign */
    switch (*S) {
        case '-':
            Minus = 1;
            /* FALLTHROUGH */
        case '+':
            ++S;
    }

    /* If base is zero, we may have a 0 or 0x prefix. If base is 16, we may
    ** have a 0x prefix.
    */
    if (base == 0) {
        if (*S == '0') {
            ++S;
            if (*S == 'x' || *S == 'X') {
                ++S;
                base = 16;
            } else {
                base = 8;
            }
        } else {
            base = 10;
        }
    } else if (base == 16 && *S == '0' && (S[1] == 'x' || S[1] == 'X')) {
        S += 2;
    }

    /* Determine the maximum valid number and (if the number is equal to this
    ** value) the maximum valid digit.
    */
    if (Minus) {
        MaxVal = LONG_MIN;
    } else {
        MaxVal = LONG_MAX;
    }
    MaxDigit = MaxVal % base;
    MaxVal  /= base;

    /* Convert the number */
    while (1) {

        /* Convert the digit into a numeric value */
        if (isdigit (*S)) {
            DigitVal = *S - '0';
        } else if (isupper (*S)) {
            DigitVal = *S - ('A' - 10);
        } else if (islower (*S)) {
            DigitVal = *S - ('a' - 10);
        } else {
            /* Unknown character */
            break;
        }

        /* Don't accept a character that doesn't match base */
        if (DigitVal >= base) {
            break;
        }

        /* Don't accept anything that makes the final value invalid */
        if (Val > MaxVal || (Val == MaxVal && DigitVal > MaxDigit)) {
            Ovf = 1;
        }

        /* Calculate the next value if digit is not invalid */
        if (Ovf == 0) {
            Val = (Val * base) + DigitVal;
            ++CvtCount;
        }

        /* Next character from input */
        ++S;
    }

    /* Store the end pointer. If no conversion was performed, the value of
    ** nptr is returned in endptr.
    */
    if (endptr) {
        if (CvtCount > 0) {
            *endptr = (char*) S;
        } else {
            *endptr = (char*) nptr;
        }
    }

    /* Handle overflow */
    if (Ovf) {
        _seterrno (ERANGE);
        if (Minus) {
            return LONG_MIN;
        } else {
            return LONG_MAX;
        }
    }

    /* Return the result */
    if (Minus) {
        return -(long)Val;
    } else {
        return Val;
    }
}

