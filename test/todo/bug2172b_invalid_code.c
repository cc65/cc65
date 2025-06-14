
#include <stdlib.h>
#include <stdio.h>

/* Just some arbitrary code, more fun with goto */
int func(int m)
{
    long x = -42;               /* sp: -4 */
    switch (x) {
        /* return 0; // C99 only */
        int i = 42;             /* sp: -6 */
L0:
        --i;
default:
        if (i != 0) {
            long j = 13;        /* sp: -10 */
            goto L1;
L1:
case 0x7FFF01:
            m--;
case 0x7EFF0001:
case 0x7FFF0001:
            i++;
        }                       /* sp: -6 */
case 0x7FFF00:
case 0x7FFF0000:
        break;
        goto L0;
        {
            int skipped = 42;   /* sp: -8 */
case 0x7EFF00:
case 0x7EFF0000:
            ++skipped;
        }                       /* sp: -6 */
    }                           /* sp: -4 */

    return m;
}

int err = 0;

int main(void)
{
    int n = 42;
    n = func(7);
    if (n != 7) {
        err++;
    }
    printf("n:%d\n", n);
    return err;
}
