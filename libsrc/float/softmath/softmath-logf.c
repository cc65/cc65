#include <math.h>

/* natural logarithm */

#if 1
/*#define LOGBASE     10*/
#define LOGBASE     2   /* e=2.7182818... */
float logf(float x)
{
    int i;
    float alpha;
    float save;
    float ans;

    alpha = (x-1)/(x+1);
    ans = alpha;
    save = ans * alpha * alpha;

    for (i = 2 ; i <= LOGBASE ; i++) {
        ans += (1.0/(float)(2*i-1)) * save;
        save = save * alpha * alpha;
    }

    return 2.0*ans;
}
#endif

#if 0
static int msb(int a)
{
    unsigned int r = 0;

    while(a >>= 1) {
        r++;
    }
    return r;
}

float logf(float y)
{
    float result;
    int log2;
    float divisor, x;

    log2 = msb((int)y);
    divisor = (float)(1 << log2);
    x = y / divisor;

    result = -1.7417939f + (2.8212026f + (-1.4699568f + (0.44717955f - 0.056570851f * x) * x) * x) * x;
    result += ((float)log2) * 0.69314718f; // ln(2) = 0.69314718

    return result;
}
#endif

#if 0
float logf(float x)
{

  /* ASSUMING:
     - non-denormalized numbers i.e. x > 2^−126
     - integer is 32 bit. float is IEEE 32 bit.

     INSPIRED BY:
     - https://stackoverflow.com/a/44232045
     - http://mathonweb.com/help_ebook/html/algorithms.htm#ln
     - https://en.wikipedia.org/wiki/Fast_inverse_square_root

     FORMULA:
     x = m * 2^p =>
       ln(x) = ln(m) + ln(2)p,

     first normalize the value to between 1.0 and 2.0
     assuming normalized IEEE float
        sign  exp       frac
     0b 0    [00000000] 00000000000000000000000
     value = (-1)^s * M * 2 ^ (exp-127)

     exp = 127 for x = 1,
     so 2^(exp-127) is the multiplier */

  /* evil floating point bit level hacking */
  unsigned long bx = * (unsigned long *) (&x);

  /* extract exp, since x>0, sign bit must be 0 */
  unsigned long ex = bx >> 23;
  signed long t = (signed long)ex-(signed long)127;
  unsigned long s = (t < 0) ? (-t) : t;

  /* reinterpret back to float
       127 << 23 = 1065353216
       0b11111111111111111111111 = 8388607 */
  bx = 1065353216 | (bx & 8388607);
  x = * (float *) (&bx);


  /* use remez algorithm to find approximation between [1,2]
     - see this answer https://stackoverflow.com/a/44232045
     - or this usage of C++/boost's remez implementation
       https://computingandrecording.wordpress.com/2017/04/24/
     e.g.
     boost::math::tools::remez_minimax<double> approx(
        [](const double& x) { return log(x); },
     4, 0, 1, 2, false, 0, 0, 64);

     4th order is:
     { -1.74178, 2.82117, -1.46994, 0.447178, -0.0565717 }

     3rd order is:
     { -1.49278, 2.11263, -0.729104, 0.10969 } */

  return;

  /* less accurate */
    -1.49278+(2.11263+(-0.729104+0.10969*x)*x)*x

  /* OR more accurate */
  /* -1.7417939+(2.8212026+(-1.4699568+(0.44717955-0.056570851*x)*x)*x)*x */

  /* compensate for the ln(2)s. ln(2)=0.6931471806 */
    + 0.6931471806*t;
}
#endif
