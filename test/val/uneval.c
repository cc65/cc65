/*
  Copyright 2021, The cc65 Authors

  This software is provided "as-is", without any express or implied
  warranty. In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications; and, to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated, but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

/*
  Test of deferred operations in unevaluated context resulted from 'sizeof' and
  short-circuited code-paths in AND, OR and tenary operations.

  https://github.com/cc65/cc65/issues/1406
*/

#include <stdio.h>

int main(void)
{
    int i = 0;
    int j = 0;

    sizeof(i++ | j--);
    0 && (i++ | j--);
    1 || (i++ | j--);
    0 ? i++ | j-- : 0;
    1 ? 0 : i++ | j--;

    if (i != 0 || j != 0) {
        printf("i = %d, j = %d\n", i, j);
        printf("Failures: %d\n", i - j);
    }
    return i - j;
}
