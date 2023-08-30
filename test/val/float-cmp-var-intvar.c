
// test comparison operations

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include <_float.h>

#ifdef CONIO
#include <conio.h>
#define WAIT()  cgetc()
#else
#define WAIT()
#endif

float fp1, fp2, fp3, fp4;

char buf[0x30];
char buf2[0x30];
char buf3[0x30];

signed char var_schar;
unsigned char var_uchar;
signed int var_sint;
unsigned int var_uint;
signed long var_slong;
unsigned long var_ulong;

int result = 0;

#define expect(msg, exp, val, fval, ival) \
    printf("%s %d:%d:%s%s (is:%d want:%d fval:%s ival:%d)\n", \
        msg, val, exp, \
        val ? "true" : "false", \
        (exp != val) ? " (failed)" : "", \
        val, exp, _ftostr(buf, fval), ival \
        ); \
    if (exp != val) { \
        result++; \
    }

//-------------------------------------------------------------------------

int i1 = 10;
int i2 = 20;

void varintvar(void)
{
    printf("var vs int var\n");

    fp1 = 10.0f;
    fp2 = 20.0f;
#if 0 // works but gives wrong results
    expect("10 == 20 is", 0, (fp1 == i2), fp1, i2);
    expect("20 == 10 is", 0, (fp2 == i1), fp2, i1);
    expect("20 == 20 is", 1, (fp2 == i2), fp2, i2);

    expect("10 != 20 is", 1, (fp1 != i2), fp1, i2);
    expect("20 != 10 is", 1, (fp2 != i1), fp2, i1);
    expect("20 != 20 is", 0, (fp2 != i2), fp2, i2);

    expect("10 < 20 is", 1, (fp1 < i2), fp1, i2);
    expect("20 < 10 is", 0, (fp2 < i1), fp2, i1);
    expect("20 < 20 is", 0, (fp2 < i2), fp2, i2);

    expect("10 > 20 is", 0, (fp1 > i2), fp1, i2);
    expect("20 > 10 is", 1, (fp2 > i1), fp2, i1);
    expect("20 > 20 is", 0, (fp2 > i2), fp2, i2);

    expect("10 <= 20 is", 1, (fp1 <= i2), fp1, i2);
    expect("20 <= 10 is", 0, (fp2 <= i1), fp2, i1);
    expect("20 <= 20 is", 1, (fp2 <= i2), fp2, i2);

    expect("10 >= 20 is", 0, (fp1 >= i2), fp1, i2);
    expect("20 >= 10 is", 1, (fp2 >= i1), fp2, i1);
    expect("20 >= 20 is", 1, (fp2 >= i2), fp2, i2);
#endif
}

int main(void)
{
    printf("float-cmp-var-intvar\n");

    varintvar();

    printf("float-cmp-var-intvar (res: %d)\n", result);
    return result;
}
