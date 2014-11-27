/*
  !!DESCRIPTION!! display type limits
  !!ORIGIN!!      LCC 4.1 Testsuite
  !!LICENCE!!     own, freely distributeable for non-profit. read CPYRIGHT.LCC
*/

#include <stdio.h>
#include <limits.h>

#define SSHRT_MAX 	SHRT_MAX
#define SINT_MAX 	INT_MAX
#define SLONG_MAX 	LONG_MAX

#define UCHAR_MIN   0
#define USHRT_MIN   0
#define SSHRT_MIN   SHRT_MIN
#define UINT_MIN    0
#define SINT_MIN    INT_MIN
#define ULONG_MIN   0l
#define SLONG_MIN   LONG_MIN

int main(void) {
        printf("CHAR_MAX:  0x%08x=%d\n", CHAR_MAX, CHAR_MAX);
        printf("UCHAR_MAX: 0x%08x=%d\n", UCHAR_MAX, UCHAR_MAX);
        printf("SCHAR_MAX: 0x%08x=%d\n", SCHAR_MAX, SCHAR_MAX);

        printf("SHRT_MAX:  0x%08x=%d\n", SHRT_MAX, SHRT_MAX);
        printf("USHRT_MAX: 0x%08x=%d\n", USHRT_MAX, USHRT_MAX);
        printf("SSHRT_MAX: 0x%08x=%d\n", SSHRT_MAX, SSHRT_MAX);

        printf("INT_MAX:   0x%08x=%d\n", INT_MAX, INT_MAX);
        printf("UINT_MAX:  0x%08x=%d\n", UINT_MAX, UINT_MAX);
        printf("SINT_MAX:  0x%08x=%d\n", SINT_MAX, SINT_MAX);

        printf("LONG_MAX:  0x%08lx=%ld\n", LONG_MAX, LONG_MAX);
        printf("ULONG_MAX: 0x%08lx=%ld\n", ULONG_MAX, ULONG_MAX);
        printf("SLONG_MAX: 0x%08lx=%ld\n", SLONG_MAX, SLONG_MAX);

        printf("CHAR_MIN:  0x%08x=%d\n", CHAR_MIN, CHAR_MIN);
        printf("UCHAR_MIN: 0x%08x=%d\n", UCHAR_MIN, UCHAR_MIN);
        printf("SCHAR_MIN: 0x%08x=%d\n", SCHAR_MIN, SCHAR_MIN);

        printf("SHRT_MIN:  0x%08x=%d\n", SHRT_MIN, SHRT_MIN);
        printf("USHRT_MIN: 0x%08x=%d\n", USHRT_MIN, USHRT_MIN);
        printf("SSHRT_MIN: 0x%08x=%d\n", SSHRT_MIN, SSHRT_MIN);

        printf("INT_MIN:   0x%08x=%d\n", INT_MIN, INT_MIN);
        printf("UINT_MIN:  0x%08x=%d\n", UINT_MIN, UINT_MIN);
        printf("SINT_MIN:  0x%08x=%d\n", SINT_MIN, SINT_MIN);

        printf("LONG_MIN:  0x%08lx=%ld\n", LONG_MIN, LONG_MIN);
        printf("ULONG_MIN: 0x%08lx=%ld\n", ULONG_MIN, ULONG_MIN);
        printf("SLONG_MIN: 0x%08lx=%ld\n", SLONG_MIN, SLONG_MIN);

        return 0;
}
