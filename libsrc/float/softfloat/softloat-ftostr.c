
#include <stdlib.h>
#include <stdio.h>

//#define DEBUG_VALUE

// This is really a terrible function that should get completely rewritten and
// tested by someone who knows what he is doing =P
char *_ftostr(char *buffer, float f) {
    signed long intpart;
    float fracpart;
    float f1, f2;
    signed long n0;
    int is_negative;
    unsigned long raw_value = *((unsigned long*)(&f));

    is_negative = (raw_value & 0x80000000) ? 1 : 0;     // very hacky indeed

    if (!is_negative) {
        intpart = (signed long)(f);
    } else {
        intpart = (signed long)(f);
        intpart = -intpart;
    }

    f2 = intpart;
#if 0
    if (!is_negative) {
        if (f2 > f) {
            if (intpart > 0) {
                --intpart;
            }
        }
    } else {
        if (f2 > f) {
            if (intpart > 0) {
                --intpart;
            }
        }
//                ++intpart;
    }

    if (!is_negative) {
        if (intpart < 0) {
            intpart *= -1;
        }
    } else {
        if (intpart < 0) {
            intpart *= -1;
        }
    }
#endif
    f2 = intpart;
    if (!is_negative) {
        fracpart = f - f2;
    } else {
        fracpart = (-f) - f2;
    }

    f1 = 10000.0f;

    f1 = f1 * fracpart;

    n0 = (signed long)f1;
#if 0
    // caution: make sure fracpart can never be negative
    if (n0 < 0) {
        n0 *= -1;
    }
#endif
//    printf("n0:%ld\n",n0);
#ifdef DEBUG_VALUE
    if (n0 >= 1000) {
        sprintf(buffer, "<0x%08lx:%s%ld.%ld>", raw_value, is_negative ? "-" : "", intpart, n0);
    } else if (n0 >= 100) {
        sprintf(buffer, "<0x%08lx:%s%ld.0%ld>", raw_value, is_negative ? "-" : "", intpart, n0);
    } else if (n0 >= 10) {
        sprintf(buffer, "<0x%08lx:%s%ld.00%ld>", raw_value, is_negative ? "-" : "", intpart, n0);
    } else if (n0 >= 1) {
        sprintf(buffer, "<0x%08lx:%s%ld.000%ld>", raw_value, is_negative ? "-" : "", intpart, n0);
    } else {
        sprintf(buffer, "<0x%08lx:%s%ld.0000>", raw_value, is_negative ? "-" : "", intpart);
    }
#else
    if (n0 >= 1000) {
        sprintf(buffer, "%s%ld.%ld", is_negative ? "-" : "", intpart, n0);
    } else if (n0 >= 100) {
        sprintf(buffer, "%s%ld.0%ld", is_negative ? "-" : "", intpart, n0);
    } else if (n0 >= 10) {
        sprintf(buffer, "%s%ld.00%ld", is_negative ? "-" : "", intpart, n0);
    } else if (n0 >= 1) {
        sprintf(buffer, "%s%ld.000%ld", is_negative ? "-" : "", intpart, n0);
    } else {
        sprintf(buffer, "%s%ld.0000", is_negative ? "-" : "", intpart);
    }
#endif
    return &buffer[0];
}
