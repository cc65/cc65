
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


void intconstconst(void)
{
    printf("int const vs const\n");

    expect("10 == 20.0f is", 0, (10 == 20.0f));
    expect("20 == 10.0f is", 0, (20 == 10.0f));
    expect("20 == 20.0f is", 1, (20 == 20.0f));

    expect("10 != 20.0f is", 1, (10 != 20.0f));
    expect("20 != 10.0f is", 1, (20 != 10.0f));
    expect("20 != 20.0f is", 0, (20 != 20.0f));

    expect("10 < 20.0f is", 1, (10 < 20.0f));
    expect("20 < 10.0f is", 0, (20 < 10.0f));
    expect("20 < 20.0f is", 0, (20 < 20.0f));

    expect("10 > 20.0f is", 0, (10 > 20.0f));
    expect("20 > 10.0f is", 1, (20 > 10.0f));
    expect("20 > 20.0f is", 0, (20 > 20.0f));

    expect("10 <= 20.0f is", 1, (10 <= 20.0f));
    expect("20 <= 10.0f is", 0, (20 <= 10.0f));
    expect("20 <= 20.0f is", 1, (20 <= 20.0f));

    expect("10 >= 20.0f is", 0, (10 >= 20.0f));
    expect("20 >= 10.0f is", 1, (20 >= 10.0f));
    expect("20 >= 20.0f is", 1, (20 >= 20.0f));
}

//-------------------------------------------------------------------------

int main(void)
{
    printf("float-cmp-intconst-const\n");

    intconstconst();

    printf("float-cmp-intconst-const (res: %d)\n", result);
    return result;
}
