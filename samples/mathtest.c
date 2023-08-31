#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <conio.h>  // for cgetc

#ifdef __SIM6502__
#define cgetc()
#endif

char buf[100];

/*
float __fastcall__ sinf(float s);
float __fastcall__ cosf(float s);
float __fastcall__ tanf(float x);
float __fastcall__ atanf(float x);
*/

float atanvalues[8]= {
     0.50f,  // 0.46
    -1.50f,  // -0.98
};

void sincostanatan(void)
{
    int n;
    float angle = 0.0;
//    float angle = -M_PI;
    printf("sinus:\n");
    angle = 0.0;
    for (n = 0; n < 18; n++) {
        printf("%15s ", _ftostr(buf, angle));
        printf("%15s", _ftostr(buf, sinf(angle)));
        printf("\n");
//        angle += (M_PI / 8); Error: Invalid left operand for binary operator '+='
        angle = angle + ((M_PI * 2.0f) / 16);
    }
    printf("<key>\n"); cgetc();
    printf("cosinus:\n");
    angle = 0.0;
    for (n = 0; n < 18; n++) {
        printf("%15s ", _ftostr(buf, angle));
        printf("%15s", _ftostr(buf, cosf(angle)));
        printf("\n");
//        angle += (M_PI / 8); Error: Invalid left operand for binary operator '+='
        angle = angle + ((M_PI * 2.0f) / 16);
    }
    printf("<key>\n"); cgetc();
    printf("tangens:\n");
    angle = 0.0;
    for (n = 0; n < 18; n++) {
        printf("%15s ", _ftostr(buf, angle));
        printf("%15s", _ftostr(buf, tanf(angle)));
        printf("\n");
//        angle += (M_PI / 8); Error: Invalid left operand for binary operator '+='
        angle = angle + ((M_PI * 2.0f) / 16);
    }
    printf("<key>\n"); cgetc();
    printf("atan:\n");
    angle = 0.0;
    for (n = 0; n < 2; n++) {
        printf("%15s ", _ftostr(buf, atanvalues[n]));
        printf("%15s", _ftostr(buf, atanf(atanvalues[n])));
        printf("\n");
    }
    printf("<key>\n"); cgetc();
}

/*
float __fastcall__ roundf(float x);
float __fastcall__ truncf(float x);
float __fastcall__ fabsf(float x);
*/

float roundtruncvals[22] = {
    2.0f,
    1.9f,
    1.5f,
    1.4f,
    1.1f,
    1.0f,
    0.9f,
    0.5f,   //  1.0
    0.4f,
    0.1f,
    0.0f,
   -0.1f,
   -0.4f,
   -0.5f,   // -1.0
   -0.501f,   // -1.0
   -0.9f,
   -1.0f,
   -1.1f,
   -1.4f,
   -1.5f,
   -1.5001f,
   -1.9f,
//   -2.0f,
//   -2.1f,
};

void roundtruncabs(void)
{
    int n;
    printf("round/trunc:\n");
    for (n = 0; n < 21; n++) {
        printf("%12s", _ftostr(buf, roundtruncvals[n]));
        printf("%12s", _ftostr(buf, roundf(roundtruncvals[n])));
        printf("%12s", _ftostr(buf, truncf(roundtruncvals[n])));
        printf("\n");
    }
    printf("<key>\n"); cgetc();
    printf("abs/ceil:\n");
    for (n = 0; n < 21; n++) {
        printf("%12s", _ftostr(buf, roundtruncvals[n]));
        printf("%12s", _ftostr(buf, fabsf(roundtruncvals[n])));
        printf("%12s", _ftostr(buf, ceilf(roundtruncvals[n])));
        printf("\n");
    }
    printf("<key>\n"); cgetc();
}

/*
float __fastcall__ powf(float f, float a);
float __fastcall__ sqrtf(float x);
*/
float powvals0[8] = {
    2,
    10,
    2,
    10
};

float powvals1[8] = {
    3,
    3,
    4,
    4
};

float sqrtvals[8] = {
    9,
    2,
    1,
    16,
    0,
    4,
    5,
    6
};

void powersqrt(void)
{
    int n;
    printf("pow:\n");
    for (n = 0; n < 4; n++) {
        printf("%12s", _ftostr(buf, powvals0[n]));
        printf("%12s", _ftostr(buf, powvals1[n]));
        printf("%12s", _ftostr(buf, powf(powvals0[n], powvals1[n])));
        printf("\n");
    }
    printf("sqrt:\n");
    for (n = 0; n < 8; n++) {
        printf("%12s", _ftostr(buf, sqrtvals[n]));
        printf("%12s", _ftostr(buf, sqrtf(sqrtvals[n])));
        printf("\n");
    }
    printf("<key>\n"); cgetc();
}

/*
float __fastcall__ logf(float x);
float __fastcall__ expf(float x);
*/
float logvals[8] = {
    1.0f, // 0
    5.6f, // 1.72
    10.0f, // 2.302585
};

float expvals[8] = {
    12.0f, // 162754.79
    5.0f,  // 148.413159
    4.0f,  // 54.598150
    3.0f,  // 20.085537
};

void logexp(void)
{
    int n;
    printf("log:\n");
    for (n = 0; n < 3; n++) {
        printf("%12s", _ftostr(buf, logvals[n]));
        printf("%12s", _ftostr(buf, logf(logvals[n])));
        printf("\n");
    }
    printf("exp:\n");
    for (n = 0; n < 4; n++) {
        printf("%12s", _ftostr(buf, expvals[n]));
        printf("%12s", _ftostr(buf, expf(expvals[n])));
        printf("\n");
    }
    printf("<key>\n"); cgetc();
}

int main(void)
{
    roundtruncabs();
    sincostanatan();
    logexp();
    powersqrt();
}
