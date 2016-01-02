/*
** !!DESCRIPTION!! Simple tests about adding pointers and offsets
** !!ORIGIN!!      cc65 regression tests
** !!LICENCE!!     Public Domain
** !!AUTHOR!!      2016-01-01, Greg King
*/

#include <stdio.h>

static unsigned char failures = 0;

static char array[16];

static char *cPtr;
static int  *iPtr;
static long *lPtr;

/* These functions test: adding an offset variable to a pointer variable. */

static void cPointer_char(void)
{
    char *cP = array;
    char offset = 3;

    cPtr = cP + offset;
    if (cPtr != (void *)&array[3]) {
        ++failures;
    }
}

static void cPointer_int(void)
{
    char *cP = array;
    int offset = 3;

    cPtr = cP + offset;
    if (cPtr != (void *)&array[3]) {
        ++failures;
    }
}

static void cPointer_long(void)
{
    char *cP = array;
    long offset = 3;

    cPtr = cP + offset;
    if (cPtr != (void *)&array[3]) {
        ++failures;
    }
}

static void iPointer_char(void)
{
    int *iP = (int *)array;
    char offset = 3;

    iPtr = iP + offset;
    if (iPtr != (void *)&array[6]) {
        ++failures;
    }
}

static void iPointer_int(void)
{
    int *iP = (int *)array;
    int offset = 3;

    iPtr = iP + offset;
    if (iPtr != (void *)&array[6]) {
        ++failures;
    }
}

static void iPointer_long(void)
{
    int *iP = (int *)array;
    long offset = 3;

    iPtr = iP + offset;
    if (iPtr != (void *)&array[6]) {
        ++failures;
    }
}

static void lPointer_char(void)
{
    long *lP = (long *)array;
    char offset = 3;

    lPtr = lP + offset;
    if (lPtr != (void *)&array[12]) {
        ++failures;
    }
}

static void lPointer_int(void)
{
    long *lP = (long *)array;
    int offset = 3;

    lPtr = lP + offset;
    if (lPtr != (void *)&array[12]) {
        ++failures;
    }
}

static void lPointer_long(void)
{
    long *lP = (long *)array;
    long offset = 3;

    lPtr = lP + offset;
    if (lPtr != (void *)&array[12]) {
        ++failures;
    }
}

/* These functions test: adding a pointer variable to an offset variable. */

static void char_cPointer(void)
{
    char *cP = array;
    char offset = 3;

    cPtr = offset + cP;
    if (cPtr != (void *)&array[3]) {
        ++failures;
    }
}

static void int_cPointer(void)
{
    char *cP = array;
    int offset = 3;

    cPtr = offset + cP;
    if (cPtr != (void *)&array[3]) {
        ++failures;
    }
}

static void long_cPointer(void)
{
    char *cP = array;
    long offset = 3;

    cPtr = (offset + cP);
    if (cPtr != (void *)&array[3]) {
        ++failures;
    }
}

static void char_iPointer(void)
{
    int *iP = (int *)array;
    char offset = 3;

    iPtr = offset + iP;
    if (iPtr != (void *)&array[6]) {
        ++failures;
    }
}

static void int_iPointer(void)
{
    int *iP = (int *)array;
    int offset = 3;

    iPtr = offset + iP;
    if (iPtr != (void *)&array[6]) {
        ++failures;
    }
}

static void long_iPointer(void)
{
    int *iP = (int *)array;
    long offset = 3;

    iPtr = (offset + iP);
    if (iPtr != (void *)&array[6]) {
        ++failures;
    }
}

static void char_lPointer(void)
{
    long *lP = (long *)array;
    char offset = 3;

    lPtr = offset + lP;
    if (lPtr != (void *)&array[12]) {
        ++failures;
    }
}

static void int_lPointer(void)
{
    long *lP = (long *)array;
    int offset = 3;

    lPtr = offset + lP;
    if (lPtr != (void *)&array[12]) {
        ++failures;
    }
}

static void long_lPointer(void)
{
    long *lP = (long *)array;
    long offset = 3;

    lPtr = (offset + lP);
    if (lPtr != (void *)&array[12]) {
        ++failures;
    }
}

int main(void)
{
    cPointer_char();
    cPointer_int();
    cPointer_long();

    iPointer_char();
    iPointer_int();
    iPointer_long();

    lPointer_char();
    lPointer_int();
    lPointer_long();

    char_cPointer();
     int_cPointer();
    long_cPointer();

    char_iPointer();
     int_iPointer();
    long_iPointer();

    char_lPointer();
     int_lPointer();
    long_lPointer();

    if (failures != 0) {
        printf("add5: failures: %u\n", failures);
    }
    return failures;
}
