
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#ifdef __CC65__
#define testasm1(C) (__AX__ = (C),   \
                     asm("and #$3f"),\
                    __AX__)

#define testasm2(C) (__A__ = (C),   \
                     asm("and #$3f"),\
                    __A__)
#else
/* Non-cc65 compiler. Just make the code compile and work. */
uint16_t testasm1(uint16_t C)
{
    uint16_t AX = C;
    AX &= 0x3f;
    return AX;
}

uint8_t testasm2(uint8_t C)
{
    uint8_t A = C;
    A &= 0x3f;
    return A;
}
#endif

uint8_t src[32]  = { 0x10, 0x41, 0x62, 0x83, 0xb4, 0xf5,  0xe6,  0xc7, 0, 0 };
uint8_t src2[32] = { 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6,  0xc7,  0xc8, 0, 0 };


uint8_t ref1[32] = { 0x10, 0x01, 0x22, 0x03, 0x34, 0x35,  0x26,  0x07, 0, 0 };
uint8_t ref2[32] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,  0x07,  0x08, 0, 0 };

uint8_t dest[32];
int res = 0;

void dotest1a(uint8_t *s, uint8_t *d)
{
    printf("dotest1a\n");
    while (*s != 0) {
        *d = (testasm1(*s));
//        printf("%04x:%02x\n",d,*d);
        d++;
        s++;
    }
}

void dotest1b(uint8_t *s, uint8_t *d)
{
    printf("dotest1b\n");
    while (*s != 0) {
        *d = (testasm2(*s));
        d++;
        s++;
    }
}

void dotest2a (void)
{
  char *p = &src2[0];
  uintptr_t scaddr=&dest[0];     //output to line 11 on the screen

    printf("dotest2a\n");
    while (*p != 0) {
        (*(unsigned char *)(scaddr++)=(testasm1(*p)));
        p++;
    }
}

void dotest2b (void)
{
  char *p = &src2[0];
  uintptr_t scaddr=&dest[0];     //output to line 11 on the screen

    printf("dotest2b\n");
    while (*p != 0) {
        (*(unsigned char *)(scaddr++)=(testasm2(*p)));
        p++;
    }
}

int docmp(uint8_t *s, uint8_t *r)
{
    int res = 0;
    while (*s != 0) {
//        printf("is %02x\n", *s);
        if (*r != *s) {
            printf("is %02x expected %02x\n", *s, *r);
            res++;
        }
        r++;
        s++;
    }
    return res;
}

int main(void)
{
    memset(dest, 0, 10);
    memset(dest, 0x11, 8);
    dotest1a(src, dest);
    res = docmp(dest, ref1);

    memset(dest, 0, 10);
    memset(dest, 0x22, 8);
    dotest2a();
    res += docmp(&dest[0], &ref2[0]);

    memset(dest, 0, 10);
    memset(dest, 0x33, 8);
    dotest1b(&src[0], &dest[0]);
    res = docmp(&dest[0], &ref1[0]);

    memset(dest, 0, 10);
    memset(dest, 0x44, 8);
    dotest2b();
    res += docmp(&dest[0], &ref2[0]);

    printf("res: %02x\n", res);
    return res;
}
