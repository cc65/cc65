
#include <stdlib.h>
#include <stdio.h>

char buffer[32];

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
    static char digits[10]={0,1,2,3,4,5,6,7,8,9};

    unsigned long mantissa_mod = 1000000000;
    unsigned long mantissa_rest;

    if (d == NULL) {
        buf = bp = &buffer[0];
    } else {
        buf = bp = d;
    }

    f = s;
    p = (unsigned char*)&f;

//    printf("%02x %02x %02x %02x\n", p[3], p[2], p[1], p[0]);

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
//    printf("mantissa: %ld\n", mantissa);
    mantissa_rest = mantissa;
    for (n = 0; n < 10; n++) {
//         printf("n:%2d rest:%ld mod:%ld\n", n, mantissa_rest, mantissa_mod);
         if ((mantissa_mod <= mantissa_rest) && (mantissa_rest > 0)) {
             val = mantissa_rest / mantissa_mod;
//             printf("n:%2d val:%ld\n", n, val);
//            *bp++ = digits[(int)val];
            *bp++ = '0' + val;
            mantissa_rest -= (val * mantissa_mod);
         }
         mantissa_mod /= 10;
     }
     if (val == 0xff) {
         *bp++ = '0';
     }

//    *bp++ = 'e';
//    *bp++ = 0;
    *bp++ = '*';
    *bp++ = '2';
    *bp++ = '^';

//    printf("exp: %ld\n", exp);
    mantissa_mod = 1000;
    if (exp < 0) {
        mantissa_rest = -1 * exp;
        *bp++ = '-';
    } else {
        mantissa_rest = exp;
    }
    val = 0xff;
    for (n = 0; n < 10; n++) {
//         printf("n:%2d rest:%ld mod:%ld\n", n, mantissa_rest, mantissa_mod);
         if ((mantissa_mod <= mantissa_rest) && (mantissa_rest > 0)) {
             val = mantissa_rest / mantissa_mod;
//             printf("n:%2d val:%ld\n", n, val);
//            *bp++ = digits[(int)val];
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
