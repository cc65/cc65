/*
  !!DESCRIPTION!! Assign an int; then, do an operation that depends directly on that assignment.
  !!ORIGIN!!      cc65 regression tests
  !!LICENCE!!     Public Domain
  !!AUTHOR!!      Greg King
*/

#include <stdio.h>

static unsigned char failures = 0;

static unsigned int result;
static const unsigned int buffer = 0xABCD;

int main(void)
{
    result = buffer;

    /* Shift doesn't use high byte (X register); previous assignment should be optimized. */
    result <<= 8;
    if (result != 0xCD00) {
        ++failures;
        printf("assign-use1: left shift is $%X, not $CD00.\n", result);
    }

    result = buffer;

    /* Shift does use high byte; previous assignment shouldn't be optimized by OptStore5(). */
    result >>= 8;
    if (result != 0x00AB) {
        ++failures;
        printf("assign-use1: right shift is $%X, not $00AB.\n", result);
    }

    return failures;
}
