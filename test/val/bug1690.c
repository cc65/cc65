/* OptCmp1 messed up with labels */

#include <stdio.h>

int main(void)
{
    register unsigned int x = 0x200;
    register unsigned int y = 0;

    do
    {
        ++y;
    }
    while (--x);

    if (y != 0x200)
	{
		printf("0x%X\n", y);
		return 1;
	}

	return 0;
}
