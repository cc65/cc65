
#ifndef _MATH_H_
#define _MATH_H_

#include "_float.h"

/* double pow(double x, double y); */
float __fastcall__ powf(float f, float a);      /* C99 */

/* double sin(double x); */
float __fastcall__ sinf(float s);               /* C99 */

/* double cos(double x); */
float __fastcall__ cosf(float s);               /* C99 */

/* double log(double x); */
float __fastcall__ logf(float x);               /* C99 */

/* double exp(double x); */
float __fastcall__ expf(float x);               /* C99 */

/* double sqrt(double x); */
float __fastcall__ sqrtf(float x);              /* C99 */

/* double tan(double x); */
float __fastcall__ tanf(float x);               /* C99 */

/* double atan(double x); */
float __fastcall__ atanf(float x);              /* C99 */

/* double fabs(double x); */
float __fastcall__ fabsf(float x);              /* C99 */

/* double round(double x); */              /* C99 */
float __fastcall__ roundf(float x);              /* C99 */

/* double trunc(double x); */              /* C99 */
float __fastcall__ truncf(float x);              /* C99 */

/* beware, this is not standard */
#ifndef M_PI
#define M_PI    3.14159265358979323846f
#endif

/* FIXME */
float __fastcall__ _fatan2(float x, float y);


float ffloor(float x);

#endif /* _MATH_H_ */
 
