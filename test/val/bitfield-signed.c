/*
  Copyright 2020-2022 The cc65 Authors

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
  2. Altered source versions must be signedly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

/*
  Tests that signed bit-fields are indeed signed; see issues below
  - unsigned integer types by default: https://github.com/cc65/cc65/issues/1095
  - char bit-field support: https://github.com/cc65/cc65/issues/1047
  - long bit-field support: https://github.com/cc65/cc65/issues/1131
*/

#include <stdio.h>

static unsigned char failures = 0;

static struct signed_ints {
    signed int x : 4;
    signed int y : 10;
} pi = {-8, -500};

static void test_signed_int_bitfields (void)
{
    if (pi.x != -8) {
        printf ("Got pi.x = %ld, expected -8.\n", (long)pi.x);
        failures++;
    }
    if (pi.y != -500) {
        printf ("Got pi.y = %ld, expected -500.\n", (long)pi.y);
        failures++;
    }

    pi.x = -3;
    pi.y = -512;

    if (pi.x != -3) {
        printf ("Got pi.x = %ld, expected -3.\n", (long)pi.x);
        failures++;
    }
    if (pi.y != -512) {
        printf ("Got pi.y = %ld, expected -512.\n", (long)pi.y);
        failures++;
    }
}

static struct signed_shorts {
    signed short x : 4;
    signed short y : 10;
} ps = {-8, -500};

static void test_signed_short_bitfields (void)
{
    if (ps.x != -8) {
        printf ("Got ps.x = %ld, expected -8.\n", (long)ps.x);
        failures++;
    }
    if (ps.y != -500) {
        printf ("Got ps.y = %ld, expected -500.\n", (long)ps.y);
        failures++;
    }

    ps.x = -3;
    ps.y = -512;

    if (ps.x != -3) {
        printf ("Got ps.x = %ld, expected -3.\n", (long)ps.x);
        failures++;
    }
    if (ps.y != -512) {
        printf ("Got ps.y = %ld, expected -512.\n", (long)ps.y);
        failures++;
    }
}

static struct signed_chars {
    signed char x : 4;
} pc = {-8};

static void test_signed_char_bitfields (void)
{
    if (pc.x != -8) {
        printf ("Got pc.x = %ld, expected -8.\n", (long)pc.x);
        failures++;
    }

    pc.x = -3;

    if (pc.x != -3) {
        printf ("Got pc.x = %ld, expected -3.\n", (long)pc.x);
        failures++;
    }
}

static struct signed_longs {
    signed long x : 4;
    signed long y : 10;
    signed long z : 18;
} pl = {-8, -500, -70000};

static void test_signed_long_bitfields (void)
{
    if (pl.x != -8) {
        if (pl.x < 0) {
            printf ("Got pl.x = %ld, expected -8.\n", (long)pl.x);
        } else {
            printf ("Got pl.x = %lu, expected -8.\n", (unsigned long)pl.x);
        }
        failures++;
    }
    if (pl.y != -500) {
        if (pl.y < 0) {
            printf ("Got pl.y = %ld, expected -500.\n", (long)pl.y);
        } else {
            printf ("Got pl.y = %lu, expected -500.\n", (unsigned long)pl.y);
        }
        failures++;
    }
    if (pl.z != -70000) {
        if (pl.z < 0) {
            printf ("Got pl.z = %ld, expected -70000.\n", (long)pl.z);
        } else {
            printf ("Got pl.z = %lu, expected -70000.\n", (unsigned long)pl.z);
        }
        failures++;
    }

    pl.x = -3;
    pl.y = -512;
    pl.z = -131072;

    if (pl.x != -3) {
        if (pl.x < 0) {
            printf ("Got pl.x = %ld, expected -3.\n", (long)pl.x);
        } else {
            printf ("Got pl.x = %lu, expected -3.\n", (unsigned long)pl.x);
        }
        failures++;
    }
    if (pl.y != -512) {
        if (pl.y < 0) {
            printf ("Got pl.y = %ld, expected -512.\n", (long)pl.y);
        } else {
            printf ("Got pl.y = %lu, expected -512.\n", (unsigned long)pl.y);
        }
        failures++;
    }
    if (pl.z != -131072) {
        if (pl.z < 0) {
            printf ("Got pl.z = %ld, expected -131072.\n", (long)pl.z);
        } else {
            printf ("Got pl.z = %lu, expected -131072.\n", (unsigned long)pl.z);
        }
        failures++;
    }
}

int main (void)
{
    test_signed_int_bitfields ();
    test_signed_short_bitfields ();
    test_signed_char_bitfields ();
    test_signed_long_bitfields ();
    printf ("failures: %u\n", failures);
    return failures;
}
