
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

void SKIPPEDtest1(float f, char *str)
{
    char temp[12];
    sprintf(temp, "%08lx", *((uint32_t*)&f));
    printf(" (SKIPPED:%s:%s)\n", temp, str);
}

void constvar(void)
{
    printf("\n*** constant vs variable\n\n");

    fp2 = 64.25f;
    fp3 = 16.75f + fp2;
    printf("addition: %s+%s=%s\n", _ftostr(buf, 16.75f), _ftostr(buf2, fp2), _ftostr(buf3, fp3));
    printf(" fp3:0x%08lx [0x42a20000] %s (81.0)", *((uint32_t*)&fp3), _ftostr(buf, fp3));
    test1(fp3, "42a20000");

    fp2 = 16.25;
    fp3 =  8.5f - fp2;
    printf("substraction: %s-%s=%s\n", _ftostr(buf, 8.5f), _ftostr(buf2, fp2), _ftostr(buf3, fp3));
    printf(" fp3:0x%08lx [0xc0f80000] %s (-7.75)", *((uint32_t*)&fp3), _ftostr(buf, fp3));
SKIPPEDtest1(fp3, "c0f80000");

    fp2 = 2.25f;
    fp3 = 16.25f * fp2;
    printf("multiplication: %s*%s=%s\n", _ftostr(buf, 16.25f), _ftostr(buf2, fp2), _ftostr(buf3, fp3));
    printf(" fp3:0x%08lx [0x42124000] %s (36.5625)", *((uint32_t*)&fp3), _ftostr(buf, fp3));
    test1(fp3, "42124000");

    fp2 = 2.5f;
    fp3 = 16.2f / fp2;
    printf("division: %s/%s=%s\n", _ftostr(buf, 16.2f), _ftostr(buf2, fp2), _ftostr(buf3, fp3));
    printf(" fp3:0x%08lx [0x40cf5c2a] %s (6.48)", *((uint32_t*)&fp3), _ftostr(buf, fp3));
    test1(fp3, "40cf5c2a");
}

int main(void)
{
    float fp2 = 43.21f;

    printf("*** float-basic ***\n");
    printf("fp1:0x%08lx [0x414570a4] %s (12.340000)\n", *((uint32_t*)&fp1), _ftostr(buf, fp1));
    printf("fp2:0x%08lx [0x422cd70a] %s (43.209999)\n", *((uint32_t*)&fp2), _ftostr(buf, fp2));

    constvar();
    WAIT();

    printf("\nfloat-basic (res:%d)\n", result);
    return result;
}
