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
  Tests of char bit-fields; see https://github.com/cc65/cc65/issues/1047
*/

#include <stdio.h>

static unsigned char failures = 0;

static struct four_bits {
    unsigned char x : 4;
} fb = {1};

static void test_four_bits (void)
{
    if (sizeof (struct four_bits) != 1) {
        printf ("Got sizeof (struct four_bits) = %zu, expected 1.\n",
                sizeof (struct four_bits));
        failures++;
    }

    if (fb.x != 1) {
        printf ("Got fb.x = %u, expected 1.\n", fb.x);
        failures++;
    }

    fb.x = 3;

    if (fb.x != 3) {
        printf ("Got fb.x = %u, expected 3.\n", fb.x);
        failures++;
    }
}

static struct four_bits_signed {
    signed char x : 4;
} fbs = {1};

static void test_four_bits_signed (void)
{
    if (sizeof (struct four_bits_signed) != 1) {
        printf ("Got sizeof (struct four_bits_signed) = %zu, expected 1.\n",
                sizeof (struct four_bits));
        failures++;
    }

    if (fbs.x != 1) {
        printf ("Got fbs.x = %d, expected 1.\n", fbs.x);
        failures++;
    }

    fbs.x = 3;

    if (fbs.x != 3) {
        printf ("Got fbs.x = %d, expected 3.\n", fbs.x);
        failures++;
    }
}

static struct four_bits_plain {
    char x : 4;
} fbp = {1};

static void test_four_bits_plain (void)
{
    if (sizeof (struct four_bits_plain) != 1) {
        printf ("Got sizeof (struct four_bits_plain) = %zu, expected 1.\n",
                sizeof (struct four_bits));
        failures++;
    }

    if (fbp.x != 1) {
        printf ("Got fbp.x = %d, expected 1.\n", fbp.x);
        failures++;
    }

    fbp.x = 3;

    if (fbp.x != 3) {
        printf ("Got fbp.x = %d, expected 3.\n", fbp.x);
        failures++;
    }
}

/*
  Logic is somewhat diferent for bit-fields that end a struct vs
  having additional fields.
*/

static struct four_bits_with_char {
    unsigned char x : 4;
    unsigned char y;
} fbi = {1, 2};

static void test_four_bits_with_char (void)
{
    if (sizeof (struct four_bits_with_char) != 2) {
        printf ("Got sizeof (struct four_bits_with_char) = %zu, expected 2.\n",
                sizeof (struct four_bits_with_char));
        failures++;
    }

    if (fbi.x != 1) {
        printf ("Got fbi.x = %u, expected 1.\n", fbi.x);
        failures++;
    }

    if (fbi.y != 2) {
        printf ("Got fbi.y = %u, expected 2.\n", fbi.y);
        failures++;
    }

    fbi.x = 3;
    fbi.y = 17;

    if (fbi.x != 3) {
        printf ("Got fbi.x = %u, expected 3.\n", fbi.x);
        failures++;
    }

    if (fbi.y != 17) {
        printf ("Got fbi.y = %u, expected 17.\n", fbi.y);
        failures++;
    }
}

static struct two_chars {
    unsigned char x : 4;
    unsigned char y : 4;
} o = {11, 7};

/* Tests that bit-fields can share allocation units. */
static void test_two_chars (void)
{
    if (sizeof (struct two_chars) != 1) {
        printf ("Got sizeof (struct two_chars) = %zu, expected 1.\n",
                sizeof (struct two_chars));
        failures++;
    }

    if (o.x != 11) {
        printf ("Got o.x = %u, expected 11.\n", o.x);
        failures++;
    }

    if (o.y != 7) {
        printf ("Got o.y = %u, expected 7.\n", o.y);
        failures++;
    }

    o.x = 3;
    o.y = 4;

    if (o.x != 3) {
        printf ("Got o.x = %u, expected 3.\n", o.x);
        failures++;
    }

    if (o.y != 4) {
        printf ("Got o.y = %u, expected 4.\n", o.y);
        failures++;
    }
}

static struct full_width {
    unsigned char x : 8;
} fw = {255};

static void test_full_width (void)
{
    if (sizeof (struct full_width) != 1) {
        printf ("Got sizeof (struct full_width) = %zu, expected 1.\n",
                sizeof (struct full_width));
        failures++;
    }

    if (fw.x != 255) {
        printf ("Got fw.x = %u, expected 255.\n", fw.x);
        failures++;
    }

    fw.x = 42;

    if (fw.x != 42) {
        printf ("Got fw.x = %u, expected 42.\n", fw.x);
        failures++;
    }
}

static struct aligned_end {
    unsigned char : 2;
    unsigned char x : 6;
    unsigned char : 3;
    unsigned char y : 5;
} ae = {63, 17};

static void test_aligned_end (void)
{
    if (sizeof (struct aligned_end) != 2) {
        printf ("Got sizeof (struct aligned_end) = %zu, expected 2.\n",
                sizeof (struct aligned_end));
        failures++;
    }

    if (ae.x != 63) {
        printf ("Got ae.x = %u, expected 63.\n", ae.x);
        failures++;
    }

    if (ae.y != 17) {
        printf ("Got ae.y = %u, expected 17.\n", ae.y);
        failures++;
    }

    ae.x = 42;
    ae.y = 15;

    if (ae.x != 42) {
        printf ("Got ae.x = %u, expected 42.\n", ae.x);
        failures++;
    }

    if (ae.y != 15) {
        printf ("Got ae.y = %u, expected 15.\n", ae.y);
        failures++;
    }
}

struct { signed char x : 1; } sc = {-1};
struct { unsigned char x : 1; } uc = {1};
struct { char x : 1; } pc = {1};

static void test_signedness (void)
{
    if (sc.x != -1) {
        printf ("Got sc.x = %d, expected -1.\n", sc.x);
        failures++;
    }

    if (uc.x != 1) {
        printf ("Got uc.x = %u, expected 1.\n", uc.x);
        failures++;
    }

    if (pc.x != 1) {
        printf ("Got pc.x = %u, expected 1.\n", pc.x);
        failures++;
    }
}

int main (void)
{
    test_four_bits ();
    test_four_bits_with_char ();
    test_two_chars ();
    test_full_width ();
    test_aligned_end ();
    test_signedness ();
    printf ("failures: %u\n", failures);
    return failures;
}
