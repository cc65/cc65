
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

float fp1 = 1.0f;
float fp2 = 200.0f; // aliased by variable in main
float fp3 = 3.0f;
float fp4 = 4.0f;

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
    float fp2 = 2.00f;

    printf("float negate (minus)\n");

    printf("fp1:0x%08lx [0x3f800000] %s (1.0)", *((uint32_t*)&fp1), _ftostr(buf, fp1));
    test1(fp1, "3f800000");
    printf("fp2:0x%08lx [0x40000000] %s (2.0)", *((uint32_t*)&fp2), _ftostr(buf, fp2));
    test1(fp2, "40000000");

    var_sint = -fp1;
    fp3 = -fp2;
    fp2 = -fp1;
    printf("fp2 0x%08lx [0xbf800000] %s (-1.0) %d", *((uint32_t*)&fp2), _ftostr(buf, fp2), var_sint);
    test1(fp2, "bf800000");
    printf("fp3 0x%08lx [0xc0000000] %s (-2.0)", *((uint32_t*)&fp3), _ftostr(buf, fp3));
    test1(fp3, "c0000000");

    fp1 = 12.6f;
    fp2 = -fp1;
    var_sint = -fp1;
    fp3 = -fp2;
    printf("fp2 0x%08lx [0xc149999a] %s (-12.6) %d", *((uint32_t*)&fp2), _ftostr(buf, fp2), var_sint);
    test1(fp2, "c149999a");
    printf("fp3 0x%08lx [0x4149999a] %s (12.6)", *((uint32_t*)&fp3), _ftostr(buf, fp3));
    test1(fp3, "4149999a");

    fp1 = -12.6f;
    fp2 = -fp1;
    var_sint = -fp1;
    fp3 = -fp2;
    printf("fp2 0x%08lx [0x4149999a] %s (12.6) %d", *((uint32_t*)&fp2), _ftostr(buf, fp2), var_sint);
    test1(fp2, "4149999a");
    printf("fp3 0x%08lx [0xc149999a] %s (-12.6)", *((uint32_t*)&fp3), _ftostr(buf, fp3));
    test1(fp3, "c149999a");

    printf("float minus (res:%d)\n", result);
    return result;
}
