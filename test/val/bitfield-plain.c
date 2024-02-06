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
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

/*
  Tests that plain bit-fields are unsigned; see issues below
  - unsigned integer types by default: https://github.com/cc65/cc65/issues/1095
  - char bit-field support: https://github.com/cc65/cc65/issues/1047
  - long bit-field support: https://github.com/cc65/cc65/issues/1131
*/

#include <stdio.h>

static unsigned char failures = 0;

static struct plain_ints {
    int x : 4;
    int y : 10;
} pi = {15, 700};

static void test_plain_int_bitfields (void)
{
    if (pi.x != 15) {
        printf ("Got pi.x = %ld, expected 15.\n", (long)pi.x);
        failures++;
    }
    if (pi.y != 700) {
        printf ("Got pi.y = %ld, expected 700.\n", (long)pi.y);
        failures++;
    }

    pi.x = 3;
    pi.y = 1023;

    if (pi.x != 3) {
        printf ("Got pi.x = %ld, expected 3.\n", (long)pi.x);
        failures++;
    }
    if (pi.y != 1023) {
        printf ("Got pi.y = %ld, expected 1023.\n", (long)pi.y);
        failures++;
    }
}

static struct plain_shorts {
    short x : 4;
    short y : 10;
} ps = {15, 700};

static void test_plain_short_bitfields (void)
{
    if (ps.x != 15) {
        printf ("Got ps.x = %ld, expected 15.\n", (long)ps.x);
        failures++;
    }
    if (ps.y != 700) {
        printf ("Got ps.y = %ld, expected 700.\n", (long)ps.y);
        failures++;
    }

    ps.x = 3;
    ps.y = 1023;

    if (ps.x != 3) {
        printf ("Got ps.x = %ld, expected 3.\n", (long)ps.x);
        failures++;
    }
    if (ps.y != 1023) {
        printf ("Got ps.y = %ld, expected 1023.\n", (long)ps.y);
        failures++;
    }
}

static struct plain_chars {
    char x : 4;
} pc = {15};

static void test_plain_char_bitfields (void)
{
    if (pc.x != 15) {
        printf ("Got pc.x = %ld, expected 15.\n", (long)pc.x);
        failures++;
    }

    pc.x = 3;

    if (pc.x != 3) {
        printf ("Got pc.x = %ld, expected 3.\n", (long)pc.x);
        failures++;
    }
}

static struct plain_longs {
    long x : 4;
    long y : 10;
    long z : 18;
} pl = {15, 700, 200000};

static void test_plain_long_bitfields (void)
{
    if (pl.x != 15) {
        if (pl.x < 0) {
            printf ("Got pl.x = %ld, expected 15.\n", (long)pl.x);
        } else {
            printf ("Got pl.x = %lu, expected 15.\n", (unsigned long)pl.x);
        }
        failures++;
    }
    if (pl.y != 700) {
        if (pl.y < 0) {
            printf ("Got pl.y = %ld, expected 700.\n", (long)pl.y);
        } else {
            printf ("Got pl.y = %lu, expected 700.\n", (unsigned long)pl.y);
        }
        failures++;
    }
    if (pl.z != 200000) {
        if (pl.z < 0) {
            printf ("Got pl.z = %ld, expected 200000.\n", (long)pl.z);
        } else {
            printf ("Got pl.z = %lu, expected 200000.\n", (unsigned long)pl.z);
        }
        failures++;
    }

    pl.x = 3;
    pl.y = 1023;
    pl.z = 262143;

    if (pl.x != 3) {
        if (pl.x < 0) {
            printf ("Got pl.x = %ld, expected 3.\n", (long)pl.x);
        } else {
            printf ("Got pl.x = %lu, expected 3.\n", (unsigned long)pl.x);
        }
        failures++;
    }
    if (pl.y != 1023) {
        if (pl.y < 0) {
            printf ("Got pl.y = %ld, expected 1023.\n", (long)pl.y);
        } else {
            printf ("Got pl.y = %lu, expected 1023.\n", (unsigned long)pl.y);
        }
        failures++;
    }
    if (pl.z != 262143) {
        if (pl.z < 0) {
            printf ("Got pl.z = %ld, expected 262143.\n", (long)pl.z);
        } else {
            printf ("Got pl.z = %lu, expected 262143.\n", (unsigned long)pl.z);
        }
        failures++;
    }
}

int main (void)
{
    test_plain_int_bitfields ();
    test_plain_short_bitfields ();
    test_plain_char_bitfields ();
    test_plain_long_bitfields ();
    printf ("failures: %u\n", failures);
    return failures;
}
