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
  Tests of constant expressions.  https://github.com/cc65/cc65/issues/1310
*/

#include <stdio.h>

static unsigned char failures = 0;

int main (void)
{
    /* 255 * 255 is signed integer overflow, so UB, but it would be nice if
    ** (1) there were a warning, and (2) it did the "obvious" thing.
    */
    const int two_fifty_five = 255;
    const int two_fifty_five_squared = 255 * 255;

    /* Unsigned overflow is not UB, but has similar problems with comparison. */
    const int two_fifty_six = 256U;
    const int two_fifty_six_squared = 256U * 256U;

    if (255 * 255 != -511) {
        fprintf (stderr, "Expected 255 * 255 == -511, got: %d\n", 255 * 255);
        failures++;
    }
    if (two_fifty_five * two_fifty_five != -511) {
        fprintf (stderr, "Expected two_fifty_five * two_fifty_five == -511, got: %d\n",
                 two_fifty_five * two_fifty_five);
        failures++;
    }
    if (two_fifty_five_squared != -511) {
        fprintf (stderr, "Expected two_fifty_five_squared == -511, got: %d\n",
                 two_fifty_five_squared);
        failures++;
    }

    if (256U * 256U != 0) {
        fprintf (stderr, "Expected 256U * 256U == 0, got: %d\n", 256U * 256U);
        failures++;
    }
    if (two_fifty_six * two_fifty_six != 0) {
        fprintf (stderr, "Expected two_fifty_six * two_fifty_six == 0, got: %d\n",
                 two_fifty_six * two_fifty_six);
        failures++;
    }
    if (two_fifty_six_squared != 0) {
        fprintf (stderr, "Expected two_fifty_six_squared == 0, got: %d\n",
                 two_fifty_six_squared);
        failures++;
    }

    if (-32768U != 32768U) {
        fprintf (stderr, "Expected -32768U == 32768U, got: %ld\n", (long)-32768U);
        failures++;
    }
    if (~32767U != 32768U) {
        fprintf (stderr, "Expected ~32767U == 32768U, got: %ld\n", (long)~32767U);
        failures++;
    }

    if ((long*)0x1000 - (long*)0x2000 >= 0) {
        fprintf (stderr, "Expected (long*)0x1000 - (long*)0x2000 < 0, got: %ld\n", (long*)0x1000 - (long*)0x2000);
        failures++;
    }
    printf ("failures: %u\n", failures);

    return failures;
}
