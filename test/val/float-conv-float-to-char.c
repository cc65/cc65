
// test basic type conversions

#define TEST_8
#define TEST_16
#define TEST_32

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

void test2(long n, long val)
{
    if (n == val) {
//        printf("(ok)");
        printf("\n");
    } else {
        printf(" (failed) !!!\n");
        result++;
    }
}

void varvar(void)
{

    printf("\nconversions (float variable to integer variable)\n");
    fp1 = -12.3f;
    fp2 = 19.9f;

    var_uchar = (unsigned char)fp2;
    printf("fp2 0x%08lx %s (19.9) uchar:%u (exp:19)", *((uint32_t*)&fp2), _ftostr(buf, fp2), (int)var_uchar);
    test2(var_uchar, 19);
}

int main(void)
{

    printf("float-conv-float-to-char\n");

    varvar();
    WAIT();

    printf("\nfloat-conv (res:%d)\n", result);
    return result;
}
