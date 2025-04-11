
#include <math.h>

float fmodf(float x, float y)
{
    float res;
    res = x / y;
    return x - (truncf(res) * y);
}
