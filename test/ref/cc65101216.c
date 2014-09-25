/*
  !!DESCRIPTION!! division bug
  !!ORIGIN!!      testsuite
  !!LICENCE!!     Public Domain
  !!AUTHOR!!      Stefan Wessels
*/

/*
The output from the code below is:
a / b = 6

Shouldn't that be 0?
*/

#include <stdio.h>
#define b 10000
char a;
int main()
{
    char c;

    a = 100;
    c = a / b;
    printf("a / b = %d", c);

    return 0;
}