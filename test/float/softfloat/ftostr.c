
#include <stdlib.h>
#include <stdio.h>

char *_ftostr(char *buffer, float f) {
    signed long intpart = (signed long)(f);
    float fracpart;
    float f1, f2;
    signed long n0;
//    printf("f:%f\n",f);
    f2 = intpart;
//    printf("f2:%f\n",f2);
//    printf("f2:%08lx\n",*((unsigned long*)(&f2)));
    if (f2 > f) {
        if (intpart > 0) {
            --intpart;
        }
    }
    f2 = intpart;
    fracpart = f - f2;
//    printf("fracpart:%f\n",fracpart);
//    printf("fracpart:%08lx\n",*((unsigned long*)(&fracpart)));
//    f1 = fracpart * 10000.0f;
    f1 = 10000.0f;
//    printf("fracpart:%08lx\n",*((unsigned long*)(&f1)));
//    f1 *= fracpart;
    f1 = f1 * fracpart;
//    f1 = fracpart;
//    f1 = intpart;
//    printf("fracpart:%f\n",f1);
//    printf("fracpart:%08lx\n",*((unsigned long*)(&f1)));
    n0 = (signed long)f1;
    if (n0 < 0) {
        n0 *= -1;
    }
//    printf("n0:%ld\n",n0);
    if (n0 >= 1000) {
        sprintf(buffer, "<0x%08lx:%ld.%ld>", *((unsigned long*)(&f)), intpart, n0);
    } else if (n0 >= 100) {
        sprintf(buffer, "<0x%08lx:%ld.0%ld>", *((unsigned long*)(&f)), intpart, n0);
    } else if (n0 >= 10) {
        sprintf(buffer, "<0x%08lx:%ld.00%ld>", *((unsigned long*)(&f)), intpart, n0);
    } else if (n0 >= 1) {
        sprintf(buffer, "<0x%08lx:%ld.000%ld>", *((unsigned long*)(&f)), intpart, n0);
    } else {
        sprintf(buffer, "<0x%08lx:%ld.0000%ld>", *((unsigned long*)(&f)), intpart, n0);
    }
    return &buffer[0];
}
