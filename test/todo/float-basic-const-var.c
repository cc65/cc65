
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
void constvar(void)
{
    printf("\nconstant vs variable\n\n");

    /* addition, constant + variable */
    fp2 = 43.21f;
    fp3 = 12.7f + fp2;  // FIXME: wrong, the add is dropped?
    printf("addition: %s+%s=%s\n", _ftostr(buf, 12.7f), _ftostr(buf2, fp2), _ftostr(buf3, fp3));
    printf(" fp3:0x%08lx [0x425fa3d7] %s", *((uint32_t*)&fp3), _ftostr(buf, fp3));
    test1(fp3, "425fa3d7");

    /* substraction, constant - variable */
    fp2 = 12.34;
    fp3 = 11.5f - fp2;  // FIXME: wrong, fp2 appears to become 0?
    printf("substraction: %s-%s=%s\n", _ftostr(buf, 11.5f), _ftostr(buf2, fp2), _ftostr(buf3, fp3));
    printf(" fp3:0x%08lx [0xbf570a40] %s ()", *((uint32_t*)&fp3), _ftostr(buf, fp3));
    test1(fp3, "bf570a40");

    fp2 = 2.3f;
    fp3 = 25.2f * fp2;  // FIXME: wrong, fp3 appears to become 0?
    printf("multiplication: %s*%s=%s\n", _ftostr(buf, 25.2f), _ftostr(buf2, fp2), _ftostr(buf3, fp3));
    printf(" fp3:0x%08lx [0x4267d70a] %s ()", *((uint32_t*)&fp3), _ftostr(buf, fp3));
    test1(fp3, "4267d70a");

    fp2 = 2.3f;
    fp3 = 25.2f / fp2;
    printf("division: %s/%s=%s\n", _ftostr(buf, 25.2f), _ftostr(buf2, fp2), _ftostr(buf3, fp3));
    printf(" fp3:0x%08lx [0x412f4dea] %s ()", *((uint32_t*)&fp3), _ftostr(buf, fp3));
    test1(fp3, "412f4dea");
}

int main(void)
{
    float fp2 = 43.21f;

    printf("float-basic-const-var\n");
    printf("fp1:0x%08lx [0x414570a4] %s (12.340000)\n", *((uint32_t*)&fp1), _ftostr(buf, fp1));
    printf("fp2:0x%08lx [0x422cd70a] %s (43.209999)\n", *((uint32_t*)&fp2), _ftostr(buf, fp2));

    constvar();
    WAIT();

    printf("\nfloat-basic-const-var (res:%d)\n", result);
    return result;
}
