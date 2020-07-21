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
  Tests of bit-field packing; see https://github.com/cc65/cc65/issues/1054
*/

#include <stdio.h>

static unsigned char failures = 0;

static struct four_bits {
    unsigned int x : 4;
} fb = {1};

static void test_four_bits(void)
{
    if (sizeof(struct four_bits) != 1) {
        printf("Got sizeof(struct four_bits) = %zu, expected 1.\n",
               sizeof(struct four_bits));
        failures++;
    }

    if (fb.x != 1) {
        printf("Got fb.x = %u, expected 1.\n", fb.x);
        failures++;
    }

    fb.x = 3;

    if (fb.x != 3) {
        printf("Got fb.x = %u, expected 3.\n", fb.x);
        failures++;
    }
}

/*
  Logic is somewhat diferent for bit-fields that end a struct vs
  having additional fields.
*/

static struct four_bits_with_int {
    unsigned int x : 4;
    unsigned int y;
} fbi = {1, 2};

static void test_four_bits_with_int(void)
{
    /* The first 4-bit bit-field just takes one byte, so the size is 3.  */
    if (sizeof(struct four_bits_with_int) != 3) {
        printf("Got sizeof(struct four_bits_with_int) = %zu, expected 3.\n",
               sizeof(struct four_bits_with_int));
        failures++;
    }

    if (fbi.x != 1) {
        printf("Got fbi.x = %u, expected 1.\n", fbi.x);
        failures++;
    }

    if (fbi.y != 2) {
        printf("Got fbi.y = %u, expected 2.\n", fbi.y);
        failures++;
    }

    fbi.x = 3;
    fbi.y = 17;

    if (fbi.x != 3) {
        printf("Got fbi.x = %u, expected 3.\n", fbi.x);
        failures++;
    }

    if (fbi.y != 17) {
        printf("Got fbi.y = %u, expected 17.\n", fbi.y);
        failures++;
    }
}

static struct overlap {
    unsigned int x : 10;
    unsigned int y : 10;
} o = {11, 22};

/* Tests that bit-fields can share allocation units. */
static void test_overlap(void)
{
    if (sizeof(struct overlap) != 3) {
        printf("Got sizeof(struct overlap) = %zu, expected 3.\n",
               sizeof(struct overlap));
        failures++;
    }

    if (o.x != 11) {
        printf("Got o.x = %u, expected 11.\n", o.x);
        failures++;
    }

    if (o.y != 22) {
        printf("Got o.y = %u, expected 22.\n", o.y);
        failures++;
    }

    o.x = 33;
    o.y = 44;

    if (o.x != 33) {
        printf("Got o.x = %u, expected 33.\n", o.x);
        failures++;
    }

    if (o.y != 44) {
        printf("Got o.y = %u, expected 44.\n", o.y);
        failures++;
    }
}

static struct overlap_with_int {
    unsigned int x : 10;
    unsigned int y : 10;
    unsigned int z;
} oi = {111, 222, 333};

static void test_overlap_with_int(void)
{
    /* First two fields in 3 bytes, then another 2 bytes. */
    if (sizeof(struct overlap_with_int) != 5) {
        printf("Got sizeof(struct overlap_with_int) = %zu, expected 5.\n",
               sizeof(struct overlap_with_int));
        failures++;
    }

    if (oi.x != 111) {
        printf("Got oi.x = %u, expected 111.\n", oi.x);
        failures++;
    }

    if (oi.y != 222) {
        printf("Got oi.y = %u, expected 222.\n", oi.y);
        failures++;
    }

    if (oi.z != 333) {
        printf("Got oi.z = %u, expected 333.\n", oi.z);
        failures++;
    }

    oi.x = 444;
    oi.y = 555;
    oi.z = 666;

    if (oi.x != 444) {
        printf("Got oi.x = %u, expected 444.\n", oi.x);
        failures++;
    }

    if (oi.y != 555) {
        printf("Got oi.y = %u, expected 555.\n", oi.y);
        failures++;
    }

    if (oi.z != 666) {
        printf("Got oi.z = %u, expected 666.\n", oi.z);
        failures++;
    }
}

int main(void)
{
    test_four_bits();
    test_four_bits_with_int();
    test_overlap();
    test_overlap_with_int();
    printf("failures: %u\n", failures);
    return failures;
}
