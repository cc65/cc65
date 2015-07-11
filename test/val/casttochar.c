
/*
  !!DESCRIPTION!! Cast to char
  !!ORIGIN!!      Piotr Fusik
  !!LICENCE!!     PD
*/

#include <stdio.h>
#include <stdlib.h>

static unsigned int failures = 0;

int f1(int i, int j) {
    return (signed char) (i + 1) == j;
}

int f2(int i, int j) {
    return (unsigned char) (i + 1) == j;
}

int f3(int i, int j) {
    return (char) (i + 1) == j;
}

int main(void)
{
    printf("f1: got :%04x ", f1(0x1234, 0x35));
    if(f1(0x1234, 0x35) == 0) {
        printf("- failed");
        failures++;
    }
    printf("\n");

    printf("f2: got :%04x ", f2(0x1234, 0x35));
    if(f2(0x1234, 0x35) == 0) {
        printf("- failed");
        failures++;
    }
    printf("\n");

    printf("f3: got :%04x ", f3(0x1234, 0x35));
    if(f3(0x1234, 0x35) == 0) {
        printf("- failed");
        failures++;
    }
    printf("\n");

    return failures;
}
