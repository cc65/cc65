
#include <math.h>

float ceilf(float x)
{
    int n = (int) x;

    if (n >= x) {
        return n;
    }

    return n + 1;
}
