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
  Tests of enum bit-fields; see https://github.com/cc65/cc65/issues/1244
*/

#include <stdio.h>

static unsigned char failures = 0;

/* Enum with underlying type unsigned int. */
enum e10u {
    E10U_200 = 200,
    E10U_1000 = 1000,
};

static struct enum_bitfield_uint {
    enum e10u x : 1;
    enum e10u y : 8;
    enum e10u z : 16;
} e10ubf = {0, E10U_200, E10U_1000};

static void test_enum_bitfield_uint(void)
{
    if (sizeof (struct enum_bitfield_uint) != 4) {
        printf ("Got sizeof(struct enum_bitfield_uint) = %zu, expected 4.\n",
                sizeof(struct enum_bitfield_uint));
        failures++;
    }

    if (e10ubf.x != 0) {
        printf ("Got e10ubf.x = %u, expected 0.\n", e10ubf.x);
        failures++;
    }
    if (e10ubf.y != 200) {
        printf ("Got e10ubf.y = %u, expected 200.\n", e10ubf.y);
        failures++;
    }
    if (e10ubf.z != 1000) {
        printf ("Got e10ubf.z = %u, expected 1000.\n", e10ubf.z);
        failures++;
    }

    e10ubf.x = 1;
    e10ubf.y = 17;
    e10ubf.z = 1023;

    if (e10ubf.x != 1) {
        printf ("Got e10ubf.x = %u, expected 1.\n", e10ubf.x);
        failures++;
    }

    /* Check signedness, should be unsigned. */
    {
        long v = e10ubf.x - 2;
        if (v < 0) {
            printf ("Got negative v = %ld, expected large positive.\n", v);
            failures++;
        }
    }

    if (e10ubf.y != 17) {
        printf ("Got e10ubf.y = %u, expected 17.\n", e10ubf.y);
        failures++;
    }
    if (e10ubf.z != 1023) {
        printf ("Got e10ubf.z = %u, expected 1023.\n", e10ubf.z);
        failures++;
    }
}

/* Enum with underlying type signed int. */
enum e11i {
    E11I_M1 = -1,
    E11I_100 = 100,
    E11I_1000 = 1000,
};

static struct enum_bitfield_int {
    enum e11i x : 2;
    enum e11i y : 8;
    enum e11i z : 16;
} e11ibf = {E11I_M1, E11I_100, E11I_1000};

static void test_enum_bitfield_int(void)
{
    if (sizeof (struct enum_bitfield_int) != 4) {
        printf ("Got sizeof(struct enum_bitfield_int) = %zu, expected 4.\n",
                sizeof(struct enum_bitfield_int));
        failures++;
    }

    if (e11ibf.x != -1) {
        printf ("Got e11ibf.x = %d, expected -1.\n", e11ibf.x);
        failures++;
    }
    if (e11ibf.y != 100) {
        printf ("Got e11ibf.y = %d, expected 100.\n", e11ibf.y);
        failures++;
    }
    if (e11ibf.z != 1000) {
        printf ("Got e11ibf.z = %d, expected 1000.\n", e11ibf.z);
        failures++;
    }

    e11ibf.x = 1;
    e11ibf.y = 17;
    e11ibf.z = 1023;

    if (e11ibf.x != 1) {
        printf ("Got e11ibf.x = %d, expected 1.\n", e11ibf.x);
        failures++;
    }

    /* Check signedness, should be signed. */
    {
        long v = e11ibf.x - 2;
        if (v > 0) {
            printf ("Got positive v = %ld, expected negative.\n", v);
            failures++;
        }
    }

    if (e11ibf.y != 17) {
        printf ("Got e11ibf.y = %d, expected 17.\n", e11ibf.y);
        failures++;
    }
    if (e11ibf.z != 1023) {
        printf ("Got e11ibf.z = %d, expected 1023.\n", e11ibf.z);
        failures++;
    }
}

int main(void)
{
    test_enum_bitfield_uint();
    test_enum_bitfield_int();
    printf("failures: %u\n", failures);
    return failures;
}
