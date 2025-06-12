#include <stdint.h>
#include <stdio.h>

int res = 0;

int main(void)
{
    static long a, b;

    a = 0x12345678L;

    /* Test assignment */
    b = a;
    if (b != a) {
      res++;
    }

    /* Test increment */
    b++;
    if (b != 0x12345679L) {
      res++;
    }

    /* Test decrement */
    b--;
    if (b != 0x12345678L) {
      res++;
    }

    /* Test pre-decrement with test */
    if (--b != 0x12345677L) {
      res++;
    }

    a = --b;
    if (a != 0x12345676L) {
      res++;
    }

    return res;
}
