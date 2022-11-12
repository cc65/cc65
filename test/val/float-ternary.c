
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
//        printf("(ok)");
        printf("\n");
    } else {
        printf(" (failed)\n");
        result++;
    }
}

void test2(long n, long val)
{
    if (n == val) {
//        printf("(ok)");
        printf("\n");
    } else {
        printf(" (failed)\n");
        result++;
    }
}

int main(void)
{
    float fp2 = 43.21f;

    printf("float-ternary\n");
    printf("fp1:0x%08lx [0x414570a4] %s (12.340000)\n", *((uint32_t*)&fp1), _ftostr(buf, fp1));
    printf("fp2:0x%08lx [0x422cd70a] %s (43.209999)\n", *((uint32_t*)&fp2), _ftostr(buf, fp2));

    fp1 = (fp2 == 2.5f) ? 1.5f : 0.5f;

    printf("fp1 0x%08lx [0x3f000000] %s (0.5)", *((uint32_t*)&fp1), _ftostr(buf, fp1));
    test1(fp1, "3f000000");

    fp2 = 2.5f;

    fp1 = (fp2 == 2.5f) ? 1.5f : 0.5f;

    printf("fp1 0x%08lx [0x3fc00000] %s (1.5)", *((uint32_t*)&fp1), _ftostr(buf, fp1));
    test1(fp1, "3fc00000");

    printf("float-ternary (res:%d)\n", result);
    return result;
}
