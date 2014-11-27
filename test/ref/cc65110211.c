/*
  !!DESCRIPTION!! unreachable code related bug
  !!ORIGIN!!      Testsuite
  !!LICENCE!!     Public Domain
*/

/*
    test2 and test3 will result in an endless loop (SVN version: 4974M)
*/

#include <stdio.h>

#define OPENTEST()
#define CLOSETEST()

static char upper[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

int test1(void)
{
    int res;
    unsigned char *p;

    p = upper;
    res = 0;

    while(*p) {
        if(*p < 0) {
            res = 1;
        }
        p++;
    }

    printf("test1:ok\n");
    return res;
}

int test2(void)
{
    int res;
    unsigned char *p;

    p = upper;
    res = 0;

    while(*p) {
        if(*p++ < 0) {
            res = 1;
        }
    }

    printf("test2:ok\n");
    return res;
}

int test3(void)
{
    int res;
    unsigned char *p;

    p = upper;
    res = 0;

    while(*p) {
        if(*++p < 0) {
            res = 1;
        }
    }

    printf("test3:ok\n");
    return res;
}

int main(int n,char **args)
{
    test1();
    test2();
    test3();

    printf("it works :)\n");

    return 0;
}
