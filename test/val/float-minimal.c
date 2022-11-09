
// test minimal float support.
// this should test everything that does NOT require linking a library
// - float literals in code are recognized as such
// - float variables are converted into the float binary format
// - taking address from float and assigning via pointer works

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

float fp1 = 42.01002f;
float fp2;  // non initialized
float *fp_p; // non initialized

uintptr_t p;

float fp3;
float fp4 = 23.12f;

char buf[0x20];

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
        printf("\n");
    } else {
        printf(" (failed)\n");
        result++;
    }
}

void references(void) {
    float fp2 = 23.1234f;

    printf("fp2:0x%08lx [exp:0x41b8fcb9] %s (23.1234)", *((uint32_t*)&fp2), _ftostr(buf, fp2));
    test1(fp2, "41b8fcb9");
    printf("fp4:0x%08lx [exp:0x41b8f5c3] %s (23.12)", *((uint32_t*)&fp4), _ftostr(buf, fp4));
    test1(fp4, "41b8f5c3");
#if 1
    printf("(global) get address, read via ptr\n");
    // get address of global (works)
    p = (uintptr_t)&fp1;
    fp_p = (float*)p;
    // read fp via pointer and assign local
    fp2 = *fp_p;

    printf("fp1:0x%08lx [exp:0x42280a43] %s (42.01002)", *((uint32_t*)&fp1), _ftostr(buf, fp1));
    test1(fp1, "42280a43");
    printf("fp2:0x%08lx [exp:0x42280a43] %s (42.01002)", *((uint32_t*)&fp2), _ftostr(buf, fp2));
    test1(fp2, "42280a43");
#endif
#if 1
    printf("(local) get address, read via ptr\n");
    fp2 = 23.1234f;

    // get address of local
    p = (uintptr_t)&fp2;
    fp_p = (float*)p;
    // read fp via pointer and assign global
    fp3 = *fp_p;

    printf("fp2:0x%08lx [exp:0x41b8fcb9] %s (23.1234)", *((uint32_t*)&fp2), _ftostr(buf, fp2));
    test1(fp2, "41b8fcb9");
    printf("fp3:0x%08lx [exp:0x41b8fcb9] %s (23.1234)", *((uint32_t*)&fp3), _ftostr(buf, fp3));
    test1(fp3, "41b8fcb9");
#endif
}

void testprinting(void)
{
    static float a,b;
    printf("\ntestprinting:\n\n");

    a = 1.0f;    printf("0x%08lx %s\n", *((uint32_t*)&a), _ftostr(buf, a) );
    b = 0.10f;
    a = b * a;       printf("0x%08lx %s\n", *((uint32_t*)&a), _ftostr(buf, a) );
    a = b * a;       printf("0x%08lx %s\n", *((uint32_t*)&a), _ftostr(buf, a) );
    a = b * a;       printf("0x%08lx %s\n", *((uint32_t*)&a), _ftostr(buf, a) );
    a = b * a;       printf("0x%08lx %s\n", *((uint32_t*)&a), _ftostr(buf, a) );
    a = b * a;       printf("0x%08lx %s\n", *((uint32_t*)&a), _ftostr(buf, a) );
    printf("\n");

    a = 0.00012f;    printf("0x%08lx %s\n", *((uint32_t*)&a), _ftostr(buf, a) );
    b = 10.0f;
    a = b * a;       printf("0x%08lx %s\n", *((uint32_t*)&a), _ftostr(buf, a) );
    a = b * a;       printf("0x%08lx %s\n", *((uint32_t*)&a), _ftostr(buf, a) );
    a = b * a;       printf("0x%08lx %s\n", *((uint32_t*)&a), _ftostr(buf, a) );
    a = b * a;       printf("0x%08lx %s\n", *((uint32_t*)&a), _ftostr(buf, a) );
    a = b * a;       printf("0x%08lx %s\n", *((uint32_t*)&a), _ftostr(buf, a) );
    printf("\n");

    a = 40000.0f;    printf("0x%08lx %s\n", *((uint32_t*)&a), _ftostr(buf, a) );
    b = 10.0f;
    a = b * a;       printf("0x%08lx %s\n", *((uint32_t*)&a), _ftostr(buf, a) );
    a = b * a;       printf("0x%08lx %s\n", *((uint32_t*)&a), _ftostr(buf, a) );
    a = b * a;       printf("0x%08lx %s\n", *((uint32_t*)&a), _ftostr(buf, a) );
    a = b * a;       printf("0x%08lx %s\n", *((uint32_t*)&a), _ftostr(buf, a) );
    a = b * a;       printf("0x%08lx %s\n", *((uint32_t*)&a), _ftostr(buf, a) );
    printf("\n");

    a = -40000.0f;    printf("0x%08lx %s\n", *((uint32_t*)&a), _ftostr(buf, a) );
    b = 10.0f;
    a = b * a;       printf("0x%08lx %s\n", *((uint32_t*)&a), _ftostr(buf, a) );
    a = b * a;       printf("0x%08lx %s\n", *((uint32_t*)&a), _ftostr(buf, a) );
    a = b * a;       printf("0x%08lx %s\n", *((uint32_t*)&a), _ftostr(buf, a) );
    a = b * a;       printf("0x%08lx %s\n", *((uint32_t*)&a), _ftostr(buf, a) );
    a = b * a;       printf("0x%08lx %s\n", *((uint32_t*)&a), _ftostr(buf, a) );
    printf("\n");
}

int main(void)
{
    printf("float-minimal\n");

    printf("sizeof (float): %d\n", (int)sizeof(float));

    references();
    WAIT();
    testprinting();
    WAIT();

    printf("float-minimal (res:%d)\n", result);
    return result;
}
