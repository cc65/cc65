
/*
  !!DESCRIPTION!! struct base address dereferencing bug
  !!ORIGIN!!      Testsuite
  !!LICENCE!!     Public Domain
*/

#include <stdlib.h>
#include <stdio.h>

struct yywork
{
        char verify, advance;
} yycrank[] =
{
        {0,0}
};

struct yysvf
{
        struct yywork *yystoff;
} yysvec[1];

unsigned char fails = 0;

int main(int n, char **args)
{
    struct yysvf *yystate = yysvec;
    struct yywork *yyt;

    yystate->yystoff = yycrank;
    yyt = yystate->yystoff;

    if(yyt == yycrank) {
        printf("yyt == yycrank (ok)\n");
    } else {
        printf("yyt != yycrank (fail)\n");
        ++fails;
    }

    if(yyt == yystate->yystoff) {
        printf("yyt == yystate->yystoff (ok)\n");
    } else {
        printf("yyt != yystate->yystoff (fail)\n");
        ++fails;
    }

    if(yycrank == yystate->yystoff) {
        printf("yycrank == yystate->yystoff (ok)\n");
    } else {
        printf("yycrank != yystate->yystoff (fail)\n");
        ++fails;
    }

    printf("fails: %d\n", fails);
    return fails;
}
