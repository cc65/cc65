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

void ref_srand(int x)
{
	uint32_t l = (x >> 0) & 0xFF;
	uint32_t h = (x >> 8) & 0xFF;
	seed =
		(l << 0) |
		(h << 8) |
		(h << 16) |
		(l << 24);
}

int ref_rand()
{
	int output;
	/* seed follows the LCG sequence * 0x01010101 + 0xB3B3B3B3 */
	seed = seed * 0x01010101UL + 0xB3B3B3B3UL;
	/* output uses the top two bytes (reversed) XOR with bottom two bytes */
	{
		int s0 = (seed >>  0) & 0xFF;
		int s1 = (seed >>  8) & 0xFF;
		int s2 = (seed >> 16) & 0xFF;
		int s3 = (seed >> 24) & 0xFF;
		int o0 = s3 ^ s1;
		int o1 = s2 ^ s0;
		output = o0 | (o1 << 8);
	}
	return output & 0x7FFF;
}

int main(void)
{
	unsigned int i,j;
	int a,b;
	
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
