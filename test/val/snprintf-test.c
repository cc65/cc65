/*
** Test a function that formats and writes characters into a string buffer.
** This program does not test formatting.  It tests some behaviors that are
** specific to the buffer.  It tests that certain conditions are handled
** properly.
**
** 2015-07-17, Greg King
*/

#include <conio.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

static const char format[] = "1234567890\nabcdefghijklmnopqrstuvwxyz\n%u\n%s\n\n";
#define FORMAT_SIZE (sizeof format - 2u - 2u - 1u)

#define arg1 12345u
#define ARG1_SIZE (5u)

static const char arg2[] = "!@#$%^&*()-+";
#define ARG2_SIZE (sizeof arg2 - 1u)

#define STRING_SIZE (FORMAT_SIZE + ARG1_SIZE + ARG2_SIZE)

static char buf[256];
static int size;


static void fillbuf(void)
{
    memset(buf, 0xFF, sizeof buf - 1u);
    buf[sizeof buf - 1u] = '\0';
}


unsigned char main(void)
{
    static unsigned char failures = 0;

    /* Show what sprintf() should create. */

    if ((size = printf(format, arg1, arg2)) != STRING_SIZE) {
        ++failures;
        printf("printf() gave the wrong size: %d.\n", size);
    }

    /* Test the normal behavior of sprintf(). */

    fillbuf();
    size = sprintf(buf, format, arg1, arg2);
    fputs(buf, stdout);
    if (size != STRING_SIZE) {
        ++failures;
        printf("sprintf() gave the wrong size: %d.\n", size);
    }

    /* Test the normal behavior of snprintf(). */

    fillbuf();
    size = snprintf(buf, sizeof buf, format, arg1, arg2);
    fputs(buf, stdout);
    if (size != STRING_SIZE) {
        ++failures;
        printf("snprintf(sizeof buf) gave the wrong size:\n %d.\n", size);
    }

    /* Does snprintf() return the full-formatted size even when the buffer
    ** is short?  Does it write beyond the end of that buffer?
    */

    fillbuf();
    size = snprintf(buf, STRING_SIZE - 5u, format, arg1, arg2);
    if (size != STRING_SIZE) {
        ++failures;
        printf("snprintf(STRING_SIZE-5) gave the wrong size:\n %d.\n", size);
    }
    if (buf[STRING_SIZE - 5u - 1u] != '\0' || buf[STRING_SIZE - 5u] != 0xFF) {
        ++failures;
        printf("snprintf(STRING_SIZE-5) wrote beyond\n the end of the buffer.\n");
    }

    /* Does snprintf() detect a buffer size that is too big? */

    fillbuf();
    errno = 0;
    size = snprintf(buf, 0x8000, format, arg1, arg2);
    if (size >= 0) {
        ++failures;
        printf("snprintf(0x8000) didn't give an error:\n %d; errno=%d.\n", size, errno);
    } else {
        printf("snprintf(0x8000) did give an error:\n errno=%d.\n", errno);
    }
    if (buf[0] != 0xFF) {
        ++failures;
        printf("snprintf(0x8000) wrote into the buffer.\n");
    }

    /* snprintf() must measure the length of the formatted output even when the
    ** buffer size is zero.  But, it must not touch the buffer.
    */

    fillbuf();
    size = snprintf(buf, 0, format, arg1, arg2);
    if (size != STRING_SIZE) {
        ++failures;
        printf("snprintf(0) gave the wrong size:\n %d.\n", size);
    }
    if (buf[0] != 0xFF) {
        ++failures;
        printf("snprintf(0) wrote into the buffer.\n");
    }

    /* Does sprintf() detect a zero buffer-pointer? */

    errno = 0;
    size = sprintf(NULL, format, arg1, arg2);
    if (size >= 0) {
        ++failures;
        printf("sprintf(NULL) didn't give an error:\n %d; errno=%d.\n", size, errno);
    } else {
        printf("sprintf(NULL) did give an error:\n errno=%d.\n", errno);
    }

    /* snprintf() must measure the length of the formatted output even when the
    ** buffer size is zero.  A zero pointer is not an error, in that case.
    */

    size = snprintf(NULL, 0, format, arg1, arg2);
    if (size != STRING_SIZE) {
        ++failures;
        printf("snprintf(NULL,0) gave the wrong size:\n %d.\n", size);
    }

    if (failures != 0) {
        printf("There were %u", failures);
    } else {
        printf("There were no");
    }
    printf(" failures.\nTap a key. ");
    cgetc();

    return failures;
}
