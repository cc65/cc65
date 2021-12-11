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
  Tests bit-field bug.  https://github.com/cc65/cc65/issues/1332
*/

#include <stdio.h>
#include <string.h>

static unsigned char failures = 0;

static const struct bitfield {
    unsigned lsb : 1;
    unsigned pad : 14;
    unsigned msb : 1;
} b = {1, 0, 1};

static unsigned v;

void test_if_val (void)
{
    /* Gets appropriate garbage (0x12) into .X so the test fails. */
    v = 0x1234;

    if (b.msb == 1) {
        fprintf (stderr, "if (msb == 1) OK\n");
    } else {
        fprintf (stderr, "if (msb == 1) FAILED\n");
        failures++;
    }
}

void test_sprintf (void)
/* This test case is similar to the original bug report. */
{
    char buf[10];
    snprintf (buf, sizeof (buf), "%u", b.msb);

    if (strcmp (buf, "1") != 0) {
        fprintf (stderr, "Expected: sprintf (msb) == \"1\", got: %s\n", buf);
        failures++;
    } else {
        fprintf (stderr, "sprintf (msb) OK\n");
    }
}

int main (void)
{
    test_if_val ();
    test_sprintf ();

    printf ("failures: %u\n", failures);
    return failures;
}
