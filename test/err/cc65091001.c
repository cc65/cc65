/*
  !!DESCRIPTION!! invalid binary operation on pointer, should not compile
  !!ORIGIN!!      testsuite
  !!LICENCE!!     Public Domain
  !!AUTHOR!!
*/

/* > Gets stuck in an endless loop with -O. */

#include <assert.h>
#include <string.h>
#include <stdio.h>
typedef unsigned char U8;
char var = 0xf0;
char fn(char bar)
{
    char* ptr = (char*)0xf;
    var |= ptr; /* should throw an error here */
    while (var > bar)
        var <<= 1;
    return 0;
}
int main() {
    fn(0x7f);
    assert(0);

    printf("it works :)\n");

    return 0;
}