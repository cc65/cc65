
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

// when making sub tests work, remove them here and uncomment them in val/float-basic.c
void varvar(void)
{
    printf("\nvariable vs variable\n\n");

    /* addition, variable + variable */
    fp1 = 16.5f;
    fp2 = 64.25f;
    fp3 = fp1 + fp2;   // = 80.75f
    printf("addition: %s+%s=%s\n", _ftostr(buf, fp1), _ftostr(buf2, fp2), _ftostr(buf3, fp3));
    printf(" fp3:0x%08lx [0x42a18000] %s (exp:80.75)", *((uint32_t*)&fp3), _ftostr(buf, fp3));
    test1(fp3, "42a18000");

    /* substraction, variable - variable */
    fp1 = 64.25f;
    fp2 = 16.5f;
    fp3 = fp1 - fp2;
    printf("substraction: %s-%s=%s\n", _ftostr(buf, fp1), _ftostr(buf2, fp2), _ftostr(buf3, fp3));
    printf(" fp3:0x%08lx [0x423f0000] %s (exp:47.75)", *((uint32_t*)&fp3), _ftostr(buf, fp3));
    test1(fp3, "423f0000");

    fp1 = 0.1f;
    fp2 = 0.3f;

    fp3 = fp2 - fp1;
    printf("fp3:0x%08lx [0x3e4cccce] %s (0.2)", *((uint32_t*)&fp3), _ftostr(buf, fp3));
    test1(fp3, "3e4cccce");
    fp3 = fp1 - fp2;
    printf("fp3:0x%08lx [0xbe4cccce] %s (-0.2)", *((uint32_t*)&fp3), _ftostr(buf, fp3));
    test1(fp3, "be4cccce");

    fp1 = 8.5f;
    fp2 = 2.25f;
    fp3 = fp1 * fp2;
    printf("multiplication: %s*%s=%s\n", _ftostr(buf, fp1), _ftostr(buf2, fp2), _ftostr(buf3, fp3));
    printf(" fp3:0x%08lx [0x41990000] %s (exp:19.125)", *((uint32_t*)&fp3), _ftostr(buf, fp3));
    test1(fp3, "41990000");

    fp1 = 16.25f;
    fp2 = 2.5f;
    fp3 = fp1 / fp2;
    printf("division: %s/%s=%s\n", _ftostr(buf, fp1), _ftostr(buf2, fp2), _ftostr(buf3, fp3));
    printf(" fp3:0x%08lx [0x40d00000] %s (exp:6.5)", *((uint32_t*)&fp3), _ftostr(buf, fp3));
    test1(fp3, "40d00000");
}


int main(void)
{
    float fp2 = 43.21f;

    printf("float-basic-var-var\n");
    printf("fp1:0x%08lx [0x414570a4] %s (12.340000)\n", *((uint32_t*)&fp1), _ftostr(buf, fp1));
    printf("fp2:0x%08lx [0x422cd70a] %s (43.209999)\n", *((uint32_t*)&fp2), _ftostr(buf, fp2));

    varvar();
    WAIT();

    printf("\nfloat-basic-var-var (res:%d)\n", result);
    return result;
}
