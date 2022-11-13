
// test basic arithmetic operations
// WIP WIP WIP

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
unsigned char ch;

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
        printf("\n");
    } else {
        printf(" (failed) !!!\n");
        result++;
    }
}

void SKIPPEDtest1(float f, char *str)
{
    char temp[12];
    sprintf(temp, "%08lx", *((uint32_t*)&f));
    printf(" (SKIPPED:%s:%s)\n", temp, str);
}

void test(void)
{
#define XMIN 2.0f
#define XSTEP 0.2f

    ch = 3;

    printf("floatconst + intvar * floatconst\n");
    fp1 = XMIN + ch * XSTEP;
    printf("fp1:0x%08lx [0x402663e6] %s (2.5997)\n", *((uint32_t*)&fp1), _ftostr(buf, fp1));
    test1(fp1, "402663e6");

    ch = 4;
    fp1 = ch * 2.0f + 3.0f;
    fp2 = 3.0f + ch * 2.0f;
    printf(" fp1:0x%08lx [0x41300000] %s (exp:11)\n", *((uint32_t*)&fp1), _ftostr(buf, fp1));
    printf(" fp2:0x%08lx [0x41300000] %s (exp:11)\n", *((uint32_t*)&fp2), _ftostr(buf, fp2));
    test1(fp1, "41300000");
    test1(fp2, "41300000");

    printf("floatconst / intconst, intconst / floatconst\n");
    fp1 = ( (20.0f / 4.5f));
    fp2 = ( (4.5f / 20.0f));
    printf(" fp1:0x%08lx [0x408e38e4] %s (exp:4.444445)\n", *((uint32_t*)&fp1), _ftostr(buf, fp1));
    test1(fp1, "408e38e4");
    printf(" fp2:0x%08lx [0x3e666666] %s (exp:0.225000)\n", *((uint32_t*)&fp2), _ftostr(buf, fp2));
    test1(fp2, "3e666666");

    fp1 = ((20 / 4.5f));  // 4.44
    fp2 = ((4.5f / 20));  // 0.225
    printf(" fp1:0x%08lx [0x408e38e4] %s (exp:4.444445)\n", *((uint32_t*)&fp1), _ftostr(buf, fp1));
    test1(fp1, "408e38e4");
    printf(" fp2:0x%08lx [0x3e666666] %s (exp:0.225000)\n", *((uint32_t*)&fp2), _ftostr(buf, fp2));
    test1(fp2, "3e666666");

}

int main(void)
{
    float fp2 = 43.21f;

    printf("*** float-mixed ***\n");
    printf("fp1:0x%08lx [0x414570a4] %s (12.340000)\n", *((uint32_t*)&fp1), _ftostr(buf, fp1));
    printf("fp2:0x%08lx [0x422cd70a] %s (43.209999)\n", *((uint32_t*)&fp2), _ftostr(buf, fp2));

    test();
    WAIT();

    printf("\nfloat-mixed (res:%d)\n", result);
    return result;
}
