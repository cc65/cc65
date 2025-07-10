/* Bug #1822 - Redefined macros failed to be all undefined with a single #undef */

#undef F
#undef F

#define F 1
#define F 1

#undef F
#if defined F
#error #undef F fails!
#endif

#define F 0

#include <stdio.h>

int main(void)
{
    if (F != 0)
    {
        printf("failed: F = %d\n", F);
    }
    return F;
}
