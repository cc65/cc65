/*
  !!DESCRIPTION!!
  !!ORIGIN!!      testsuite
  !!LICENCE!!     Public Domain
  !!AUTHOR!!      Marc 'BlackJack' Rintsch
*/

/*
Compiler is build from cc65-snapshot-2.13.9.20101031 sources.

Expected results and also what I get from this without any optimisations
are: 48663 and 49218

When I turn on ``-O``: 58096 and 58096.  After swapping the two variable
declaration lines in `calculate_checksum()` the results are correct
with ``-O``.

But with ``--O --static-locals`` the results are incorrect again (31757
and 15408).  ``--static-locals`` alone works though.
*/

#include <stdio.h>
#include <stdint.h>

// uint16_t __fastcall__ calculate_checksum(uint8_t *block);
uint8_t block[256];

uint16_t calculate_checksum(uint8_t *block)
{
    uint16_t i, result = 0xffff;
    uint8_t j;

    for (i = 0; i < 256; ++i) {
        result ^= block[i] << 8;
        for (j = 0; j < 8; ++j) {
            if (result & (1 << 15)) {
                result = (result << 1) ^ 0x1021;
            } else {
                result <<= 1;
            }
        }
    }
    return ~result;
}

int main(void)
{
    uint16_t i;

    printf("zeroes: %u\n", calculate_checksum(block));
    for (i = 0; i < 256; ++i) block[i] = i;
    printf("0..255: %u\n", calculate_checksum(block));

    return 0;
}

