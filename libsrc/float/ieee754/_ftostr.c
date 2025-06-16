
#include <stdlib.h>
#include <stdio.h>

char buffer[32];

/* FIXME: this is a really hacky implementation meant to be used while developing
 * the floating point support (it does not use floating point operations). Once
 * the support is more stable, it should be rewritten */
char * __fastcall__ _ftostr(char *d, float s)
{
    float f;
    unsigned char *p;
    char *bp, *buf;
    int exp;
    unsigned long mantissa;
    unsigned long val;
    int sign;
    unsigned char n;

    unsigned long mantissa_mod = 1000000000;
    unsigned long mantissa_rest;

    if (d == NULL) {
        buf = bp = &buffer[0];
    } else {
        buf = bp = d;
    }

    f = s;
    p = (unsigned char*)&f;

    sign = (p[3] & 0x80) ? 1 : 0;
    exp = ((p[3] << 1) & 0xfe) | ((p[2] >> 7) & 1);
    exp -= 127;
    mantissa = p[2] & 0x7f;
    mantissa <<=8;
    mantissa |= p[1];
    mantissa <<=8;
    mantissa |= p[0];

    *bp++ = sign ? '-' : ' ';

    *bp++ = '1';
    *bp++ = '.';

    val = 0xff;

    mantissa_rest = mantissa;
    for (n = 0; n < 10; n++) {
         if ((mantissa_mod <= mantissa_rest) && (mantissa_rest > 0)) {
             val = mantissa_rest / mantissa_mod;
            *bp++ = '0' + val;
            mantissa_rest -= (val * mantissa_mod);
         }
         mantissa_mod /= 10;
     }
     if (val == 0xff) {
         *bp++ = '0';
     }

    *bp++ = '*';
    *bp++ = '2';
    *bp++ = '^';

    mantissa_mod = 1000;
    if (exp < 0) {
        mantissa_rest = -1 * exp;
        *bp++ = '-';
    } else {
        mantissa_rest = exp;
    }
    val = 0xff;
    for (n = 0; n < 10; n++) {
         if ((mantissa_mod <= mantissa_rest) && (mantissa_rest > 0)) {
             val = mantissa_rest / mantissa_mod;
            *bp++ = '0' + val;
            mantissa_rest -= (val * mantissa_mod);
         }
         mantissa_mod /= 10;
     }
     if (val == 0xff) {
         *bp++ = '0';
     }
    *bp++ = 0;

    return buf;
}
