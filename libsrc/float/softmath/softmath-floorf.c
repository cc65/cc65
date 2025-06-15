
#include <math.h>

/* FIXME: this is really too simple */
float ffloor(float x)
{
    signed long n;
    float d;

    n = (signed long)x;
    d = (float)n;

    if (x >= 0) {
        return d;
    }
    return d - 1;
}
