
#include <stdlib.h>

int main(void)
{
// this works
c_label:
  asm("inx\n"
      "bne %g\n",
      c_label);

// this does not work
c_label2:
  asm("inx\n"
      "bne %g        \n",
      c_label2);

    return EXIT_SUCCESS;
}
