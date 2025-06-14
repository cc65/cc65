
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

void constconst(void)
{
    printf("const vs const\n");

    expect("1.5f == 1.6f is", 0, (1.5f == 1.6f));
    expect("1.6f == 1.5f is", 0, (1.6f == 1.5f));
    expect("1.6f == 1.6f is", 1, (1.6f == 1.6f));

    expect("1.5f != 1.6f is", 1, (1.5f != 1.6f));
    expect("1.6f != 1.5f is", 1, (1.6f != 1.5f));
    expect("1.6f != 1.6f is", 0, (1.6f != 1.6f));

    expect("1.5f < 1.6f is", 1, (1.5f < 1.6f));
    expect("1.6f < 1.5f is", 0, (1.6f < 1.5f));
    expect("1.6f < 1.6f is", 0, (1.6f < 1.6f));

    expect("1.5f > 1.6f is", 0, (1.5f > 1.6f));
    expect("1.6f > 1.5f is", 1, (1.6f > 1.5f));
    expect("1.6f > 1.6f is", 0, (1.6f > 1.6f));

    expect("1.5f <= 1.6f is", 1, (1.5f <= 1.6f));
    expect("1.6f <= 1.5f is", 0, (1.6f <= 1.5f));
    expect("1.6f <= 1.6f is", 1, (1.6f <= 1.6f));

    expect("1.5f >= 1.6f is", 0, (1.5f >= 1.6f));
    expect("1.6f >= 1.5f is", 1, (1.6f >= 1.5f));
    expect("1.6f >= 1.6f is", 1, (1.6f >= 1.6f));
}

//-------------------------------------------------------------------------

int main(void)
{
    printf("float-cmp-const-const\n");

    constconst();

    printf("float-cmp-const-const (res: %d)\n", result);
    return result;
}
