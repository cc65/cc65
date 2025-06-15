
#include <math.h>

float powf(float x, float y)
{
    return expf(x * logf(y));
}
