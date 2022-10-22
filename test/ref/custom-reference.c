
/*
    this is an example (not actually a regression test) that shows how to
    make a check that compares the compiler (error-) output with a provided
    reference.

    to produce a reference file, first make sure your program "works" as intended,
    then "make" in this directory once and copy the produced compiler output to
    the reference:

    $ cp ../../testwrk/ref/custom-reference.g.6502.out custom-reference.cref

    and then "make" again to confirm
*/

#include <stdint.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
    printf("%02x", 0x42);
    /* produce a warning */
}
