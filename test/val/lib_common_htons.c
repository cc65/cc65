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

static void CheckHtons (int input, int expected)
{
    int result = htons(input);
    if (result != expected) {
        printf ("htons error:\n"
                "  result = %d for %d, should be %d\n", result, input, expected);
        ++Failures;
    }
}

int main (void)
{
    CheckHtons(0x0000, 0x0000);
    CheckHtons(0x1234, 0x3412);
    CheckHtons(0xA0F2, 0xF2A0);
    CheckHtons(0xFFFF, 0xFFFF);

    printf ("Failures: %u\n", Failures);

    return Failures;
}
