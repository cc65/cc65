
#ifdef __CC65__
#include <conio.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include <float.h>
#include <math.h>

#include <cbmfp.h>

char strbuf[0x20];
char strbuf2[0x20];
char strbuf3[0x20];



typedef union {
    float    f;
    unsigned char b[sizeof (float)];
} U;

#ifdef __CC65__
uint32_t ftobin(float f)
#else
unsigned long ftobin(float f)
#endif
{
    U u;
    u.f = f;
#ifdef __CC65__
    return ((uint32_t)u.b[0] << 0) | ((uint32_t)u.b[1] << 8) | ((uint32_t)u.b[2] << 16) | ((uint32_t)u.b[3] << 24);
#else
    return ((uint32_t)u.b[3] << 0) | ((uint32_t)u.b[2] << 8) | ((uint32_t)u.b[1] << 16) | ((uint32_t)u.b[0] << 24);
#endif
}

float fd, fs;
float a, b, c1;
int c, i, t;
int n;

void testconversions(void)
{
    printf("\ntestconversions:\n\n");
    printf("ftobin(0x12345678):0x%08lx\n", ftobin((float)0x12345678));

    /* 42 ieee754: 0x42280000 */
    b=_ctof(42);
    printf("b:0x%08lx  ", ftobin(b));
    n=(int)((float)0x42280000);
    printf("n:%d\n", n);

    b=_ctof(42);
    printf("b:0x%08lx  ", ftobin(b));
    n=(int)(b);
    printf("n:%d\n", n);

    b=_utof(42);
    printf("b:0x%08lx  ", ftobin(b));
    n=(int)(b);
    printf("n:%d\n", n);

    b=_stof(1234);
    printf("b:0x%08lx  ", ftobin(b));
    n=(int)(b);
    printf("n:%d\n", n);

    b=(float)(1234);
    printf("b:0x%08lx  ", ftobin(b));
    n=(int)(b);
    printf("n:%d\n", n);
    
    b=atof("1234");
    printf("b:0x%08lx  ", ftobin(b));
    n=(int)(b);
    printf("n:%d\n", n);

    _ftoa(strbuf, b);
    printf("s:%s\n", strbuf);

    printf("a:%s\n", _ftostr(strbuf, (float)(1234)));
    
    printf("a:%s b:%s c:%s\n",
           _ftostr(strbuf, (float)(1234)),
           _ftostr(strbuf2, (float)(5678)),
           _ftostr(strbuf3, (float)(9012))
          );
#ifdef __CC65__
    cgetc();
#endif
}

void testlogical(void)
{
    printf("\ntestlogical:\n\n");
    a = (float)(0xffa5);
    b = (float)(0x5aff);
    printf("a:0x%08lx (0xffa5)\n", ftobin(a));
    printf("b:0x%08lx (0x5aff)\n", ftobin(b));
    c1 = _fand(a, b);
    printf("c1:0x%08lx\n", ftobin(c1));
    n=(int)(c1);
    printf("c1:%04x\n", n);
#ifdef __CC65__
    cgetc();
#endif
}


#define _fcmplt(_d, _s)  (_fcmp((_d), (_s)) == 1)
#define _fcmpgt(_d, _s)  (_fcmp((_d), (_s)) == 255)
#define _fcmpeq(_d, _s)  (_fcmp((_d), (_s)) == 0)

void testcompare(void)
{
    printf("\ntestcompare:\n\n");
    a=(float)(2);
    b=(float)(3);
    printf("cmp 2,3: %3d %d %d %d\n", fcmp(a, b), fcmpgt(a, b), fcmpeq(a, b), fcmplt(a, b));
    printf("cmp 3,2: %3d %d %d %d\n", fcmp(b, a), fcmpgt(b, a), fcmpeq(b, a), fcmplt(b, a));
    printf("cmp 2,2: %3d %d %d %d\n", fcmp(a, a), fcmpgt(a, a), fcmpeq(a, a), fcmplt(a, a));
#ifdef __CC65__
    cgetc();
#endif
}
 
int main(void)
{
    printf("sizeof (float): %d\n", (int)sizeof(float));
    testconversions();
    testlogical();
    testcompare();
    return EXIT_SUCCESS;
}
