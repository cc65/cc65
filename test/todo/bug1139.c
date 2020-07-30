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

static struct overlap {
    unsigned int x : 10;
    unsigned int y : 10;
} o = {11, 22};

/* Test using bit-fields in if conditions. */
static void test_if(void)
{
    o.x = 0;
    o.y = 44;
    if (o.x) {
        printf("Bad, o.x is false\n");
        failures++;
    } else {
        printf("Good\n");
    }
}

int main(void)
{
    test_if();
    printf("failures: %u\n", failures);
    return failures;
}
