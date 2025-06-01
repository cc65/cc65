/* This test verifies that the assembly implementation of rand() matches its
 * theoretical high level equivalent.
 *
 * This does about 3000 tests from various starting srand() seeds.
 * A more thorough test might visit the entire sequence with 2^32 tests, but
 * that takes hours to simulate, and this should be a sufficient sampling.
 *
 * This will also fail if rand() is ever altered, which might be a warning to
 * tread carefully. Some past discussion of RNG here:
 * https://github.com/cc65/cc65/pull/951
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

/* for faster execution */
#pragma static-locals (on)

/* values tested per seed */
#define SUBTESTS 50

/* increments used between tested seeds */
/* 653 is prime and divides 32768 by ~50 */
#define TESTINC 653

static uint32_t seed;

int ref_rand()
{
        uint16_t output;
        /* seed follows the LCG sequence * 0x01010101 + 0xB3B3B3B3 */
        seed = seed * 0x01010101UL + 0xB3B3B3B3UL;
        /* output uses the top two bytes (reversed) XOR with bottom two bytes */
        {
                uint16_t s0 = (seed >>  0) & 0xFF;
                uint16_t s1 = (seed >>  8) & 0xFF;
                uint16_t s2 = (seed >> 16) & 0xFF;
                uint16_t s3 = (seed >> 24) & 0xFF;
                uint16_t o0 = s3 ^ s1;
                uint16_t o1 = s2 ^ s0;
                output = o0 | (o1 << 8);
        }
        return (int)(output & 0x7FFF);
}

void ref_srand(int ax)
{
        uint32_t s = (unsigned int)ax;
        seed = s | (s << 16); /* low 16 bits is convenient filler for high 16 bits */
        ref_rand(); /* one pre-call "shuffles" the first rand() result so it isn't too predictable */
}

int main(void)
{
        unsigned int i,j;
        int a,b;

        /* test that startup state is equivalent to srand(1) */
        {
                //srand(1); // implied
                ref_srand(1);
                for (j=0; j<SUBTESTS; ++j)
                {
                        a = rand();
                        b = ref_rand();
                        if (a != b)
                        {
                                printf("failed startup seed at test %d. rand()=%d reference=%d\n",j,a,b);
                                return EXIT_FAILURE;
                        }
                }
        }

        /* test every power of 2 seed */
        for (i = 0; i < 16; ++i)
        {
                srand(1<<i);
                ref_srand(1<<i);
                for (j=0; j<SUBTESTS; ++j)
                {
                        a = rand();
                        b = ref_rand();
                        if (a != b)
                        {
                                printf("failed seed %d at test %d. rand()=%d reference=%d\n",(1<<i),j,a,b);
                                return EXIT_FAILURE;
                        }
                }
        }

        /* test a sampling of seeds*/
        for (i = 0; i < 32768UL; i += TESTINC)
        {
                srand(i);
                ref_srand(i);
                for (j=0; j<SUBTESTS; ++j)
                {
                        a = rand();
                        b = ref_rand();
                        if (a != b)
                        {
                                printf("failed seed %d at test %d. rand()=%d reference=%d\n",(1<<i),j,a,b);
                                return EXIT_FAILURE;
                        }
                }
        }

        return EXIT_SUCCESS;
}
