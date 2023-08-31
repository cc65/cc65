
#include <math.h>

float roundf(float x)
{
    if (x > 0.0f) {
        return (float)((signed)(x + 0.5f));
    }
    return (float)((signed)(x - 0.5f));
}
