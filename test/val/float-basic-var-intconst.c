
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

void SKIPPEDtest1(float f, char *str)
{
    char temp[12];
    sprintf(temp, "%08lx", *((uint32_t*)&f));
    printf(" (SKIPPED:%s:%s)\n", temp, str);
}

void test(void)
{

    var_float = 11.123f;

    fp1 = 47;
    printf("fp1:0x%08lx [42687df4] %s (47)\n", *((uint32_t*)&fp1), _ftostr(buf, fp1));
    fp1 = var_float;
    printf("fp1:0x%08lx [42687df4] %s (11.123)\n", *((uint32_t*)&fp1), _ftostr(buf, fp1));

    /* addition */
    fp1 = var_float + 47;
    printf("fp1:0x%08lx [42687df4] %s (58.123)\n", *((uint32_t*)&fp1), _ftostr(buf, fp1));
    test1(fp1, "42687df4");

#if 0 // works but gives wrong result
    fp1 = var_float - 47;
    printf("fp1:0x%08lx [42687df4] %s (-35.877)\n", *((uint32_t*)&fp1), _ftostr(buf, fp1));
    test1(fp1, "42687df4");
#endif

#if 0 // works but gives wrong result
    fp1 = var_float * 47;
    printf("fp1:0x%08lx [42687df4] %s (522.781)\n", *((uint32_t*)&fp1), _ftostr(buf, fp1));
    test1(fp1, "42687df4");
#endif
#if 0 // works but gives wrong result
    fp1 = var_float / 47;
    printf("fp1:0x%08lx [42687df4] %s (0.2367)\n", *((uint32_t*)&fp1), _ftostr(buf, fp1));
    test1(fp1, "42687df4");
#endif

}

int main(void)
{
    float fp2 = 43.21f;

    printf("*** float-basic-var-intconst ***\n");
    printf("fp1:0x%08lx [0x414570a4] %s (12.340000)\n", *((uint32_t*)&fp1), _ftostr(buf, fp1));
    printf("fp2:0x%08lx [0x422cd70a] %s (43.209999)\n", *((uint32_t*)&fp2), _ftostr(buf, fp2));

    test();
    WAIT();

    printf("\nfloat-basic-var-intconst (res:%d)\n", result);
    return result;
}
