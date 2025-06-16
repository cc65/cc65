
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

int i1 = 10;
int i2 = 20;

void intvarconst(void)
{
    printf("int var vs const\n");

#if 0 // internal compiler error
    expect("10 == 20.0f is", 0, (i1 == 20.0f));
    expect("20 == 10.0f is", 0, (i2 == 10.0f));
    expect("20 == 20.0f is", 1, (i2 == 20.0f));

    expect("10 != 20.0f is", 1, (i1 != 20.0f));
    expect("20 != 10.0f is", 1, (i2 != 10.0f));
    expect("20 != 20.0f is", 0, (i2 != 20.0f));

    expect("10 < 20.0f is", 1, (i1 < 20.0f));
    expect("20 < 10.0f is", 0, (i2 < 10.0f));
    expect("20 < 20.0f is", 0, (i2 < 20.0f));

    expect("10 > 20.0f is", 0, (i1 > 20.0f));
    expect("20 > 10.0f is", 1, (i2 > 10.0f));
    expect("20 > 20.0f is", 0, (i2 > 20.0f));

    expect("10 <= 20.0f is", 1, (i1 <= 20.0f));
    expect("20 <= 10.0f is", 0, (i2 <= 10.0f));
    expect("20 <= 20.0f is", 1, (i2 <= 20.0f));

    expect("10 >= 20.0f is", 0, (i1 >= 20.0f));
    expect("20 >= 10.0f is", 1, (i2 >= 10.0f));
    expect("20 >= 20.0f is", 1, (i2 >= 20.0f));
#endif
}

//-------------------------------------------------------------------------

int main(void)
{
    printf("float-cmp-intvar-const\n");

    intvarconst();

    printf("float-cmp-intvar-const (res: %d)\n", result);
    return result;
}
