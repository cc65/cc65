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
  Tests bit-field in if condition; see https://github.com/cc65/cc65/issues/1139
*/

#include <stdio.h>

static unsigned char failures = 0;

static struct four_bits {
    unsigned int x : 4;
} fb = {1};

static struct overlap {
    unsigned int x : 10;
    unsigned int y : 10;
} o = {11, 22};

static struct full_width {
    unsigned int x : 8;
    unsigned int y : 16;
} fw = {255, 17};

static struct aligned_end {
    unsigned int : 2;
    unsigned int x : 6;
    unsigned int : 3;
    unsigned int y : 13;
    /* z crosses a byte boundary, but fits in a byte when shifted. */
    unsigned int : 6;
    unsigned int z : 7;
} ae = {63, 17, 100};

/* Test using bit-fields in if conditions. */
static void test_if(void)
{
    /* Original test case for the bug. */
    o.x = 0;
    o.y = 44;
    if (o.x) {
        printf("Bad, o.x is false\n");
        failures++;
    } else {
        printf("Good\n");
    }

    /* Additionally, test most fields from bitfield.c to try to cover all start/end situations. */
    /* four_bits */
    fb.x = 1;
    if (fb.x) {
        printf("Good\n");
    } else {
        printf("Bad, fb.x is true (1)\n");
        failures++;
    }

    fb.x = 0;
    if (fb.x) {
        printf("Bad, fb.x is false\n");
        failures++;
    } else {
        printf("Good\n");
    }

    /* overlap */
    o.x = 123;
    if (o.x) {
        printf("Good\n");
    } else {
        printf("Bad, o.x is true (123)\n");
        failures++;
    }

    o.x = 0;
    if (o.x) {
        printf("Bad, o.x is false\n");
        failures++;
    } else {
        printf("Good\n");
    }

    o.y = 321;
    if (o.y) {
        printf("Good\n");
    } else {
        printf("Bad, o.y is true (321)\n");
        failures++;
    }

    o.y = 0;
    if (o.y) {
        printf("Bad, o.y is false\n");
        failures++;
    } else {
        printf("Good\n");
    }

    /* full_width */
    fw.x = 117;
    if (fw.x) {
        printf("Good\n");
    } else {
        printf("Bad, fw.x is true (117)\n");
        failures++;
    }

    fw.x = 0;
    if (fw.x) {
        printf("Bad, fw.x is false\n");
        failures++;
    } else {
        printf("Good\n");
    }

    fw.y = 32123;
    if (fw.y) {
        printf("Good\n");
    } else {
        printf("Bad, fw.y is true (32123)\n");
        failures++;
    }

    fw.y = 0;
    if (fw.y) {
        printf("Bad, fw.y is false\n");
        failures++;
    } else {
        printf("Good\n");
    }

    /* aligned_end */
    ae.x = 2;
    if (ae.x) {
        printf("Good\n");
    } else {
        printf("Bad, ae.x is true (2)\n");
        failures++;
    }

    ae.x = 0;
    if (ae.x) {
        printf("Bad, ae.x is false\n");
        failures++;
    } else {
        printf("Good\n");
    }

    ae.y = 2222;
    if (ae.y) {
        printf("Good\n");
    } else {
        printf("Bad, ae.y is true (2222)\n");
        failures++;
    }

    ae.y = 0;
    if (ae.y) {
        printf("Bad, ae.y is false\n");
        failures++;
    } else {
        printf("Good\n");
    }

    ae.z = 111;
    if (ae.z) {
        printf("Good\n");
    } else {
        printf("Bad, ae.z is true (111)\n");
        failures++;
    }

    ae.z = 0;
    if (ae.z) {
        printf("Bad, ae.z is false\n");
        failures++;
    } else {
        printf("Good\n");
    }
}

/* Test using bit-fields in inverted if conditions. */
static void test_if_not(void)
{
    /* Original test case for the bug, inverted. */
    o.x = 0;
    o.y = 44;
    if (!o.x) {
        printf("Good\n");
    } else {
        printf("Bad, o.x is false\n");
        failures++;
    }

    /* Additionally, test most fields from bitfield.c to try to cover all start/end situations. */
    /* four_bits */
    fb.x = 1;
    if (!fb.x) {
        printf("Bad, fb.x is true (1)\n");
        failures++;
    } else {
        printf("Good\n");
    }

    fb.x = 0;
    if (!fb.x) {
        printf("Good\n");
    } else {
        printf("Bad, fb.x is false\n");
        failures++;
    }

    /* overlap */
    o.x = 123;
    if (!o.x) {
        printf("Bad, o.x is true (123)\n");
        failures++;
    } else {
        printf("Good\n");
    }

    o.x = 0;
    if (!o.x) {
        printf("Good\n");
    } else {
        printf("Bad, o.x is false\n");
        failures++;
    }

    o.y = 321;
    if (!o.y) {
        printf("Bad, o.y is true (321)\n");
        failures++;
    } else {
        printf("Good\n");
    }

    o.y = 0;
    if (!o.y) {
        printf("Good\n");
    } else {
        printf("Bad, o.y is false\n");
        failures++;
    }

    /* full_width */
    fw.x = 117;
    if (!fw.x) {
        printf("Bad, fw.x is true (117)\n");
        failures++;
    } else {
        printf("Good\n");
    }

    fw.x = 0;
    if (!fw.x) {
        printf("Good\n");
    } else {
        printf("Bad, fw.x is false\n");
        failures++;
    }

    fw.y = 32123;
    if (!fw.y) {
        printf("Bad, fw.y is true (32123)\n");
        failures++;
    } else {
        printf("Good\n");
    }

    fw.y = 0;
    if (!fw.y) {
        printf("Good\n");
    } else {
        printf("Bad, fw.y is false\n");
        failures++;
    }

    /* aligned_end */
    ae.x = 2;
    if (!ae.x) {
        printf("Bad, ae.x is true (2)\n");
        failures++;
    } else {
        printf("Good\n");
    }

    ae.x = 0;
    if (!ae.x) {
        printf("Good\n");
    } else {
        printf("Bad, ae.x is false\n");
        failures++;
    }

    ae.y = 2222;
    if (!ae.y) {
        printf("Bad, ae.y is true (2222)\n");
        failures++;
    } else {
        printf("Good\n");
    }

    ae.y = 0;
    if (!ae.y) {
        printf("Good\n");
    } else {
        printf("Bad, ae.y is false\n");
        failures++;
    }

    ae.z = 111;
    if (!ae.z) {
        printf("Bad, ae.z is true (111)\n");
        failures++;
    } else {
        printf("Good\n");
    }

    ae.z = 0;
    if (!ae.z) {
        printf("Good\n");
    } else {
        printf("Bad, ae.z is false\n");
        failures++;
    }
}

int main(void)
{
    test_if();
    test_if_not();
    printf("failures: %u\n", failures);
    return failures;
}
