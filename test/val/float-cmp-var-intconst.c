
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
    printf("%s %d:%d:%s%s (is:%d want:%d)\n", \
        msg, exp, val, \
        val ? "true" : "false", \
        (exp != val) ? " (failed)" : "", \
        exp, val \
        ); \
    if (exp != val) { \
        result++; \
    }

//-------------------------------------------------------------------------

void varintconst(void)
{
    printf("var vs int const\n");

    fp1 = 10.0f;
    fp2 = 20.0f;

    expect("10 == 20 is", 0, (fp1 == 20));
    expect("20 == 10 is", 0, (fp2 == 10));
    expect("20 == 20 is", 1, (fp2 == 20));

    expect("10 != 20 is", 1, (fp1 != 20));
    expect("20 != 10 is", 1, (fp2 != 10));
    expect("20 != 20 is", 0, (fp2 != 20));

    expect("10 < 20 is", 1, (fp1 < 20));
    expect("20 < 10 is", 0, (fp2 < 10));
    expect("20 < 20 is", 0, (fp2 < 20));

    expect("10 > 20 is", 0, (fp1 > 20));
    expect("20 > 10 is", 1, (fp2 > 10));
    expect("20 > 20 is", 0, (fp2 > 20));

    expect("10 <= 20 is", 1, (fp1 <= 20));
    expect("20 <= 10 is", 0, (fp2 <= 10));
    expect("20 <= 20 is", 1, (fp2 <= 20));

    expect("10 >= 20 is", 0, (fp1 >= 20));
    expect("20 >= 10 is", 1, (fp2 >= 10));
    expect("20 >= 20 is", 1, (fp2 >= 20));

}

int main(void)
{
    printf("float-cmp-var-intconst\n");

    varintconst();

    printf("float-cmp-var-intconst (res: %d)\n", result);
    return result;
}
