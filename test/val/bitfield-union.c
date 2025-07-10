/*
  Copyright 2020-2023 The cc65 Authors

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
  Tests of union of bit-fields; see https://sourceforge.net/p/cc65/mailman/message/36152700/
*/

#include <stdio.h>

typedef union {
     const unsigned int : 1;
     unsigned int bf;

     struct {
         unsigned int a : 1;
         unsigned int b : 1;
         unsigned int c : 1;
     };
} bitfield_t;

static unsigned char failures = 0;

int main (void)
{
     bitfield_t bitfield = { 42 };

     printf ("Bitfield: %u\n", bitfield.bf);
     if (bitfield.bf != 42) failures++;

     bitfield.bf ^= 42;
     printf ("Bitfield: %u\n", bitfield.bf);
     if (bitfield.bf != 0) failures++;

     bitfield.a = bitfield.a ^ 1;
     printf ("a=1: %u\n", bitfield.bf);
     if (bitfield.bf != 1) failures++;

     bitfield.a = bitfield.a ^ 1;
     printf ("a=0: %u\n\n", bitfield.bf);
     if (bitfield.bf != 0) failures++;

     bitfield.b = bitfield.b ^ 1;
     printf ("b=1: %u\n", bitfield.bf);
     if (bitfield.bf != 2) failures++;

     bitfield.b = bitfield.b ^ 1;
     printf ("b=0: %u\n\n", bitfield.bf);
     if (bitfield.bf != 0) failures++;

     bitfield.c = bitfield.c ^ 1;
     printf ("c=1: %u\n", bitfield.bf);
     if (bitfield.bf != 4) failures++;

     bitfield.c = bitfield.c ^ 1;
     printf ("c=0: %u\n\n", bitfield.bf);
     if (bitfield.bf != 0) failures++;

     return failures;
}
