
#include <stdlib.h>
#include <stdio.h>

char *_ftostr(char *buffer, float f) {
    signed long intpart;
    float fracpart;
    float f1, f2;
    signed long n0;
    int is_negative;
    unsigned long raw_value = *((unsigned long*)(&f));

    is_negative = (raw_value & 0x80000000) ? 1 : 0;     // very hacky indeed

    intpart = (signed long)(f);

    f2 = intpart;

    if (f2 > f) {
        if (intpart > 0) {
            --intpart;
        }
    }

    if (intpart < 0) {
        intpart *= -1;
    }

    f2 = intpart;
    fracpart = f - f2;

    f1 = 10000.0f;

    f1 = f1 * fracpart;

    n0 = (signed long)f1;
    if (n0 < 0) {
        n0 *= -1;
    }
//    printf("n0:%ld\n",n0);
    if (n0 >= 1000) {
        sprintf(buffer, "<0x%08lx:%s%ld.%ld>", raw_value, is_negative ? "-" : "", intpart, n0);
    } else if (n0 >= 100) {
        sprintf(buffer, "<0x%08lx:%s%ld.0%ld>", *((unsigned long*)(&f)), is_negative ? "-" : "", intpart, n0);
    } else if (n0 >= 10) {
        sprintf(buffer, "<0x%08lx:%s%ld.00%ld>", *((unsigned long*)(&f)), is_negative ? "-" : "", intpart, n0);
    } else if (n0 >= 1) {
        sprintf(buffer, "<0x%08lx:%s%ld.000%ld>", *((unsigned long*)(&f)), is_negative ? "-" : "", intpart, n0);
    } else {
        sprintf(buffer, "<0x%08lx:%s%ld.0000%ld>", *((unsigned long*)(&f)), is_negative ? "-" : "", intpart, n0);
    }
    return &buffer[0];
}
