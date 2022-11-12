
// test comparison operations
// WIP WIP WIP

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

// float constant vs float variable
// when making individual sub tests work, remove them here and uncomment them
// in val/float-cmp.c
void constvar(void)
{
    printf("const vs var\n");

    expect("1.5f == 1.6f is", 0, (1.5f == fp1));
    expect("1.6f == 1.5f is", 0, (1.6f == fp2));
    expect("1.6f == 1.6f is", 1, (1.6f == fp1));

    expect("1.5f != 1.6f is", 1, (1.5f != fp1));
    expect("1.6f != 1.5f is", 1, (1.6f != fp2));
    expect("1.6f != 1.6f is", 0, (1.6f != fp1));

    expect("1.5f < 1.6f is", 1, (1.5f < fp1));
    expect("1.6f < 1.5f is", 0, (1.6f < fp2));
    expect("1.6f < 1.6f is", 0, (1.6f < fp1));

    expect("1.5f > 1.6f is", 0, (1.5f > fp1));
    expect("1.6f > 1.5f is", 1, (1.6f > fp2));
    expect("1.6f > 1.6f is", 0, (1.6f > fp1));

    expect("1.5f <= 1.6f is", 1, (1.5f <= fp1));
    expect("1.6f <= 1.5f is", 0, (1.6f <= fp2));
    expect("1.6f <= 1.6f is", 1, (1.6f <= fp1));

    expect("1.5f >= 1.6f is", 0, (1.5f >= fp1));
    expect("1.6f >= 1.5f is", 1, (1.6f >= fp2));
    expect("1.6f >= 1.6f is", 1, (1.6f >= fp1));
}

//-------------------------------------------------------------------------

int main(void)
{
    printf("float-cmp-const-var\n");

    fp1 = 1.6f;
    fp2 = 1.5f;
    constvar();

    printf("float-cmp-const-var (res: %d)\n", result);
    return result;
}
