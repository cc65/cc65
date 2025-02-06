
#include <math.h>

/*  The input argument is in radians. Note that the function
    computes tan(pi*x/4), NOT tan(x); it's up to the range
    reduction algorithm that calls this to scale things properly.

    Algorithm:
          tan(x)= x(c1 + c2*x**2 + c3*x**4)/(c4 + c5*x**2 + c6*x**4 + x**6)

    Accurate to about 14 decimal digits over the range [0, pi/4]. */
static float _tan(float x)
{
    const float c1=-34287.4662577359568109624f;
    const float c2=  2566.7175462315050423295f;
    const float c3=-   26.5366371951731325438f;
    const float c4=-43656.1579281292375769579f;
    const float c5= 12244.4839556747426927793f;
    const float c6=-  336.611376245464339493f;

    float x2;

    x2 = x * x;
    return (x * (c1 + x2 * (c2 + x2 * c3)) / (c4 + x2 * (c5 + x2 * (c6 + x2))));
}

float tanf(float x){
    int octant;

    x = fmodf(x, (2.0f * M_PI));
    octant=(int)(x * (4.0f / M_PI));
    switch (octant){
        case 0: return         _tan(x                          * (4.0f / M_PI));
        case 1: return  1.0f / _tan(((M_PI / 2.0f) - x)        * (4.0f / M_PI));
        case 2: return -1.0f / _tan((x - (M_PI / 2.0f))        * (4.0f / M_PI));
        case 3: return -       _tan((M_PI - x)                 * (4.0f / M_PI));
        case 4: return         _tan((x - M_PI)                 * (4.0f / M_PI));
        case 5: return  1.0f / _tan(((3.0f * M_PI / 2.0f) - x) * (4.0f / M_PI));
        case 6: return -1.0f / _tan((x - (3.0f * M_PI / 2.0f)) * (4.0f / M_PI));
        case 7: return -       _tan(((2.0f * M_PI)-x)          * (4.0f / M_PI));
    }
}
