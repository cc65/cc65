
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

#define expect(msg, exp, val, fval) \
    printf("%s %d:%d:%s%s (is:%d want:%d fval:%s)\n", \
        msg, val, exp, \
        val ? "true" : "false", \
        (exp != val) ? " (failed)" : "", \
        val, exp, _ftostr(buf, fval) \
        ); \
    if (exp != val) { \
        result++; \
    }

//-------------------------------------------------------------------------

void seperate(void)
{
    int res;
    fp1 = 10.0f;
    fp2 = 20.0f;
//    res = (20.0f == fp2);   // works
    res = (20 == fp2);   // fails
    printf("res:%d\n", res);
}

int i1 = 10;
int i2 = 20;

void intvarvar(void)
{
    printf("int const vs var\n");

    fp1 = 10.0f;
    fp2 = 20.0f;

#if 1
    expect("10 == 20 is", 0, (i1 == fp2), fp2);
    expect("20 == 10 is", 0, (i2 == fp1), fp1);
    expect("20 == 20 is", 1, (i2 == fp2), fp2);

    expect("10 != 20 is", 1, (i1 != fp2), fp2);
    expect("20 != 10 is", 1, (i2 != fp1), fp1);
    expect("20 != 20 is", 0, (i2 != fp2), fp2);

    expect("10 < 20 is", 1, (i1 < fp2), fp2);
    expect("20 < 10 is", 0, (i2 < fp1), fp1);
    expect("20 < 20 is", 0, (i2 < fp2), fp2);

    expect("10 > 20 is", 0, (i1 > fp2), fp2);
    expect("20 > 10 is", 1, (i2 > fp1), fp1);
    expect("20 > 20 is", 0, (i2 > fp2), fp2);

    expect("10 <= 20 is", 1, (i1 <= fp2), fp2);
    expect("20 <= 10 is", 0, (i2 <= fp1), fp1);
    expect("20 <= 20 is", 1, (i2 <= fp2), fp2);

    expect("10 >= 20 is", 0, (i1 >= fp2), fp2);
    expect("20 >= 10 is", 1, (i2 >= fp1), fp1);
    expect("20 >= 20 is", 1, (i2 >= fp2), fp2);
#endif

}

int main(void)
{
    printf("float-cmp-intvar-var\n");

    seperate();
    intvarvar();

    printf("float-cmp-intvar-var (res: %d)\n", result);
    return result;
}
