/*
  !!DESCRIPTION!!
  !!ORIGIN!!      testsuite
  !!LICENCE!!     Public Domain
  !!AUTHOR!!      Johan Kotlinski
*/

#include <stdio.h>

/*
This produces the compiler error "test.c(9): Error: Assignment to const"
Shouldn't be an error, should it? baz is const, bar isn't.
*/

typedef struct {
    char foo;
} Bar;

int main() {
    Bar bar;
    Bar* const baz = &bar;

    baz->foo = 1;

    printf("it works :)\n");

    return 0;
}

