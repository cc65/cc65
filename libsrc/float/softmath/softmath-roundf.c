
#include <math.h>

/* FIXME: this is really too simple */
float roundf(float x)
{
    if (x > 0.0f) {
        return (float)((signed long)(x + 0.5f));
    }
    return (float)((signed long)(x - 0.5f));
}
