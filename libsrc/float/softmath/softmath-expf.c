
#include <math.h>

#if 1

/* natural logarithm of 2 */
#define LN2 0.693147180559945309417f

float expf(float x)
{
    float p;
    int i;
    int k;

    float r;
    float tn;
    float x0 = fabsf(x);        /* FIXME: somehow this doesnt work! */

    if (x == 0) {
        return 1;
    }
    x0 = fabsf(x);

    k = ceilf((x0 / LN2) - 0.5f);
    p = (float)(1 << (int)k);
    r = x0 - (LN2 * (float)k);  /* FIXME: internal error */
    tn = 1.0f;

    for (i = 14; i > 0; --i) {
        float tmp;
        tmp = (r / (float)i);
        tn = tn * tmp;
        tn = tn + 1.0f;
    }

    p *= tn;

    if (x < 0) {
        return 1.0f / p;
    }
    return p;
}
#endif
