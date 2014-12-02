/*
  !!DESCRIPTION!!
  !!ORIGIN!!      testsuite
  !!LICENCE!!     Public Domain
  !!AUTHOR!!      Johan Kotlinski
*/

#include <stdio.h>

/*
...gives "test.c(2): Error: Variable `foo' has unknown size" using -Cl.
Is it really unknown?

cc65 V2.13.0, SVN version: 4384
*/

int main() {
    char foo[] = { 0 };
    printf("it works :)\n");

    return 0;
}
