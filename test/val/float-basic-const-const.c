
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

void constconst(void)
{
    printf("\n*** constant vs constant\n\n");

    // addition
    printf("\nconstant + constant\n\n");
    fp1 = 0.1f;
    fp2 = 0.2f;
    fp3 = 0.1f + 0.2f;

    printf("    0x%08lx [0x3dcccccd] %s (0.1)\n", *((uint32_t*)&fp1), _ftostr(buf, fp1));
    printf("    0x%08lx [0x3e4ccccd] %s (0.2)\n", *((uint32_t*)&fp2), _ftostr(buf, fp2));
    printf("fp3:0x%08lx [0x3e99999a] %s (0.3)", *((uint32_t*)&fp3), _ftostr(buf, fp3));
    test1(fp3, "3e99999a");

    // substraction
    printf("\nconstant - constant\n\n");
    fp1 = 0.1f;
    fp2 = 0.2f;
    fp3 = 0.1f - 0.2f;

    printf("    0x%08lx [0x3dcccccd] %s (0.1)\n", *((uint32_t*)&fp1), _ftostr(buf, fp1));
    printf("    0x%08lx [0x3e4ccccd] %s (0.2)\n", *((uint32_t*)&fp2), _ftostr(buf, fp2));
    printf("fp3:0x%08lx [0xbdcccccd] %s (-0.1)", *((uint32_t*)&fp3), _ftostr(buf, fp3));
    test1(fp3, "bdcccccd");

    fp3 = 0.3f - 0.1f;
    printf("fp3:0x%08lx [0x3e4ccccd] %s (0.2)", *((uint32_t*)&fp3), _ftostr(buf, fp3));
    test1(fp3, "3e4ccccd");
    fp3 = 0.1f - 0.3f;
    printf("fp3:0x%08lx [0xbe4ccccd] %s (-0.2)", *((uint32_t*)&fp3), _ftostr(buf, fp3));
    test1(fp3, "be4ccccd");

    // multiplication
    printf("\nconstant * constant\n\n");
    fp1 = 0.1f;
    fp2 = 0.2f;
    fp3 = 0.1f * 5.0f;

    printf("    0x%08lx [0x3dcccccd] %s (0.1)\n", *((uint32_t*)&fp1), _ftostr(buf, fp1));
    printf("    0x%08lx [0x3e4ccccd] %s (0.2)\n", *((uint32_t*)&fp2), _ftostr(buf, fp2));
    printf("fp3:0x%08lx [0x3f000000] %s (0.5)", *((uint32_t*)&fp3), _ftostr(buf, fp3));
    test1(fp3, "3f000000");

    // division
    printf("\nconstant / constant\n\n");
    fp1 = 0.1f;
    fp2 = 0.2f;
    fp3 = 0.1f / 0.2f;

    printf("    0x%08lx [0x3dcccccd] %s (0.1)\n", *((uint32_t*)&fp1), _ftostr(buf, fp1));
    printf("    0x%08lx [0x3e4ccccd] %s (0.2)\n", *((uint32_t*)&fp2), _ftostr(buf, fp2));
    printf("fp3:0x%08lx [0x3f000000] %s (0.5)", *((uint32_t*)&fp3), _ftostr(buf, fp3));
    test1(fp3, "3f000000");
}

int main(void)
{
    float fp2 = 43.21f;

    printf("*** float-basic-const-const ***\n");
    printf("fp1:0x%08lx [0x414570a4] %s (12.340000)\n", *((uint32_t*)&fp1), _ftostr(buf, fp1));
    printf("fp2:0x%08lx [0x422cd70a] %s (43.209999)\n", *((uint32_t*)&fp2), _ftostr(buf, fp2));

    constconst();
    WAIT();

    printf("\nfloat-basic-const-const (res:%d)\n", result);
    return result;
}
