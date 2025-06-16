
#include <math.h>

/* FIXME: this is really too simple */
float ceilf(float x)
{
    int n = (int) x;

    if (n >= x) {
        return n;
    }

    return n + 1;
}
