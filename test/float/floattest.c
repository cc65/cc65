
#ifdef __CC65__
#include <conio.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include <_float.h>
#include <math.h>

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

void csetpoint8(unsigned char x)
{
    while(x--) {
        printf(" ");
    }
    printf("*\n");
}

#define YNUM    32
#define YNUM2   16

#define XNUM    32
#define XNUM2   16

void calcsin(void)
{
    printf("sinus:\n");
    for(i = 0; i < YNUM; i++) {
        csetpoint8(XNUM2+((int)(sinf(deg2rad((float)(i),(float)(YNUM))) * (float)(YNUM2))/(YNUM/XNUM)));
    }
}

void calccos(void)
{
    printf("cosinus:\n");
    for(i = 0; i < YNUM; i++) {
        fs=deg2rad((float)(i),(float)(YNUM));
        fd=cosf(fs) * (float)(YNUM2);
        csetpoint8(XNUM2+((int)(fd)/(YNUM/XNUM)));
    }
}

#ifdef __CC65__

int fx, fy;

#define CYNUM    20
#define CYNUM2   10

#define CXNUM    20
#define CXNUM2   10

void cplot(unsigned char x, unsigned char y)
{
    gotoxy(x,y);cputc('*');
}

void testatan2(void)
{
    for(i = 0; i < YNUM; i++) {
        fx = CXNUM2+((int)((sinf(deg2rad((float)(i),(float)(CYNUM)))*(float)(CYNUM2)))/(CYNUM/CXNUM));

        fs = deg2rad((float)(i), (float)(CYNUM));
        fd = cosf(fs) * (float)(CYNUM2);
        fy = CXNUM2 + ((int)(fd) / (CYNUM/CXNUM));

        cplot(fx, fy);
        fd = fatan2((float)(fx-CXNUM2), (float)(fy-CYNUM2));
        _ftostr(strbuf, fd);
        cputs(strbuf);
   }
}
#endif

#if 0
void calcpoly1(void){

    p=malloc(sizeof(fpoly)+((1+3)*sizeof(float)));
    p.exponent=3;

//    _strtof(&(p->coffs[3]),"0.25");
//    _strtof(&(p->coffs[2]),"1");
//    _strtof(&(p->coffs[1]),"-0.5");
//    _strtof(&(p->coffs[0]),"0.25");

//    _ftostr(&strbuf,&(p->coffs[3])); printf("a0:%s\n",strbuf);
//    _ftostr(&strbuf,&(p->coffs[2])); printf("a1:%s\n",strbuf);
//    _ftostr(&strbuf,&(p->coffs[1])); printf("a2:%s\n",strbuf);
//    _ftostr(&strbuf,&(p->coffs[0])); printf("a3:%s\n",strbuf);

//    _fpoly1(&fd,&p,&_f_pi);
//    _ftostr(&strbuf,&fd); printf("x:%s\n",strbuf);

      for(i=0;i<256;i++){
        fs=_(float)(i);
        _ftostr(&strbuf,fs); printf("%s:",strbuf);
        fd=_fpoly1(&p,&fs);
        _ftostr(&strbuf,fd); printf("%s\n",strbuf);
//        c=_(int)(&fd);
//        csetpoint8(i/(256/35),12+(c/(256/23)));
      }

    free(p);
}
#endif

unsigned short var_bs;
unsigned short var_fs;
float var_i;
unsigned char var_w;
unsigned char var_j;
float var_k;
unsigned short var_co;

void f1(void)
{
    printf("i:0x%08lx\n",ftobin(_ctof(0)));

#if 0
    printf("exp mantissa sign\n");
    printf("%02x  ",*(unsigned char*)(1024+(40*2)+0));
    printf("%02x  ",*(unsigned char*)(1024+(40*2)+1));
    printf("%02x  ",*(unsigned char*)(1024+(40*2)+2));
    printf("%02x  ",*(unsigned char*)(1024+(40*2)+3));
    printf("%02x  ",*(unsigned char*)(1024+(40*2)+4));
    printf("%02x  ",*(unsigned char*)(1024+(40*2)+5));
#endif

    var_bs=1024;
    var_fs=55304;

//        var_i=_(float)(var_bs);while(1)
    var_i=(float)(2);while(1)
    {
        printf("i:%d\n",(int)(var_i));
        var_bs++;if(var_bs==1030)break;

#if 0
        if(FCMPGT(0x00028000,U16TOF(0)))
        {
            if (FCMPGTEQ(var_i,(U16TOF(var_bs+1000)))) break;
        }
        else if(FCMPLT(0x00028000,U16TOF(0)))
        {
            if (FCMPLTEQ(var_i,(U16TOF(var_bs+1000)))) break;
        }
#endif
//        var_i=var_i + 0x00028000;
        var_i=var_i + 0x00818000;       // FIXME: float value?
//        var_i=var_i + _ctof(1);
    };
#ifdef __CC65__
    cgetc();
#endif
}


void testbinary(void)
{
    printf("\ntestbinary:\n\n");
    n= 5;b=(float)(n);printf("%d:0x%08lx\n", n, ftobin(b));
    n=-5;b=(float)(n);printf("%d:0x%08lx\n", n, ftobin(b));
    n= 2;b=(float)(n);printf("%d:0x%08lx\n", n, ftobin(b));
    n= 3;b=(float)(n);printf("%d:0x%08lx\n", n, ftobin(b));
#ifdef __CC65__
    cgetc();
#endif
}

void testbasicmath(void)
{
    printf("\ntestbasicmath:\n\n");
    t=123;
    fd=(float)((int)t);
    fs=_fneg(fd);
    _ftostr(strbuf,fd);
    printf("123:%s\n",strbuf);
    _ftostr(strbuf,fs);
    printf("-123:%s\n",strbuf);

    a = (float)(4321);
    b = (float)(1234);
    printf("4321:0x%08lx\n", ftobin(a));
    printf("1234:0x%08lx\n", ftobin(b));
    c1 = a + b;
    printf("4321+1234:0x%08lx\n", ftobin(c1));
    _ftoa(strbuf, c1);
    printf("4321+1234:%s\n", strbuf);

    a = (float)(1111);
    b = (float)(2222);
    printf("1111:0x%08lx\n", ftobin(a));
    printf("2222:0x%08lx\n", ftobin(b));
    c1 = a - b;
    printf("1111-2222:0x%08lx\n", ftobin(c1));
    _ftoa(strbuf, c1);
    printf("1111-2222:%s\n", strbuf);

    fd=(float)((int)t);
    fs=(float)((int)2);
    fd=fd / fs;
    _ftostr(strbuf,fd);
    printf("t:%s\n",strbuf);

    // 1234 / 60 = 20,5666...
    t=1234;
    fd=(float)((int)t);
    fs=(float)((int)60);
    fd=fd / fs;
    _ftostr(strbuf,fd);
    printf("t:%s\n",strbuf);

    // 5678 / 60 = 94,6333...
    t=5678;
    fd=(float)((int)t);
    fs=(float)((int)60);
    fd=fd / fs;
    _ftostr(strbuf,fd);
    printf("t:%s\n",strbuf);

    // ! operator DOES work on floats!
    fd=(float)((int)4);
    fd=!fd;
    _ftostr(strbuf,!fd);
    printf("!!4:%s\n",strbuf);

    fd=(float)((int)0);
    fd=!fd;
    _ftostr(strbuf,!fd);
    printf("!!0:%s\n",strbuf);

    a = 10.0f;
    b = powf(a,(float)(1)); printf("0x%08lx %s\n", ftobin(b), _ftoa(strbuf,b));
    b = powf(a,(float)(2)); printf("0x%08lx %s\n", ftobin(b), _ftoa(strbuf,b));
    b = powf(a,(float)(3)); printf("0x%08lx %s\n", ftobin(b), _ftoa(strbuf,b));
#ifdef __CC65__
    cgetc();
#endif
}

#define fcmp(_d, _s)    (((_d) < (_s)) ? 1 : (((_d) > (_s)) ? 255 : (0)))
#define fcmplt(_d, _s)  ((_d) < (_s))
#define fcmpgt(_d, _s)  ((_d) > (_s))
#define fcmpeq(_d, _s)  ((_d) == (_s))

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


void testprinting(void)
{
    printf("\ntestprinting:\n\n");
    a = 1.0f;
    b = (a / 10.0f); printf("0x%08lx %s %s\n", ftobin(b), _ftostr(strbuf, b), _ftoa(strbuf2, b));
    b = (a / 100.0f); printf("0x%08lx %s %s\n", ftobin(b), _ftostr(strbuf, b), _ftoa(strbuf2, b));
    b = (a / 1000.0f); printf("0x%08lx %s %s\n", ftobin(b), _ftostr(strbuf, b), _ftoa(strbuf2, b));
    b = (a / 10000.0f); printf("0x%08lx %s %s\n", ftobin(b), _ftostr(strbuf, b), _ftoa(strbuf2, b));
//    printf("\n");
    a = 2.0f;
    b = (a / 10.0f); printf("0x%08lx %s %s\n", ftobin(b), _ftostr(strbuf, b), _ftoa(strbuf2, b));
    b = (a / 100.0f); printf("0x%08lx %s %s\n", ftobin(b), _ftostr(strbuf, b), _ftoa(strbuf2, b));
    b = (a / 1000.0f); printf("0x%08lx %s %s\n", ftobin(b), _ftostr(strbuf, b), _ftoa(strbuf2, b));
    b = (a / 10000.0f); printf("0x%08lx %s %s\n", ftobin(b), _ftostr(strbuf, b), _ftoa(strbuf2, b));
//    printf("\n");
    a = 400000000.0f;
    b = (a * 10.0f); printf("0x%08lx %s %s\n", ftobin(b), _ftostr(strbuf, b), _ftoa(strbuf2, b));
    b = (a * 100.0f); printf("0x%08lx %s %s\n", ftobin(b), _ftostr(strbuf, b), _ftoa(strbuf2, b));
    b = (a * 1000.0f); printf("0x%08lx %s %s\n", ftobin(b), _ftostr(strbuf, b), _ftoa(strbuf2, b));
    b = (a * 10000.0f); printf("0x%08lx %s %s\n", ftobin(b), _ftostr(strbuf, b), _ftoa(strbuf2, b));
//    printf("\n");
    a = -2.0f;
    b = (a / 10.0f); printf("0x%08lx %s %s\n", ftobin(b), _ftostr(strbuf, b), _ftoa(strbuf2, b));
    b = (a / 100.0f); printf("0x%08lx %s %s\n", ftobin(b), _ftostr(strbuf, b), _ftoa(strbuf2, b));
    b = (a / 1000.0f); printf("0x%08lx %s %s\n", ftobin(b), _ftostr(strbuf, b), _ftoa(strbuf2, b));
    b = (a / 10000.0f); printf("0x%08lx %s %s\n", ftobin(b), _ftostr(strbuf, b), _ftoa(strbuf2, b));
//    printf("\n");
    a = -400000000.0f;
    b = (a * 10.0f); printf("0x%08lx %s %s\n", ftobin(b), _ftostr(strbuf, b), _ftoa(strbuf2, b));
    b = (a * 100.0f); printf("0x%08lx %s %s\n", ftobin(b), _ftostr(strbuf, b), _ftoa(strbuf2, b));
    b = (a * 1000.0f); printf("0x%08lx %s %s\n", ftobin(b), _ftostr(strbuf, b), _ftoa(strbuf2, b));
    b = (a * 10000.0f); printf("0x%08lx %s %s\n", ftobin(b), _ftostr(strbuf, b), _ftoa(strbuf2, b));
#ifdef __CC65__
    cgetc();
#endif
}

int main(void)
{
    printf("sizeof (float): %d\n", (int)sizeof(float));

    testbasicmath();
//    testlogical();
    testbinary();
    testcompare();
    testprinting();
#if 0
    f1();

    calcsin();
    calccos();
    testatan2();        // FIXME
#endif
#if 0
    calcpoly1();
#endif
    return EXIT_SUCCESS;
}
