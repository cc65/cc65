/*
  Copyright 2022 The cc65 Authors

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
  Tests of unsigned short/unsigned int vs signed long comparisons;
  see https://github.com/cc65/cc65/issues/1696
*/

#include <stdio.h>

static unsigned char failures = 0;

int main(void)
{
	unsigned int x = 65535;
	unsigned short y = 65535;
	if (!(x > 1L)) {
		printf("x = %ld but x > 1L failed\n", (long)x);
		++failures;
	}
	if (!(y == 65535L)) {
		printf("y = %ld but y == 65535L failed\n", (long)y);
		++failures;
	}
    printf("failures: %u\n", failures);
    return failures;
}
