/*
  !!DESCRIPTION!! bit field bug
  !!ORIGIN!!      testsuite
  !!LICENCE!!     Public Domain
  !!AUTHOR!!      Johan Kotlinski
*/

#include <stdio.h>
#include <assert.h>

struct {
    int foo : 7;
    int bar : 4;
} baz = { 0, 2 };

int main() {
    char bar = baz.bar;

    assert(2 == bar);

    printf("it works :)\n");

    /* if not assert() */
    return 0;
}

/* Assert fails. (SVN rev 4381) */