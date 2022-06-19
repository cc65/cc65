
// test comparison operations
// WIP WIP WIP

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include <_float.h>

float fp1, fp2, fp3, fp4;

char buf[0x10];
char buf2[0x10];
char buf3[0x10];

signed char var_schar;
unsigned char var_uchar;
signed int var_sint;
unsigned int var_uint;
signed long var_slong;
unsigned long var_ulong;

int main(void)
{
    printf("float-basic-cmp\n");

    //-------------------------------------------------------------------------
    // float variable vs float constant
    printf("var vs const\n");

//FIXME: compiles, but is wrong, the constant in the comparison becomes 0
    fp1 = 1.5f;
    printf("1.5f == 1.6f is ");
    if (fp1 == 1.6f) {
        printf("true\n");
    } else {
        printf("false\n");
    }
    fp1 = 1.5f;
    printf("1.5f == 1.5f is ");
    if (1.5f == fp1) {
        printf("true\n");
    } else {
        printf("false\n");
    }

//FIXME: compiles, but is wrong, the constant in the comparison becomes 0
    fp1 = 1.5f;
    printf("1.5f != 1.6f is ");
    if (fp1 != 1.6f) {
        printf("true\n");
    } else {
        printf("false\n");
    }
    fp1 = 1.5f;
    printf("1.5f != 1.5f is ");
    if (1.5f != fp1) {
        printf("true\n");
    } else {
        printf("false\n");
    }

//FIXME: compiles, but is wrong, the constant in the comparison becomes 0
    fp1 = 1.5f;
    printf("1.5f < 1.6f is ");
    if (fp1 < 1.6f) {
        printf("true\n");
    } else {
        printf("false\n");
    }
    fp1 = 1.7f;
    printf("1.7f < 1.5f is ");
    if (1.7f < fp1) {
        printf("true\n");
    } else {
        printf("false\n");
    }

    //-------------------------------------------------------------------------
    // float variable vs float variable
    printf("var vs var\n");
    fp1 = 1.5f;
    fp2 = 1.6f;
    printf("1.5f == 1.6f is ");
    if (fp1 == fp2) {
        printf("true\n");
    } else {
        printf("false\n");
    }
    printf("1.5f != 1.6f is ");
    if (fp1 != fp2) {
        printf("true\n");
    } else {
        printf("false\n");
    }
    printf("1.5f < 1.6f is ");
    if (fp1 < fp2) {
        printf("true\n");
    } else {
        printf("false\n");
    }
    printf("1.5f > 1.6f is ");
    if (fp1 > fp2) {
        printf("true\n");
    } else {
        printf("false\n");
    }
    printf("1.5f <= 1.6f is ");
    if (fp1 <= fp2) {
        printf("true\n");
    } else {
        printf("false\n");
    }
    printf("1.5f >= 1.6f is ");
    if (fp1 >= fp2) {
        printf("true\n");
    } else {
        printf("false\n");
    }

    fp1 = 1.6f;
    printf("1.6f == 1.6f is ");
    if (fp1 == fp2) {
        printf("true\n");
    } else {
        printf("false\n");
    }
    printf("1.6f != 1.6f is ");
    if (fp1 != fp2) {
        printf("true\n");
    } else {
        printf("false\n");
    }
    printf("1.6f < 1.6f is ");
    if (fp1 < fp2) {
        printf("true\n");
    } else {
        printf("false\n");
    }
    printf("1.6f > 1.6f is ");
    if (fp1 > fp2) {
        printf("true\n");
    } else {
        printf("false\n");
    }
    printf("1.6f <= 1.6f is ");
    if (fp1 <= fp2) {
        printf("true\n");
    } else {
        printf("false\n");
    }
    printf("1.6f >= 1.6f is ");
    if (fp1 >= fp2) {
        printf("true\n");
    } else {
        printf("false\n");
    }

    return 0;
}
