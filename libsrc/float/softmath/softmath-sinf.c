
#include <math.h>

float sinf(float x) {
    return cosf(x - (M_PI / 2.0));
}
