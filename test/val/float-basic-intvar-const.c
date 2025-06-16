
// test basic arithmetic operations

#ifdef CONIO
#include <conio.h>
#define WAIT()  cgetc()
#else
#define WAIT()
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <_float.h>

float fp1 = 12.34f;
float fp2;  // non initialized
float fp3, fp4 = 55.55f;

char buf[0x20];
char buf2[0x20];
char buf3[0x20];

unsigned long l1,l2;

signed char var_schar;
unsigned char var_uchar;
signed int var_sint;
unsigned int var_uint;
signed long var_slong;
unsigned long var_ulong;

int result = 0;
int i;

unsigned char var_char;
unsigned int var_int;
float var_float;

// returns 1 if value in f matches the string
// the string is a hex value without leading "0x"
int compare(float f, char *str)
{
    char temp[12];
    sprintf(temp, "%08lx", *((uint32_t*)&f));
    printf("[%s:%s]", temp, str);
    return (strcmp(temp, str) == 0) ? 1 : 0;
}

void test1(float f, char *str)
{
    if (compare(f, str)) {
//        printf(" (ok)");
    } else {
        printf(" (failed) !!!  ");
        result++;
    }
    printf("result:%d\n", result);
}

void test2(unsigned int i, unsigned int exp)
{
    if (i == exp) {
//        printf(" (ok)");
    } else {
        printf(" (failed) !!!  ");
        result++;
    }
    printf("result:%d\n", result);
}

void SKIPPEDtest1(float f, char *str)
{
    char temp[12];
    sprintf(temp, "%08lx", *((uint32_t*)&f));
    printf(" (SKIPPED:%s:%s)\n", temp, str);
}

void intvarconst(void)
{
    var_int = 47;
#if 1
    fp1 = var_int;
    printf("fp1:0x%08lx [42687df4] %s (47)\n", *((uint32_t*)&fp1), _ftostr(buf, fp1));
    fp1 = 11.123f;
    printf("fp1:0x%08lx [42687df4] %s (11.123)\n", *((uint32_t*)&fp1), _ftostr(buf, fp1));

    printf("int var + float const\n");

    /* addition */
    var_char = 42;
    fp1 = var_char + 23.123f;
    printf("fp1:0x%08lx [0x42823efa] %s (65.123)\n", *((uint32_t*)&fp1), _ftostr(buf, fp1));
    test1(fp1, "42823efa");

    fp1 = var_int + 11.123f;
    printf("fp1:0x%08lx [42687df4] %s (58.123)\n", *((uint32_t*)&fp1), _ftostr(buf, fp1));
    test1(fp1, "42687df4");
#endif
    /* subtraction */
#if 1 // gives wrong result
    fp1 = var_int - 11.123f;
    printf("fp1:0x%08lx [420f820c] %s (35.877)\n", *((uint32_t*)&fp1), _ftostr(buf, fp1));
    test1(fp1, "420f820c");
#endif
#if 1
    /* multiplication */
    fp1 = var_int * 11.123f;
    printf("fp1:0x%08lx [4402b1fc] %s (522.781)\n", *((uint32_t*)&fp1), _ftostr(buf, fp1));
    test1(fp1, "4402b1fc");

    /* division */
    fp1 = var_int / 11.123f;
    printf("fp1:0x%08lx [4087371f] %s (4.2254)\n", *((uint32_t*)&fp1), _ftostr(buf, fp1));
    test1(fp1, "4087371f");
#endif
}

unsigned int i1;

void intvarconst2(void)
{
    var_int = 47;
#if 1
    i1 = var_int;
    printf("i1: %d (47)\n", i1);
#endif

    printf("int var X= float const\n");

    /* addition */
#if 0 // gives wrong result
    i1 = var_int;
    i1 += 11.123f;
    printf("i1: %d (58)\n", i1);
    test2(i1, 58);
#endif
    /* subtraction */
#if 0 // gives wrong result
    i1 = var_int;
    i1 -= 11.123f;
    printf("i1: %d (35)\n", i1);
    test2(i1, 35);
#endif
#if 0 // internal compiler error
    /* multiplication */
    i1 = var_int;
    i1 *= 11.123f;
    printf("i1: %d (522)\n", i1);
    test2(i1, 522);
#endif
#if 0 // internal compiler error
    /* division */
    i1 = var_int;
    i1 /= 11.123f;
    printf("i1: %d (4)\n", i1);
    test2(i1, 4);
#endif
}

int main(void)
{
    float fp2 = 43.21f;

    printf("*** float-basic-intvar-const ***\n");
    printf("fp1:0x%08lx [0x414570a4] %s (12.340000)\n", *((uint32_t*)&fp1), _ftostr(buf, fp1));
    printf("fp2:0x%08lx [0x422cd70a] %s (43.209999)\n", *((uint32_t*)&fp2), _ftostr(buf, fp2));

    intvarconst();
    intvarconst2();
    WAIT();

    printf("\nfloat-basic-intvar-const (res:%d)\n", result);
    return result;
}
