
#include <math.h>

#if 1

/* natural logarithm of 2 */
#define LN2 0.693147180559945309417f

// #include <stdio.h>
// char abuf[100];
// char abuf2[100];
// char abuf3[100];

float expf(float x)
{
    float p;
    int i;
    int k;
    // float i;
    // float k;

    float r;
    float tn;
    float x0 = fabsf(x);        // FIXME: somehow this doesnt work!

    if (x == 0) {
        return 1;
    }
    x0 = fabsf(x);
//    printf("x:%s x0:%s\n", _ftostr(abuf3, x), _ftostr(abuf, x0));

    k = ceilf((x0 / LN2) - 0.5f);
    p = (float)(1 << (int)k);
    r = x0 - (LN2 * (float)k); // internal error
    tn = 1.0f;
//    printf("k:%s tn:%s r:%s\n", _ftostr(abuf3, k), _ftostr(abuf, tn), _ftostr(abuf2, r  ));

    for (i = 14; i > 0; --i) {
        //tn = tn * (r / (float)i) + 1.0f;
        float tmp;
        tmp = (r / (float)i);
        tn = tn * tmp;
        tn = tn + 1.0f;
//        printf("i:%d tn:%s tmp:%s\n", i, _ftostr(abuf, tn), _ftostr(abuf2, tmp  ));
    }

    p *= tn;

    if (x < 0) {
        return 1.0f / p;
    }
    return p;
}
#endif

#if 0
static float expf(float n) {
    int a = 0, b = n > 0;
    float c = 1, d = 1, e = 1;
    for (b || (n = -n); e + .00001 < (e += (d *= n) / (c *= ++a));); // "Floating point type is currently unsupported"
//    for (b || (n = -n); e + .00001 < (e = e + (d *= n) / (c *= ++a));); // "Floating point type is currently unsupported"
    // approximately 15 iterations
    return b ? e : 1 / e;
}
#endif

#if 0
float expf(float x)
{
    x = x;
    return 0.0f;
}
#endif
