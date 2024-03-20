
#include <math.h>

#if 1
/*  atan(x)= x(c1 + c2*x**2 + c3*x**4)/(c4 + c5*x**2 + c6*x**4 + x**6)

    Accurate to about 13.7 decimal digits over the range [0, pi/12]. */
float _atan(float x)
{
    const float c1= 48.70107004404898384f;
    const float c2= 49.5326263772254345f;
    const float c3=  9.40604244231624f;
    const float c4= 48.70107004404996166f;
    const float c5= 65.7663163908956299f;
    const float c6= 21.587934067020262f;

    float x2;
    x2 = x * x;
    return (x * (c1 + x2 * (c2 + x2 * c3)) / (c4 + x2 * (c5 + x2 * (c6 + x2))));
}

#define TAN_SIXTHPI      0.009138776996f
#define TAN_TWELFTHPI    0.004569293096f

float atanf(float x)
{
    float y;
    int complement= 0;  // true if arg was >1
    int region= 0;      // true depending on region arg is in
    int sign= 0;        // true if arg was < 0

    if (x < 0.0f ) {
//        x = -x;
//        x = x * -1.0f;
        x = -1.0f * x;
        sign = 1;       // arctan(-x)=-arctan(x)
    }
    if (x > 1.0f) {
        x = 1.0f / x;   // keep arg between 0 and 1
        complement = 1;
    }
    if (x > TAN_TWELFTHPI) {
        /* FIXME: reduce arg to under tan(pi/12) */
#if 1
        float n, m;
        n = (TAN_SIXTHPI * x) + 1.0f;
        m = (x - TAN_SIXTHPI);
//        x = (x - TAN_SIXTHPI) / (1.0f + (TAN_SIXTHPI * x));
        x = m / n;
#else
        x = fmodf(x, TAN_SIXTHPI);
#endif
        region = 1;
    }

    y = _atan(x);

    if (region) { y += (M_PI / 6.0f); }        /* correct for region we're in */
    if (complement) { y= (M_PI / 2.0f) - y; }  /* correct for 1/x */
    if (sign) { y =- y; }                      /* correct for negative arg */
    return y;
}
#endif

#if 0
float atanf(float x)
{
    x = x;
    return 0.0f;
}
#endif

