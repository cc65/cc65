/*
  !!DESCRIPTION!! if/then, ? operator, compares
  !!ORIGIN!!      cc65 devel list
  !!LICENCE!!     Public Domain
*/

#include <stdio.h>
#include <limits.h>

void t1a(void)
{
    int a = -0x5000;

    printf(a < 0x5000 ? "ok\n" : "error\n");
}

void t1b(void)
{
    int a = -0x5000;

    if(a<0x5000)
    {
        printf("ok\n");
    }
    else
    {
        printf("error\n");
    }
}

int main(void)
{
    t1a();t1b();

    return 0;
}
