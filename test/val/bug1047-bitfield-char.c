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

static struct chars {
    unsigned char a : 3;
    unsigned char b : 3;
    unsigned char c : 3;
} cs = {4, 1, 3};

static void test_char_bitfield(void)
{
    if (sizeof (cs) != 2) {
        printf("Got sizeof (cs) = %zu, expected 2.\n", sizeof (cs));
        failures++;
    }

    if (cs.a != 4) {
        printf("Got cs.a = %u, expected 4.\n", cs.a);
        failures++;
    }

    if (cs.b != 1) {
        printf("Got cs.b = %u, expected 1.\n", cs.b);
        failures++;
    }

    if (cs.c != 3) {
        printf("Got cs.c = %u, expected 3.\n", cs.c);
        failures++;
    }

    cs.a = -1;
    cs.b = 6;
    cs.c = 1;

    if (cs.a != 7) {
        printf("Got cs.a = %u, expected 7.\n", cs.a);
        failures++;
    }

    if (cs.b != 6) {
        printf("Got cs.b = %u, expected 6.\n", cs.b);
        failures++;
    }

    if (cs.c != 1) {
        printf("Got cs.c = %u, expected 1.\n", cs.c);
        failures++;
    }
}

int main(void)
{
    test_char_bitfield();
    printf("failures: %u\n", failures);
    return failures;
}
