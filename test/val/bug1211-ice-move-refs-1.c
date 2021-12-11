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
  Test of indirect goto with label merge ICE.
  https://github.com/cc65/cc65/issues/1211
  This test case works because CS_MergeLabels has a hack to keep the "unreferenced" label
  (i.e. the one referenced in a data segment).
*/

#include <stdio.h>

/* When operating correctly, f(0) = 31 and f(1) = 41. */
int f (int x)
{
    static const void *const labels[] = {&&L0, &&L1};
    goto *labels[x];
L0: if (labels[0] != labels[1]) return 31;
    else return 13;
L1: return 41;
}

static unsigned char failures = 0;

int main (void)
{
    if (f (0) != 31) failures++;

    if (failures == 0) {
        printf ("PASS\n");
    } else {
        printf ("FAIL\n");
    }

    return failures;
}
