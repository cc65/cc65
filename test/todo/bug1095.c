/*
  Copyright 2020 Google LLC

  This software is provided 'as-is', without any express or implied
  warranty. In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

/*
  Tests of signed bit-fields; see https://github.com/cc65/cc65/issues/1095
*/

#include <stdio.h>

static unsigned char failures = 0;

static struct signed_ints {
    signed int a : 3;
    signed int b : 3;
    signed int c : 3;
} si = {-4, -1, 3};

static void test_signed_bitfield(void)
{
    if (si.a >= 0) {
        printf("Got si.a = %d, expected negative.\n", si.a);
        failures++;
    }
    if (si.a != -4) {
        printf("Got si.a = %d, expected -4.\n", si.a);
        failures++;
    }

    if (si.b >= 0) {
        printf("Got si.b = %d, expected negative.\n", si.b);
        failures++;
    }
    if (si.b != -1) {
        printf("Got si.b = %d, expected -1.\n", si.b);
        failures++;
    }

    if (si.b <= 0) {
        printf("Got si.c = %d, expected positive.\n", si.c);
        failures++;
    }
    if (si.c != 3) {
        printf("Got si.c = %d, expected 3.\n", si.c);
        failures++;
    }

    si.a = -3;
    si.b = 1;
    si.c = -2;

    if (si.a >= 0) {
        printf("Got si.a = %d, expected negative.\n", si.a);
        failures++;
    }
    if (si.a != -3) {
        printf("Got si.a = %d, expected -3.\n", si.a);
        failures++;
    }

    if (si.b <= 0) {
        printf("Got si.b = %d, expected positive.\n", si.b);
        failures++;
    }
    if (si.b != 1) {
        printf("Got si.b = %d, expected 1.\n", si.b);
        failures++;
    }

    if (si.b >= 0) {
        printf("Got si.c = %d, expected negative.\n", si.c);
        failures++;
    }
    if (si.c != -2) {
        printf("Got si.c = %d, expected -2.\n", si.c);
        failures++;
    }
}

int main(void)
{
    test_signed_bitfield();
    printf("failures: %u\n", failures);
    return failures;
}
