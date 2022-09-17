
// test basic type conversions
// WIP WIP WIP

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
        printf(" (failed) !!!\n");
        result++;
    }
}

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

void constvar(void)
{
    // conversions
    printf("\nconversions (integer constant to float variable)\n");
#ifdef TEST_8
    fp1 = -12;
    fp2 = 199;
    printf("fp1 0x%08lx [exp:0xc1400000] %s (-12)", *((uint32_t*)&fp1), _ftostr(buf, fp1));
    test1(fp1, "c1400000");
    printf("fp2 0x%08lx [exp:0x43470000] %s (199)", *((uint32_t*)&fp2), _ftostr(buf, fp2));
    test1(fp2, "43470000");
#endif
#ifdef TEST_16
    fp1 = -4711;
    fp2 = 42000U;
    printf("fp1 0x%08lx [exp:0xc5933800] %s (-4711)", *((uint32_t*)&fp1), _ftostr(buf, fp1));
    test1(fp1, "c5933800");
    printf("fp2 0x%08lx [exp:0x47241000] %s (42000)", *((uint32_t*)&fp2), _ftostr(buf, fp2));
    test1(fp2, "47241000");
#endif
#ifdef TEST_32
    fp1 = -321198;
    fp2 = 3200098;
    printf("fp1 0x%08lx [exp:0xc89cd5c0] %s (-321198)", *((uint32_t*)&fp1), _ftostr(buf, fp1));
    test1(fp1, "c89cd5c0");
    printf("fp2 0x%08lx [exp:0x4a435188] %s (3200098)", *((uint32_t*)&fp2), _ftostr(buf, fp2));
    test1(fp2, "4a435188");
#endif

    printf("\nconversions (float constant to integer variable)\n");
#ifdef TEST_8
    var_schar = (signed char)12.3f;
    printf("%s (12.3) schar:%d (exp:12)", _ftostr(buf, 12.3f), (int)var_schar);
    test2(var_schar, 12);
    var_uchar = (unsigned char)19.9f;
    printf("%s (19.9) uchar:%u (exp:19)", _ftostr(buf, 19.9f), (int)var_uchar);
    test2(var_uchar, 19);
#endif
#ifdef TEST_16
    var_sint = (signed short)1234.5f;
    printf("%s (1234.5) sint:%d (exp:1234)", _ftostr(buf, 1234.5f), var_sint);
    test2(var_sint, 1234);
    var_uint = (unsigned short)1999.9f;
    printf("%s (1999.9) uint:%u (exp:1999)", _ftostr(buf, 1999.9f), var_uint);
    test2(var_uint, 1999);
#endif
#ifdef TEST_32
    var_slong = (signed long)123456.5f;
    printf("%s (123456.5f) slong:%ld (exp:123456)", _ftostr(buf, 123456.5f), var_slong);
    test2(var_slong, 123456);
    var_ulong = (unsigned long)199988.9f;
    printf("%s (199988.9) ulong:%lu (exp:199988)", _ftostr(buf, 199988.9f), var_ulong);
    test2(var_ulong, 199988);
#endif
}

void varvar(void)
{
    float fp2 = 43.21f;
#if 1
    printf("\nconversions (integer variable to float variable)\n");
    var_schar = -12;
    fp1 = var_schar;
    var_uchar = 199;
    fp2 = var_uchar;
    printf("fp1 0x%08lx [exp:0xc1400000] %s (-12)", *((uint32_t*)&fp1), _ftostr(buf, fp1));
    test1(fp1, "c1400000");
    printf("fp2 0x%08lx [exp:0x43470000] %s (199)", *((uint32_t*)&fp2), _ftostr(buf, fp2));
    test1(fp2, "43470000");
    var_sint = -4711;
    fp1 = var_sint;
    var_uint = 42000U;
    fp2 = var_uint;
    printf("fp1 0x%08lx [exp:0xc5933800] %s (-4711)", *((uint32_t*)&fp1), _ftostr(buf, fp1));
    test1(fp1, "c5933800");
    printf("fp2 0x%08lx [exp:0x47241000] %s (42000)", *((uint32_t*)&fp2), _ftostr(buf, fp2));
    test1(fp2, "47241000");
    var_slong = -4711456;
    fp1 = var_slong;
    var_ulong = 42000456;
    fp2 = var_ulong;
    printf("fp1 0x%08lx [exp:0xca8fc840] %s (-4711456)", *((uint32_t*)&fp1), _ftostr(buf, fp1));
    test1(fp1, "ca8fc840");
    printf("fp2 0x%08lx [exp:0x4c203812] %s (42000456)", *((uint32_t*)&fp2), _ftostr(buf, fp2));
    test1(fp2, "4c203812");

    WAIT();
#endif

    printf("\nconversions (float variable to integer variable)\n");
    fp1 = -12.3f;
    var_schar = (signed char)fp1;
    printf("fp1 0x%08lx %s (-12.3) schar:%d (exp:-12)", *((uint32_t*)&fp1), _ftostr(buf, fp1), (int)var_schar);
    test2(var_schar, -12);
    fp2 = 19.9f;
    var_uchar = (unsigned char)fp2;
    printf("fp2 0x%08lx %s (19.9) uchar:%u (exp:19)", *((uint32_t*)&fp2), _ftostr(buf, fp2), (int)var_uchar);
    test2(var_uchar, 19);

    fp1 = 1234.5f;
    var_sint = (signed short)fp1;
    printf("fp1 0x%08lx %s (1234.5) sint:%d (exp:1234)", *((uint32_t*)&fp1), _ftostr(buf, fp1), var_sint);
    test2(var_sint, 1234);
    fp2 = 1999.9f;
    var_uint = (unsigned short)fp2;
    printf("fp2 0x%08lx %s (1999.9) uint:%u (exp:1999)", *((uint32_t*)&fp2), _ftostr(buf, fp2), var_uint);
    test2(var_uint, 1999);

    fp1 = 123456.5f;
    var_slong = (signed long)fp1;
    printf("fp1 0x%08lx %s (123456.5) slong:%ld (exp:123456)", *((uint32_t*)&fp1), _ftostr(buf, fp1), var_slong);
    test2(var_slong, 123456);
    fp2 = 199988.9f;
    var_ulong = (unsigned long)fp2;
    printf("fp2 0x%08lx %s (199988.9) ulong:%lu (exp:199988)", *((uint32_t*)&fp2), _ftostr(buf, fp2), var_ulong);
    test2(var_ulong, 199988);
}

int main(void)
{

    printf("float-conv\n");

    constvar();
    WAIT();
    varvar();
    WAIT();

    printf("\nfloat-conv (res:%d)\n", result);
    return result;
}
