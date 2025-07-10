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
  Tests of promotions of character types.
*/

#include <stdio.h>

typedef unsigned char u8;

static unsigned char failures = 0;

void test_sub (void)
{
    const u8 one = 1, two = 2;

    /* For any unsigned type other than unsigned char, (T) 1 - (T) 2 > 0. */
    if (1U - 2U < 0) {
        fprintf (stderr, "Expected 1U - 2U > 0\n");
        failures++;
    }

    /* The unsigned chars get promoted to int, so this is negative. */
    if (one - two > 0) {
        fprintf (stderr, "Expected one - two < 0\n");
        failures++;
    }

    /* Test the constant expression code paths. */
    if ((u8) 1 - (u8) 2 > 0) {
        fprintf (stderr, "Expected (u8) 1 - (u8) 2 < 0\n");
        failures++;
    }
}

void test_mul (void)
{
    const u8 two_fifty_five = 255;
    const u8 sixteen = 16;
    int x;

    if (255U * 255U != 65025U) {
        fprintf (stderr, "Expected 255U * 255U == 65025U\n");
        failures++;
    }
#if 0
    /* Disabled pending fix of #1310. */
    if (255 * 255 != -511) {
        fprintf (stderr, "Expected 255 * 255 == -511, got: %d\n", 255 * 255);
        failures++;
    }
#endif

    /* The unsigned chars get promoted to int, so this is -511.
    ** We should also be able to observe that, due to optimizations from #1315, the generated code
    ** uses umul, not mul.
    */
    if (two_fifty_five * two_fifty_five != -511) {
        fprintf (stderr, "Expected two_fifty_five * two_fifty_five == -511\n");
        failures++;
    }
#if 0
    /* Disabled pending fix of #1310. */
    if ((u8) 255 * (u8) 255 != -511) {
        fprintf (stderr, "Expected (u8) 255 * (u8) 255 == -511, got: %d\n",
                 (u8) 255 * (u8) 255);
        failures++;
    }
#endif

    /* This should compile to a shift. */
    x = sixteen * 4;
    if (x != 64) {
        fprintf (stderr, "Expected sixteen * 4 == 64, got: %d\n", x);
        failures++;
    }
}

void test_div (void)
{
    const u8 seventeen = 17;
    const u8 three = 3;
    int x;

    /* We should also be able to observe that, due to optimizations from #1315, the generated code
    ** uses udiv, not div.
    */
    if (seventeen / three != 5) {
        fprintf (stderr, "Expected seventeen / three == 5, got: %d\n", seventeen / three);
        failures++;
    }
    if ((u8) 17 / (u8) 3 != 5) {
        fprintf (stderr, "Expected (u8) 17 / (u8) 3 == 5, got: %d\n", (u8) 17 / (u8) 3);
        failures++;
    }

    /* Ideally, this would compile to a logical shift, but that does not happen currently. */
    x = seventeen / 4;
    if (x != 4) {
        fprintf (stderr, "Expected seventeen / 4 == 4, got: %d\n", x);
        failures++;
    }
}

void test_mod (void)
{
    const u8 seventeen = 17;
    /* Ideally, this would compile to a bitwise and, but that does not happen currently. */
    int x = seventeen % 4;
    if (x != 1) {
        fprintf (stderr, "Expected seventeen %% 4 == 1, got: %d\n", x);
        failures++;
    }
}

void test_shr (void)
{
    const unsigned int forty_two = 42;
    const unsigned int two = 2;
    int x;

    /* We should also be able to observe that, due to optimizations from #1315, the generated code
    ** uses shr, not asr.
    */
    if (forty_two >> two != 10) {
        fprintf (stderr, "Expected forty_two >> two == 10, got: %d\n", forty_two >> two);
        failures++;
    }
    if ((u8) 42 >> (u8) 2 != 10) {
        fprintf (stderr, "Expected (u8) 42 >> (u8) 2 == 10, got: %d\n", (u8) 42 >> (u8) 3);
        failures++;
    }

    /* Ideally, this would compile to a logical shift, but that does not happen currently. */
    x = forty_two >> 2;
    if (x != 10) {
        fprintf (stderr, "Expected forty_two >> 2 == 10, got: %d\n", x);
        failures++;
    }

    /* Ideally, this would compile to a logical shift, but that does not happen currently. */
    x = 42 >> two;
    if (x != 10) {
        fprintf (stderr, "Expected 42 >> two == 10, got: %d\n", x);
        failures++;
    }
}

int main (void)
{
    test_sub ();
    test_mul ();
    test_div ();
    test_mod ();
    test_shr ();
    printf ("failures: %u\n", failures);
    return failures;
}
