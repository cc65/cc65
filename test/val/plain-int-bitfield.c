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
  Tests that plain int bit-fields are unsigned.
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
        printf ("Got pi.x = %u, expected 15.\n", pi.x);
        failures++;
    }
    if (pi.y != 700) {
        printf ("Got pi.y = %u, expected 700.\n", pi.y);
        failures++;
    }

    pi.x = 3;
    pi.y = 1023;

    if (pi.x != 3) {
        printf ("Got pi.x = %u, expected 3.\n", pi.x);
        failures++;
    }
    if (pi.y != 1023) {
        printf ("Got pi.y = %u, expected 1023.\n", pi.y);
        failures++;
    }
}

int main (void)
{
    test_plain_int_bitfields ();
    printf ("failures: %u\n", failures);
    return failures;
}
