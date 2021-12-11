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
** Tests of passing _Static_asserts.
**/



#include <assert.h>

_Static_assert (1, "1 should be true.");
_Static_assert (1);  /* Support C2x syntax with no message. */
_Static_assert (!0, "!0 should be true.");
_Static_assert (1 == 1, "1 == 1 should be true.");
_Static_assert (1 == 1L, "1 == 1L should be true.");
_Static_assert (1 != 0, "1 != 0 should be true.");
_Static_assert (sizeof (char) == 1, "sizeof (char) should be 1.");
_Static_assert (sizeof (int) == 2, "sizeof (int) should be 2.");

/* Make sure we can also do structs. */
struct sc { char a; };
_Static_assert (sizeof (struct sc) == 1, "sizeof (struct sc) should be 1.");
struct si { int a; };
_Static_assert (sizeof (struct si) == 2, "sizeof (struct si) should be 2.");

/* Try enums. */
enum { k = 1 };
_Static_assert (k == 1, "k should be 1.");

/* Just test the macro version once. */
static_assert (1, "1 should be true.");
static_assert (1);

/* _Static_assert can appear anywhere a declaration can. */
void f (void)
{
    _Static_assert (1, "1 should still be true.");
    if (1) {
        _Static_assert (1, "1 should still be true.");
    }
}

/* _Static_assert can also appear in structs. */
struct S {
    int a;
    _Static_assert (1, "1 should still be true.");
    int b;
};


int main (void)
{
    return 0;
}
