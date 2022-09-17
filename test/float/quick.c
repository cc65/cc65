
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
/*
#include <_float.h>
#include <math.h>
*/
unsigned char buf[32];

unsigned char var_uchar;
// float fp2;

float fp1 = 42.0f;
float fp2 = 23;
float fp3 = 33.0;

/*
    TODO:
float-basic.c:
    fp3 = fp1 / 2.3f;   // FIXME: division by zero
    fp3 = fp1 - 11.5f;  // FIXME: Invalid operands for binary operator '-'
float-conv.c:
    conversion from float variable to char variable does not work right
float-cmp.c:
    many failing cases
float-math.c:

DONE:
    float-minimal.c
    float-misc.c:
*/

char *test(char *buffer, float f) {
    signed long intpart = (signed long)(f);
    float fracpart;
    float f1, f2;
    signed long n0;
//    printf("f:%f\n",f);
    f2 = intpart;
//    printf("f2:%f\n",f2);
    printf("f2:%08lx\n",*((unsigned long*)(&f2)));
    fracpart = f - f2;
//    printf("fracpart:%f\n",fracpart);
    printf("fracpart:%08lx\n",*((unsigned long*)(&fracpart)));
//    f1 = fracpart * 10000.0f;
    f1 = 10000.0f;
    printf("fracpart:%08lx\n",*((unsigned long*)(&f1)));
//    f1 *= fracpart;
    f1 = f1 * fracpart;
//    f1 = fracpart;
//    f1 = intpart;
//    printf("fracpart:%f\n",f1);
    printf("fracpart:%08lx\n",*((unsigned long*)(&f1)));
    n0 = (signed long)f1;
    printf("n0:%ld\n",n0);
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

int main(void)
{
    fp2 = 0.001f;    printf("fp2 0x%08lx %s\n", *((uint32_t*)&fp2), test(buf, fp2));
    fp2 = 0.001234f; printf("fp2 0x%08lx %s\n", *((uint32_t*)&fp2), test(buf, fp2));
    fp2 = 0.01f;     printf("fp2 0x%08lx %s\n", *((uint32_t*)&fp2), test(buf, fp2));
    fp2 = 0.01234f;  printf("fp2 0x%08lx %s\n", *((uint32_t*)&fp2), test(buf, fp2));
    fp2 = 0.1234f;   printf("fp2 0x%08lx %s\n", *((uint32_t*)&fp2), test(buf, fp2));
    fp2 = 1.234f;    printf("fp2 0x%08lx %s\n", *((uint32_t*)&fp2), test(buf, fp2));
    fp2 = 12.34f;    printf("fp2 0x%08lx %s\n", *((uint32_t*)&fp2), test(buf, fp2));
    fp2 = 123.4f;    printf("fp2 0x%08lx %s\n", *((uint32_t*)&fp2), test(buf, fp2));

    return 0;
}
