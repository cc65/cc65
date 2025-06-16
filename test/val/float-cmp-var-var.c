
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

#define expect(msg, exp, val) \
    printf("%s %s%s\n", \
        msg, \
        val ? "true" : "false", \
        (exp != val) ? " (failed)" : ""); \
    if (exp != val) { \
        result++; \
    }

//-------------------------------------------------------------------------
// float variable vs float variable
void varvar(void)
{
    printf("var vs var\n");

    fp1 = 1.6f;
    fp2 = 1.5f;
    fp3 = 1.6f;
    fp4 = 1.5f;

    expect("1.5f == 1.6f is", 0, (fp2 == fp3));
    expect("1.6f == 1.5f is", 0, (fp1 == fp4));
    expect("1.6f == 1.6f is", 1, (fp1 == fp3));

    expect("1.5f != 1.6f is", 1, (fp2 != fp3));
    expect("1.6f != 1.5f is", 1, (fp1 != fp4));
    expect("1.6f != 1.6f is", 0, (fp1 != fp3));

    expect("1.5f < 1.6f is", 1, (fp2 < fp3));
    expect("1.6f < 1.5f is", 0, (fp1 < fp4));
    expect("1.6f < 1.6f is", 0, (fp1 < fp3));

    expect("1.5f > 1.6f is", 0, (fp2 > fp3));
    expect("1.6f > 1.5f is", 1, (fp1 > fp4));
    expect("1.6f > 1.6f is", 0, (fp1 > fp3));

    expect("1.5f <= 1.6f is", 1, (fp2 <= fp3));
    expect("1.6f <= 1.5f is", 0, (fp1 <= fp4));
    expect("1.6f <= 1.6f is", 1, (fp1 <= fp3));

    expect("1.5f >= 1.6f is", 0, (fp2 >= fp3));
    expect("1.6f >= 1.5f is", 1, (fp1 >= fp4));
    expect("1.6f >= 1.6f is", 1, (fp1 >= fp3));

}

int main(void)
{
    printf("float-cmp-var-var\n");

    fp1 = 1.6f;
    fp2 = 1.5f;

    varvar();

    printf("float-cmp-var-var (res: %d)\n", result);
    return result;
}
