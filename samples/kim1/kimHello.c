// --------------------------------------------------------------------------
// Hello World for KIM-1
//
// Dave Plummer based on Sym-1 sample by Wayne Parham
//
// davepl@davepl.com
// --------------------------------------------------------------------------

#include <stdio.h>
#include <kim1.h>

int main (void)
{
   char str[100];
   char c = 0x00;

   printf ("\nHello World!\n\n");
   printf ("Type a line and press ENTER, please.\n\n");

   gets( str );

   printf ("\n\nThanks: %s\n\n", str);
   return 0;
}
