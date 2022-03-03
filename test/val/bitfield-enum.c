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
  Tests of enum bit-fields; see https://github.com/cc65/cc65/issues/1244
*/

#include <stdio.h>
#include <limits.h>

static unsigned char failures = 0;

/* Enum with underlying type unsigned int. */
enum e10u {
    E10U_200 = 200,
    E10U_1000 = 1000,
};

static struct enum_bitfield_uint {
    enum e10u x : 1;
    enum e10u y : 8;
    enum e10u z : CHAR_BIT * sizeof (enum e10u);
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

    /* Check signedness, should be signed. */
    {
        long v = e10ubf.x - 2;
        if (v >= 0) {
            printf ("Got non-negative v (= e10ubf.x - 2) = %ld, expected negative.\n", v);
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

    /* Check signedness, should be unsigned. */
    {
        long v = e10ubf.z - 1024;
        if (v < 0) {
            printf ("Got negative v (= e10ubf.z - 1024) = %ld, expected positive.\n", v);
            failures++;
        }
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
    enum e11i z : CHAR_BIT * sizeof (enum e11i);
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
        if (v >= 0) {
            printf ("Got non-negative v (= e11ibf.x - 2) = %ld, expected negative.\n", v);
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

    /* Check signedness, should be signed. */
    {
        long v = e11ibf.z - 1024;
        if (v >= 0) {
            printf ("Got non-negative v (= e11ibf.z - 1024) = %ld, expected negative.\n", v);
            failures++;
        }
    }
}

/* Enum with underlying type unsigned char. */
enum e7uc {
    E7UC_100 = 100,
};

static struct enum_bitfield_uchar {
    enum e7uc x : 1;
    enum e7uc y : 4;
    enum e7uc z : CHAR_BIT;
} e7ucbf = {0, 10, E7UC_100};

static void test_enum_bitfield_uchar(void)
{
    if (sizeof (struct enum_bitfield_uchar) != 2) {
        printf ("Got sizeof(struct enum_bitfield_uchar) = %zu, expected 2.\n",
                sizeof(struct enum_bitfield_uchar));
        failures++;
    }

    if (e7ucbf.x != 0) {
        printf ("Got e7ucbf.x = %u, expected 0.\n", e7ucbf.x);
        failures++;
    }
    if (e7ucbf.y != 10) {
        printf ("Got e7ucbf.y = %u, expected 10.\n", e7ucbf.y);
        failures++;
    }
    if (e7ucbf.z != 100) {
        printf ("Got e7ucbf.z = %u, expected 100.\n", e7ucbf.z);
        failures++;
    }

    e7ucbf.x = -1;  /* Will store 1. */
    e7ucbf.y = -1;  /* Will store 15. */
    e7ucbf.z = 127;

    /* Both signed char and unsigned char are converted to int in arithmetic expressions,
    ** so we write this test differently to enum_bitfield_int.
    */
    if (e7ucbf.x != 1) {
        printf ("Got e7ucbf.x = %u, expected 1.\n", e7ucbf.x);
        failures++;
    }

    if (e7ucbf.y != 15) {
        printf ("Got e7ucbf.y = %u, expected 15.\n", e7ucbf.y);
        failures++;
    }
    if (e7ucbf.z != 127) {
        printf ("Got e7ucbf.z = %u, expected 127.\n", e7ucbf.z);
        failures++;
    }
}

/* Enum with underlying type signed char. */
enum e8sc {
    E8SC_M1 = -1,
    E8SC_100 = 100,
};

static struct enum_bitfield_char {
    enum e8sc x : 1;
    enum e8sc y : 4;
    enum e8sc z : CHAR_BIT;
} e8scbf = {0, 5, E8SC_100};

static void test_enum_bitfield_char(void)
{
    if (sizeof (struct enum_bitfield_char) != 2) {
        printf ("Got sizeof(struct enum_bitfield_char) = %zu, expected 2.\n",
                sizeof(struct enum_bitfield_char));
        failures++;
    }

    if (e8scbf.x != 0) {
        printf ("Got e8scbf.x = %d, expected 0.\n", e8scbf.x);
        failures++;
    }
    if (e8scbf.y != 5) {
        printf ("Got e8scbf.y = %d, expected 5.\n", e8scbf.y);
        failures++;
    }
    if (e8scbf.z != 100) {
        printf ("Got e8scbf.z = %d, expected 100.\n", e8scbf.z);
        failures++;
    }

    e8scbf.x = -1;
    e8scbf.y = -3;
    e8scbf.z = 127;

    if (e8scbf.x != -1) {
        printf ("Got e8scbf.x = %d, expected -1.\n", e8scbf.x);
        failures++;
    }
    if (e8scbf.y != -3) {
        printf ("Got e8scbf.y = %d, expected -3.\n", e8scbf.y);
        failures++;
    }
    if (e8scbf.z != 127) {
        printf ("Got e8scbf.z = %d, expected 127.\n", e8scbf.z);
        failures++;
    }
}

/* Enum with underlying type unsigned long. */
enum e20ul {
    E20UL_10 = 10,
    E20UL_1000 = 1000,
    E20UL_1000000000 = 1000000000L,
};

static struct enum_bitfield_ulong {
    enum e20ul x : 4;
    enum e20ul y : 16;
    enum e20ul z : CHAR_BIT * sizeof (enum e20ul);
} e20ulbf = {E20UL_10, E20UL_1000, E20UL_1000000000};

static void test_enum_bitfield_ulong(void)
{
    if (sizeof (struct enum_bitfield_ulong) != 7) {
        printf ("Got sizeof(struct enum_bitfield_ulong) = %zu, expected 7.\n",
                sizeof(struct enum_bitfield_ulong));
        failures++;
    }

    if (e20ulbf.x != 10) {
        printf ("Got e20ulbf.x = %u, expected 10.\n", e20ulbf.x);
        failures++;
    }
    if (e20ulbf.y != 1000) {
        printf ("Got e20ulbf.y = %u, expected 1000.\n", e20ulbf.y);
        failures++;
    }
    if (e20ulbf.z != 1000000000L) {
        printf ("Got e20ulbf.z = %ul, expected 1000000000.\n", e20ulbf.z);
        failures++;
    }

    e20ulbf.x = 8;
    e20ulbf.y = -1;  /* Will store 65535. */
    e20ulbf.z = 1048575L;

    if (e20ulbf.x != 8) {
        printf ("Got e20ulbf.x = %ld, expected 8.\n", (long)e20ulbf.x);
        failures++;
    }

    /* Check signedness, should be signed. */
    {
        if (e20ulbf.x - 9 >= 0) {
            printf ("Got non-negative e20ulbf.x - 9 = %lu, expected negative.\n", (unsigned long)(e20ulbf.x - 9));
            failures++;
        }
    }

    if (e20ulbf.y != 65535L) {
        printf ("Got e20ulbf.y = %ld, expected 65535.\n", (long)e20ulbf.y);
        failures++;
    }

    /* Check signedness, should be signed. */
    {
        if (e20ulbf.y - 65536L >= 0) {
            printf ("Got non-negative e20ulbf.y - 65536L = %lu, expected negative.\n", (unsigned long)(e20ulbf.y - 65536L));
            failures++;
        }
    }

    if (e20ulbf.z != 1048575L) {
        printf ("Got e20ulbf.z = %lu, expected 1048575.\n", (unsigned long)e20ulbf.z);
        failures++;
    }

    /* Check signedness, should be unsigned. */
    {
        if (e20ulbf.z - 1048576L < 0) {
            printf ("Got negative e20ulbf.z - 1048576 = %ld, expected positive.\n", (long)(e20ulbf.z - 1048576L));
            failures++;
        }
    }
}

/* Enum with underlying type signed long. */
enum e20sl {
    E20SL_M1 = -1,
    E20SL_1000 = 1000,
    E20SL_1000000000 = 1000000000L,
};

static struct enum_bitfield_long {
    enum e20sl x : 2;
    enum e20sl y : 16;
    enum e20sl z : CHAR_BIT * sizeof (enum e20sl);
} e20slbf = {E20SL_M1, E20SL_1000, E20SL_1000000000};

static void test_enum_bitfield_long(void)
{
    if (sizeof (struct enum_bitfield_long) != 7) {
        printf ("Got sizeof(struct enum_bitfield_long) = %zu, expected 8.\n",
                sizeof(struct enum_bitfield_long));
        failures++;
    }

    if (e20slbf.x != -1) {
        printf ("Got e20slbf.x = %ld, expected -1.\n", (long)e20slbf.x);
        failures++;
    }
    if (e20slbf.y != 1000) {
        printf ("Got e20slbf.y = %ld, expected 1000.\n", (long)e20slbf.y);
        failures++;
    }
    if (e20slbf.z != 1000000000L) {
        printf ("Got e20slbf.z = %ld, expected 1000000000.\n", (long)e20slbf.z);
        failures++;
    }

    e20slbf.x = 1;
    e20slbf.y = 257;
    e20slbf.z = 1048575L;

    if (e20slbf.x != 1) {
        printf ("Got e20slbf.x = %d, expected 1.\n", e20slbf.x);
        failures++;
    }

    /* Check signedness, should be signed. */
    {
        if (e20slbf.x - 2 >= 0) {
            printf ("Got non-negative e20slbf.x - 2 = %lu, expected negative.\n", (unsigned long)(e20slbf.x - 2));
            failures++;
        }
    }

    if (e20slbf.y != 257) {
        printf ("Got e20slbf.y = %ld, expected 257.\n", (long)e20slbf.y);
        failures++;
    }

    /* Check signedness, should be signed. */
    {
        if (e20slbf.y - 258 >= 0) {
            printf ("Got non-negative e20slbf.y - 258 = %lu, expected negative.\n", (unsigned long)(e20slbf.y - 258));
            failures++;
        }
    }

    if (e20slbf.z != 1048575L) {
        printf ("Got e20slbf.z = %ld, expected 1048575.\n", (long)e20slbf.z);
        failures++;
    }

    /* Check signedness, should be signed. */
    {
        if (e20slbf.z - 1048576L >= 0) {
            printf ("Got non-negative e20slbf.z - 1048576L = %ld, expected negative.\n", (long)(e20slbf.z - 1048576L));
            failures++;
        }
    }
}

int main(void)
{
    test_enum_bitfield_uint();
    test_enum_bitfield_int();
    test_enum_bitfield_uchar();
    test_enum_bitfield_char();
    test_enum_bitfield_ulong();
    test_enum_bitfield_long();
    printf("failures: %u\n", failures);
    return failures;
}
