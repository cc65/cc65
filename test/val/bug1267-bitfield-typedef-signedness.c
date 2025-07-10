/*
  Copyright 2020 The cc65 Authors

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
  Tests of bit-field signedness with typedefs; see https://github.com/cc65/cc65/issues/1267
*/

#include <stdio.h>

static unsigned char failures = 0;

typedef int i16;
typedef unsigned int u16;
typedef signed int s16;

static struct ints {
    i16 i : 4;
    u16 u : 4;
    s16 s : 4;
} si = {1, 2, 3};

static void test_bitfield_typedefs (void)
{
    if (si.i != 1) {
        /* Note that this is another bug that i is signed. */
        printf ("Got si.a = %d, expected 1.\n", si.i);
        failures++;
    }
    if (si.u != 2) {
        printf ("Got si.u = %u, expected 2.\n", si.u);
        failures++;
    }
    if (si.s != 3) {
        printf ("Got si.s = %d, expected 3.\n", si.s);
        failures++;
    }

    si.i = -1;
    si.u = -2;
    si.s = -3;

    /* Note that this is another bug that i is signed. */
    if (si.i != -1) {
        printf ("Got si.a = %d, expected -1.\n", si.i);
        failures++;
    }
    if (si.u != 14) {
        printf ("Got si.u = %u, expected 14.\n", si.u);
        failures++;
    }
    if (si.s != -3) {
        printf ("Got si.s = %d, expected -3.\n", si.s);
        failures++;
    }
}

int main (void)
{
    test_bitfield_typedefs ();
    printf ("failures: %u\n", failures);
    return failures;
}
