
// bug #1252 - inline asm: cc65 chokes on label ref if space/tab follows

#include <stdlib.h>

int main(void)
{
// first test the recommended way to use labels in inline assembly:

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

// now the following is from the original bug report. note that using labels
// this way only works by chance - the name of the label may clash with
// generated labels

// this works
  asm("label1: inx\n"
      "bne label1\n");

// this does not work
  asm("label2: inx\n"
      "bne label2        \n");

// a variant of the above using local labels.

// this works
  asm("@label1: inx\n"
      "bne @label1\n");

// this does not work
  asm("@label2: inx\n"
      "bne @label2        \n");

    return EXIT_SUCCESS;
}
