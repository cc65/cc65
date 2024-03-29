
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef __C64__
#include <conio.h>
#endif

/* apparently we dont trigger the bug when not using absolute addresses? */
#ifdef __C64__
#define TARGETMEM   0x4c8
#define SOURCEMEM   0x702
#elif __SIM6502__
#define TARGETMEM   0xc4c8
#define SOURCEMEM   0xc702
#elif __SIM65C02__
#define TARGETMEM   0xc4c8
#define SOURCEMEM   0xc702
#else
static unsigned char mem[0x10];
#define TARGETMEM   &mem[0]
#define SOURCEMEM   &mem[8]
#endif

/* do not put at pos. 1, and 1 byte apart - so we can eventually notice
    off-by-one errors */
static unsigned char u8w = 3;
static unsigned char u8r = 5;

static unsigned char target[8] = { 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7 };
static unsigned char source[8] = { 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf };
static unsigned char expect[8] = { 0x0, 0xb, 0xc, 0x3, 0x4, 0x5, 0x6, 0x7 };

static unsigned char i;
static unsigned char err = EXIT_SUCCESS;

void test1(void)
{
    ((unsigned char*)TARGETMEM)[--u8w] = ((unsigned char*)SOURCEMEM)[--u8r];
}

void dotest(void)
{

    memcpy(TARGETMEM, target, 8);
    memcpy(SOURCEMEM, source, 8);

    test1();

    memcpy(target, TARGETMEM, 8);
    memcpy(source, SOURCEMEM, 8);
#ifdef __C64__
    clrscr();
#endif
    printf("source:");
    for(i = 0; i < 8; ++i) {
        printf("%0x ", source[i]);
    }
    printf("\n\rtarget:");
    for(i = 0; i < 8; ++i) {
        printf("%0x ", target[i]);
    }
    printf("\n\r");

    printf("u8w: %d\n\r", u8w);
    printf("u8r:  %d\n\r", u8r);

}

int main(void)
{
    dotest();
    dotest();
    if (memcmp(target, expect, 8) != 0) {
        printf("buffer data error\n\r");
        err = EXIT_FAILURE;
    }
    if (u8w != 1) {
        err = EXIT_FAILURE;
    }
    if (u8r != 3) {
        err = EXIT_FAILURE;
    }
    printf("return: %d\n\r", err);
    return err;
}
