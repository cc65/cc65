
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
void varconst(void)
{
    printf("\nvariable vs constant\n\n");
    /* addition, variable + constant */
    fp1 = 55.549999f;
    fp3 = fp1 + 0.05f;
    printf("addition: %s+%s=%s\n", _ftostr(buf, fp3), _ftostr(buf3, 0.05f), _ftostr(buf2, fp1));
    printf(" fp3:0x%08lx [0x425e6666] %s", *((uint32_t*)&fp3), _ftostr(buf, fp3));
    test1(fp3, "425e6666");

#if 0
    /* substraction, variable - constant */
    fp1 = 12.34;
    fp3 = fp1 - 11.5f;  // FIXME: Invalid operands for binary operator '-'
    printf("substraction: %s-%s=%s\n", _ftostr(buf, fp1), _ftostr(buf2, 11.5f), _ftostr(buf3, fp3));
    printf(" fp3:0x%08lx [0x3f570a40] %s ()", *((uint32_t*)&fp3), _ftostr(buf, fp3));
    test1(fp3, "3f570a40");
#endif

    fp1 = 25.2f;
    fp3 = fp1 * 2.3f;
    printf("multiplication: %s*%s=%s\n", _ftostr(buf, fp1), _ftostr(buf2, 2.3f), _ftostr(buf3, fp3));
    printf(" fp3:0x%08lx [0x4267d70a] %s ()", *((uint32_t*)&fp3), _ftostr(buf, fp3));
    test1(fp3, "4267d70a");

#if 0
    fp1 = 25.2f;
    fp3 = fp1 / 2.3f;   // FIXME: division by zero
    printf("division: %s/%s=%s\n", _ftostr(buf, fp1), _ftostr(buf2, 2.3f), _ftostr(buf3, fp3));
    printf(" fp3:0x%08lx [0x412f4dea] %s ()", *((uint32_t*)&fp3), _ftostr(buf, fp3));
    test1(fp3, "412f4dea");
#endif
}

int main(void)
{
    float fp2 = 43.21f;

    printf("float-basic-var-const\n");
    printf("fp1:0x%08lx [0x414570a4] %s (12.340000)\n", *((uint32_t*)&fp1), _ftostr(buf, fp1));
    printf("fp2:0x%08lx [0x422cd70a] %s (43.209999)\n", *((uint32_t*)&fp2), _ftostr(buf, fp2));

    varconst();
    WAIT();

    printf("\nfloat-basic-var-const (res:%d)\n", result);
    return result;
}
