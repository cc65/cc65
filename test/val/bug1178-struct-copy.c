
/* bug #1178 - copying structs/unions > 4 bytes is broken */

#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    unsigned char a;
    unsigned char b;
    unsigned char c;
    unsigned char d;
    unsigned char e;
}
TestStruct1;

TestStruct1 StructArray1[2];
TestStruct1 test1;

typedef struct
{
    unsigned char a;
    unsigned char b;
    unsigned char c;
    unsigned char d;
}
TestStruct2;

TestStruct2 StructArray2[2];
TestStruct2 test2;

int res = EXIT_SUCCESS;

void dotest1(void)
{
    test1.a = 42;
    test1.b = 11;
    test1.c = 23;
    test1.d = 47;
    test1.e = 13;

    StructArray1[0] = test1;

    printf ("test1: %d, %d, %d, %d, %d\n",
            (int)StructArray1[0].a, (int)StructArray1[0].b, (int)StructArray1[0].c,
            (int)StructArray1[0].d, (int)StructArray1[0].e);
    if ((StructArray1[0].a != 42) ||
        (StructArray1[0].b != 11) ||
        (StructArray1[0].c != 23) ||
        (StructArray1[0].d != 47) ||
        (StructArray1[0].e != 13)) {
        res = EXIT_FAILURE;
    }
}

void dotest2(void)
{
    test2.a = 42;
    test2.b = 11;
    test2.c = 23;
    test2.d = 47;

    StructArray2[0] = test2;

    printf ("test2: %d, %d, %d, %d\n",
            (int)StructArray2[0].a, (int)StructArray2[0].b,
            (int)StructArray2[0].c, (int)StructArray2[0].d);
    if ((StructArray2[0].a != 42) ||
        (StructArray2[0].b != 11) ||
        (StructArray2[0].c != 23) ||
        (StructArray2[0].d != 47)) {
        res = EXIT_FAILURE;
    }
}

int main(void)
{
    dotest1();
    dotest2();
    return res;
}
