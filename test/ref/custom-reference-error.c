
/*
    this is an example (not actually a regression test) that shows how to
    make a check that compares the compiler (error-) output with a provided
    reference.

    to produce a reference file, first make sure your program "works" as intended,
    then "make" in this directory once and copy the produced compiler output to
    the reference:

    $ cp ../../testwrk/ref/custom-reference-error.g.6502.out custom-reference-error.cref

    and then "make" again to confirm
*/

typedef short return_t;
#error This is an/* produce an error */error

return_t main(int argc, char* argv[])
{
    printf("%02x", 0x42); /* produce an error */
    n = 0;                /* produce an error */
    /* produce a warning */
}

int arr[main(0, 0)]; /* produce an error */
int b = 0;
