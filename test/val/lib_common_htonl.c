/*
  !!DESCRIPTION!! A small test for htons.
  !!ORIGIN!!
  !!LICENCE!!
  !!AUTHOR!! Colin Leroy-Mira
*/

#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

static unsigned int Failures = 0;

static void CheckHtonl (long input, long expected)
{
    long result = htonl(input);
    if (result != expected) {
        printf ("htonl error:\n"
                "  result = %ld for %ld, should be %ld\n", result, input, expected);
        ++Failures;
    }
}

int main (void)
{
    CheckHtonl(0x00000000, 0x00000000);
    CheckHtonl(0x12345678, 0x78563412);
    CheckHtonl(0xAABBCCDD, 0xDDCCBBAA);
    CheckHtonl(0xFFFFFFFF, 0xFFFFFFFF);

    printf ("Failures: %u\n", Failures);

    return Failures;
}
