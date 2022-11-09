
#include <_float.h>
#include <math.h>

#include <cbmfp.h>

#define _fcmplt(_d, _s)  (_fcmp((_d), (_s)) == 1)
#define _fcmpgt(_d, _s)  (_fcmp((_d), (_s)) == 255)
#define _fcmpeq(_d, _s)  (_fcmp((_d), (_s)) == 0)

/* FIXME: this is really too simple */
float ffloor(float x)
{
    signed long n;
    float d;

    n = _ftoi(x);   /* FIXME: long */
    d = _itof(n);   /* FIXME: long */

    if (_fcmpeq(d, x) || _fcmplt(_itof(0), x)) {
        return d;
    }
    return _fsub(d, 1);
}
