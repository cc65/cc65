
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

// returns 1 if value in f matches the string
// the string is a hex value without leading "0x"
int compare(float f, char *str)
{
    char temp[12];
    sprintf(temp, "%08lx", *((uint32_t*)&f));
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

void constintvar(void)
{
    printf("\nfloat constant vs int variable\n\n");

    var_sint = 64;
    fp3 = 16.75f + var_sint;
    printf("addition: %s+%d=%s\n", _ftostr(buf, 16.75f), var_sint, _ftostr(buf3, fp3));
    printf(" fp3:0x%08lx [0x42a18000] %s (80.75)", *((uint32_t*)&fp3), _ftostr(buf, fp3));
    test1(fp3, "42a18000");

    var_sint = 16;
    fp3 =  18.5f - var_sint;
    printf("substraction: %s-%d=%s\n", _ftostr(buf, 8.5f), var_sint, _ftostr(buf3, fp3));
    printf(" fp3:0x%08lx [0x40200000] %s (2.5)", *((uint32_t*)&fp3), _ftostr(buf, fp3));
    test1(fp3, "40200000");
    var_sint = 13;
    fp3 = 20.6f - var_sint;
    printf("fp3:0x%08lx [0x40f33334] %s (7.6)", *((uint32_t*)&fp3), _ftostr(buf, fp3));
    test1(fp3, "40f33334");
    var_sint = 13;
    fp3 = 10.2f - var_sint;
    printf("fp3:0x%08lx [0xc0333334] %s (-2.8)", *((uint32_t*)&fp3), _ftostr(buf, fp3));
    test1(fp3, "c0333334");


    var_sint = 2;
    fp3 = 16.25f * var_sint;
    printf("multiplication: %s*%d=%s\n", _ftostr(buf, 16.25f), var_sint, _ftostr(buf3, fp3));
    printf(" fp3:0x%08lx [0x42020000] %s (32.5)", *((uint32_t*)&fp3), _ftostr(buf, fp3));
    test1(fp3, "42020000");


    var_sint = 2;
    fp3 = 16.2f / var_sint;
    printf("division: %s/%d=%s\n", _ftostr(buf, 16.2f), var_sint, _ftostr(buf3, fp3));
    printf(" fp3:0x%08lx [0x4101999a] %s (8.1)", *((uint32_t*)&fp3), _ftostr(buf, fp3));
    test1(fp3, "4101999a");
}

int main(void)
{
    float fp2 = 43.21f;

    printf("float-basic-const-intvar\n");
    printf("fp1:0x%08lx [0x414570a4] %s (12.340000)\n", *((uint32_t*)&fp1), _ftostr(buf, fp1));
    printf("fp2:0x%08lx [0x422cd70a] %s (43.209999)\n", *((uint32_t*)&fp2), _ftostr(buf, fp2));

    constintvar();
    WAIT();

    printf("\nfloat-basic-const-intvar (res:%d)\n", result);
    return result;
}
