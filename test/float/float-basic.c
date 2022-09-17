
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

void constconst(void)
{
    // addition
#if 1
    printf("\nconstant + constant\n\n");
    fp1 = 0.1f;
    fp2 = 0.2f;
    fp3 = 0.1f + 0.2f;

    printf("    0x%08lx [0x3dcccccd] %s (0.1)\n", *((uint32_t*)&fp1), _ftostr(buf, fp1));
    printf("    0x%08lx [0x3e4ccccd] %s (0.2)\n", *((uint32_t*)&fp2), _ftostr(buf, fp2));
    printf("fp3:0x%08lx [0x3e99999a] %s (0.3)", *((uint32_t*)&fp3), _ftostr(buf, fp3));
    test1(fp3, "3e99999a");
#endif
    // substraction
#if 0
    printf("\nconstant - constant\n\n");
    fp1 = 0.1f;
    fp2 = 0.2f;
    fp3 = 0.1f - 0.2f; //FIXME: Invalid operands for binary operator '-'

    printf("    0x%08lx [0x3dcccccd] %s (0.1)\n", *((uint32_t*)&fp1), _ftostr(buf, fp1));
    printf("    0x%08lx [0x3e4ccccd] %s (0.2)\n", *((uint32_t*)&fp2), _ftostr(buf, fp2));
    printf("fp3:0x%08lx [0xbdcccccd] %s (-0.1)", *((uint32_t*)&fp3), _ftostr(buf, fp3));
    test1(fp3, "bdcccccd");
#endif
    // multiplication
#if 1
    printf("\nconstant * constant\n\n");
    fp1 = 0.1f;
    fp2 = 0.2f;
    fp3 = 0.1f * 0.2f; // FIXME: Precondition violated: IsClassInt (T), file 'cc65/datatype.c', line 1008

    printf("    0x%08lx [0x3dcccccd] %s (0.1)\n", *((uint32_t*)&fp1), _ftostr(buf, fp1));
    printf("    0x%08lx [0x3e4ccccd] %s (0.2)\n", *((uint32_t*)&fp2), _ftostr(buf, fp2));
    printf("fp3:0x%08lx [0x3ca3d70b] %s (0.2)", *((uint32_t*)&fp3), _ftostr(buf, fp3));
    test1(fp3, "3ca3d70b");
#endif

    // division
#if 1
    printf("\nconstant / constant\n\n");
    fp1 = 0.1f;
    fp2 = 0.2f;
    fp3 = 0.1f / 0.2f; // FIXME: Precondition violated: IsClassInt (T), file 'cc65/datatype.c', line 1008

    printf("    0x%08lx [0x3dcccccd] %s (0.1)\n", *((uint32_t*)&fp1), _ftostr(buf, fp1));
    printf("    0x%08lx [0x3e4ccccd] %s (0.2)\n", *((uint32_t*)&fp2), _ftostr(buf, fp2));
    printf("fp3:0x%08lx [0x3f000000] %s (0.5)", *((uint32_t*)&fp3), _ftostr(buf, fp3));
    test1(fp3, "3f000000");
#endif
}

void varvar(void)
{
    printf("\nvariable vs variable\n\n");

    /* addition, variable + variable */
    fp1 = 12.34f;
    fp2 = 43.21f;
    fp3 = fp1 + fp2;
    printf("addition: %s+%s=%s\n", _ftostr(buf, fp1), _ftostr(buf2, fp2), _ftostr(buf3, fp3));
    printf(" fp3:0x%08lx [0x425e3333] %s (exp:55.549999)", *((uint32_t*)&fp3), _ftostr(buf, fp3));
    test1(fp3, "425e3333");
#if 1
    /* substraction, variable - variable */
    fp3 = fp1 - fp2;
    printf("substraction: %s-%s=%s\n", _ftostr(buf, fp1), _ftostr(buf2, fp2), _ftostr(buf3, fp3));
    printf(" fp3:0x%08lx [0xc1f6f5c2] %s (exp:-30.869999)", *((uint32_t*)&fp3), _ftostr(buf, fp3));
    test1(fp3, "c1f6f5c2");
#endif
#if 1
    fp1 = 25.2f;
    fp2 = 2.3f;
    fp3 = fp1 * fp2;
    printf("multiplication: %s*%s=%s\n", _ftostr(buf, fp1), _ftostr(buf2, fp2), _ftostr(buf3, fp3));
    printf(" fp3:0x%08lx [0x4267d70a] %s (exp:57.96)", *((uint32_t*)&fp3), _ftostr(buf, fp3));
    test1(fp3, "4267d70a");
#endif
#if 1
    fp1 = 25.2f;
    fp2 = 2.5f;
    fp3 = fp1 / fp2;
    printf("division: %s/%s=%s\n", _ftostr(buf, fp1), _ftostr(buf2, fp2), _ftostr(buf3, fp3));
    printf(" fp3:0x%08lx [0x412147ae] %s (exp:10.08)", *((uint32_t*)&fp3), _ftostr(buf, fp3));
    test1(fp3, "412147ae");
#endif
}

void varconst(void)
{
    printf("\nvariable vs constant\n\n");
    /* addition, variable + constant */
    fp1 = 55.549999f;
    fp3 = fp1 + 0.05f;
    printf("addition: %s+%s=%s\n", _ftostr(buf, fp3), _ftostr(buf3, 0.05f), _ftostr(buf2, fp1));
//    printf(" fp1:0x%08lx [0x425e3333] %s", *((uint32_t*)&fp1), _ftostr(buf, fp1));
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
#if 1
    fp1 = 25.2f;
    fp3 = fp1 * 2.3f;
    printf("multiplication: %s*%s=%s\n", _ftostr(buf, fp1), _ftostr(buf2, 2.3f), _ftostr(buf3, fp3));
    printf(" fp3:0x%08lx [0x4267d70a] %s ()", *((uint32_t*)&fp3), _ftostr(buf, fp3));
    test1(fp3, "4267d70a");
#endif
#if 0
    fp1 = 25.2f;
    fp3 = fp1 / 2.3f;   // FIXME: division by zero
    printf("division: %s/%s=%s\n", _ftostr(buf, fp1), _ftostr(buf2, 2.3f), _ftostr(buf3, fp3));
    printf(" fp3:0x%08lx [0x412f4dea] %s ()", *((uint32_t*)&fp3), _ftostr(buf, fp3));
    test1(fp3, "412f4dea");
#endif
}

void constvar(void)
{
    printf("\nconstant vs variable\n\n");
#if 1
    /* addition, constant + variable */
    fp2 = 43.21f;
    fp3 = 12.7f + fp2;  // FIXME: wrong, the add is dropped?
    printf("addition: %s+%s=%s\n", _ftostr(buf, 12.7f), _ftostr(buf2, fp2), _ftostr(buf3, fp3));
//    printf(" fp1:0x%08lx [0x41c9999a] %s\n", *((uint32_t*)&fp1), _ftostr(buf, fp1));
    printf(" fp3:0x%08lx [0x425fa3d7] %s", *((uint32_t*)&fp3), _ftostr(buf, fp3));
    test1(fp3, "425fa3d7");
#endif
#if 1
    /* substraction, constant - variable */
    fp2 = 12.34;
    fp3 = 11.5f - fp2;  // FIXME: wrong, fp2 appears to become 0?
    printf("substraction: %s-%s=%s\n", _ftostr(buf, 11.5f), _ftostr(buf2, fp2), _ftostr(buf3, fp3));
    printf(" fp3:0x%08lx [0xbf570a40] %s ()", *((uint32_t*)&fp3), _ftostr(buf, fp3));
    test1(fp3, "bf570a40");
#endif
#if 1
    fp2 = 2.3f;
    fp3 = 25.2f * fp2;  // FIXME: wrong, fp3 appears to become 0?
    printf("multiplication: %s*%s=%s\n", _ftostr(buf, 25.2f), _ftostr(buf2, fp2), _ftostr(buf3, fp3));
    printf(" fp3:0x%08lx [0x4267d70a] %s ()", *((uint32_t*)&fp3), _ftostr(buf, fp3));
    test1(fp3, "4267d70a");
#endif
#if 1
    fp2 = 2.3f;
    fp3 = 25.2f / fp2;
    printf("division: %s/%s=%s\n", _ftostr(buf, 25.2f), _ftostr(buf2, fp2), _ftostr(buf3, fp3));
    printf(" fp3:0x%08lx [0x412f4dea] %s ()", *((uint32_t*)&fp3), _ftostr(buf, fp3));
    test1(fp3, "412f4dea");
#endif
}

int main(void)
{
    float fp2 = 43.21f;

    printf("\nfloat-basic\n\n");
    printf("fp1:0x%08lx [0x414570a4] %s (12.340000)\n", *((uint32_t*)&fp1), _ftostr(buf, fp1));
    printf("fp2:0x%08lx [0x422cd70a] %s (43.209999)\n", *((uint32_t*)&fp2), _ftostr(buf, fp2));

    constconst();
    WAIT();
    varvar();
    WAIT();
    varconst();
    WAIT();
    constvar();
    WAIT();

    printf("\nfloat-basic (res:%d)\n", result);
    return result;
}
