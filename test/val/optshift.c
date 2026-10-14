/* Tests exercising OptShiftX optimizer step conditions not triggered
** by other tests.
*/

#include <stdio.h>
#include <stdlib.h>

static int failures = 0;

unsigned char uc31 = 31;

int shift2_c_ok(signed char x)
{
    return (x >> 1) & 0x00ff;
}

int shift2_c_no(signed char x)
{
    /* Ternary operator produces an additional branch to asrax1 call;
    **   & 0x00ff discards X reg and makes it "unused". */
    return (((x < 0) ? (-16384 - x * 4) : x) >> 1) & 0x00ff;
}

int shift2_c_x01(signed char x)
{
    /* TODO: Currently, invalid code is produced due to #3010. */
    /*   & 0x00ff discards X reg and makes it "unused". */
    return ((0x100 + x) >> 2) & 0x00ff;
}

/* OptShift2 pattern, signed char addition with promotion (X != 0) */
int shift2_asm_x01(signed char x)
{
    static signed char sx;
    sx = x;

    /* return ((0x100 + sx) >> 2) & 0x00ff; */

    __AX__ = 0x100;
    /* g_addstatic(CF_CHAR, sx); signed char addition with promotion */
    asm("clc");
    asm("adc  %v", sx);
    asm("bcc  %g", no_ovfl);
    asm("inx");
no_ovfl:
    asm("bit  %v", sx);
    /* OptShift2 pattern starts here */
    asm("bpl  %g", non_neg);
    asm("dex");
non_neg:
    asm("jsr  asrax2");

    /*   & 0x00ff discards X reg and makes it "unused". */
    return __AX__ & 0x00ff;
}

/* OptShift3 pattern can come from multiple kinds of expressions,
** for example, from g_addlocal/g_addstatic(CF_CHAR); g_asr(CF_INT);
** Currently, the pattern is eliminated by OptDecouple.
*/
int shift3_c_ok(unsigned char x)
{
    return (x + uc31) >> 2;
}

int shift3_c_no(unsigned char x)
{
    /* Ternary operator produces an additional branch to asrax2 call */
    return ((x > 200) ? (-x * 8) : (x + uc31)) >> 2;
}

#define CHECK(func, expect) \
    res = (func); \
    if (res != (expect)) { \
        ++failures; \
        printf(#func ": Got %d, expected %d\n", res, (expect)); \
    }

int main(void)
{
    int res;

    CHECK(shift2_c_ok(84), 42);
    CHECK(shift2_c_ok(-84), -42 & 0x00ff);
    CHECK(shift2_c_no(84), 42);

    /* TODO: Currently fails in HEAD with -O due to unchecked label refs. */
    /* CHECK(shift2_c_no(-42), 84); */

    CHECK(shift2_c_x01(80), 84);
    /* TODO: Currently fails in HEAD due to #3010. */
    /* CHECK(shift2_c_x01(-88), 42); */

    CHECK(shift2_asm_x01(-88), 42);
    CHECK(shift2_asm_x01(80), 84);

    CHECK(shift3_c_ok(137), 42);
    CHECK(shift3_c_no(137), 42);
    CHECK(shift3_c_no(210), -420);

    printf("failures: %d\n", failures);

    return failures ? EXIT_FAILURE : EXIT_SUCCESS;
}
