
#ifndef _CBMFS_H_
#define _CBMFS_H_


/*

  format used in basic-variables

  we dont use this format, although it saves one byte per variable, since that
  removes the need of constantly converting between both formats. (someday
  we may use an entire different (not cbm-specific), more accurate, format anyway)

              sign
    exponent / /mantissa
    33333333 3 3222222222111111111110000000000
    98765432 1 0987654321098765432109876543210

 * The exponent can be computed from bits 39-32 by subtracting 129 (!)
 */
typedef struct {
    unsigned char exponent;
    unsigned char mantissa[4];
} FLOATBAS;

/* CBM format used in floating-point akku
 *
 * this format can be directly used with most CBM BASIC routines
 *
 *  exponent mantissa                         sign
 *  44444444 33333333332222222222111111111100 00000000
 *  76543210 98765432109876543210987654321098 76543210
 *
 * truncated to 32bit:
 *
 *  exponent mantissa         sign
 *  33222222 2222111111111100 00000000
 *  10987654 3210987654321098 76543210
 *
 * The exponent can be computed from bits 47-40 by subtracting 129 (!) (130 = 2^1)
 * MSB of the Mantissa must always be 1, if it is 0 the value is 0
 * 
 * 1.0 = exp=129, mantissa=$80
 *
 */
typedef struct {
    unsigned char exponent;
    unsigned char mantissa[4];
    unsigned char sign;
} FLOATFAC;

/* integer conversion functions */
float __fastcall__ _ctof(char v);
float __fastcall__ _utof(unsigned char v);
float __fastcall__ _itof(int v);
float __fastcall__ _stof(unsigned short v);

int __fastcall__ _ftoi(float f);

/* compare two floats, returns 0 if f = a, 1 if f < a, 255 if f > a */
unsigned char __fastcall__ _fcmp(float f, float a);

/* arithmetic functions */
float __fastcall__ _fadd(float f, float a);     /* ftosaddeax */
float __fastcall__ _fsub(float f, float a);     /* ftossubeax */
float __fastcall__ _fmul(float f, float a);     /* ftosmuleax */
float __fastcall__ _fdiv(float f, float a);     /* ftosdiveax */

/* math functions */
float __fastcall__ _fpow(float f, float a);  /* math.h powf */
float __fastcall__ _fsin(float s);           /* math.h sinf */
float __fastcall__ _fcos(float s);           /* math.h cosf */

float __fastcall__ _flog(float s);           /* math.h logf */
float __fastcall__ _fexp(float s);           /* math.h expf */
float __fastcall__ _fsqr(float s);           /* math.h sqrtf */

float __fastcall__ _ftan(float s);           /* math.h tanf */
float __fastcall__ _fatn(float s);           /* math.h atanf */

float __fastcall__ _fabs(float s);           /* math.h fabsf */

/* logical functions */
float __fastcall__ _fand(float f, float a);
float __fastcall__ _for(float f, float a);
float __fastcall__ _fnot(float f);

/* misc */
float __fastcall__ _frnd(float s);

float __fastcall__ _fneg(float f);              /* negate (flip sign) */
unsigned char __fastcall__ _ftestsgn(float f); /* FIXME */

float __fastcall__ _fsgn(float s);
float __fastcall__ _fint(float s);

#if 0

typedef struct {
    unsigned char exponent;
    FLOATBAS coffs[8];       /* ? */
} FLOATPOLY;

#define fpoly FLOATPOLY

/* polynom1 f(x)=a1+a2*x^2+a3*x^3+...+an*x^n */
void _fpoly1(float *d,fpoly *a,float *x);
/* polynom2 f(x)=a1+a2*x^3+a3*x^5+...+an*x^(2n-1) */
void _fpoly2(float *d,fpoly *a,float *x);

#endif

/*
   todo:

    acos,asin,ceil,cosh,fmod,hypot,ldexp,log10,modf,poly,pow10,sinh
    tanh,cabs,_matherr,matherr,

*/

#endif
