
#ifndef _FLOAT_H_
#define _FLOAT_H_

/* ieee754 32bit format:
 *
 *  sign
 * / /exponent/mantissa
 * 3 32222222 22211111111110000000000
 * 1 09876543 21098765432109876543210
 *
 * The sign is stored in bit 31.
 * The exponent can be computed from bits 23-30 by subtracting 127. (128 = 2^1)
 * The mantissa is stored in bits 0-22.
 *   An invisible leading bit (i.e. it is not actually stored) with value 1.0
 *   is placed in front, then bit 23 has a value of 1/2, bit 22 has value 1/4 etc.
 *   As a result, the mantissa has a value between 1.0 and 2.
 *
 * 1.0 = exp=127, mantissa=0
 *
 * If the exponent reaches -127 (binary 00000000), the leading 1 is no longer
 * used to enable gradual underflow.
 *
 */
typedef struct {
    unsigned char exponent;     /* msb is the sign */
    unsigned char mantissa[3];  /* msb is lsb of exponent */
} FLOAT754;

/* we dont wanna seriously use double precision eh? ;=P */
/* #define double float */

/* string to/from float conversion functions */
char *_ftoa(char *buf, float n);

/* string conversion functions, these use the exponential form */
char * __fastcall__ _ftostr(char *d, float s); /* for vsprintf */
float __fastcall__ _strtof(char *d);

/* beware, this is not standard */
#ifndef M_PI
#define M_PI    3.14159265358979323846f
#endif

/* degrees to radiants */
#define deg2rad(_fs, _n) ((_fs / _n) * (2.0f * M_PI))
/* radiants to degrees deg = (rad / (2 * pi)) * 256 */
#define rad2deg(_rad, _n)  ((_rad / (2.0f * M_PI)) * _n)

/* resets fp-libs in Turbo-C and M$-C */
#define _fpreset()

#endif /* _FLOAT_H_ */

