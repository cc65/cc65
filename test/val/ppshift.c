/*
  Test of bitwise-shift in preprocessor expressions.

  Note: Keep in mind that integer constants are always 32-bit in PP for cc65.
*/

/* Signed lhs */
#if 1 << 16 != 0x00010000
#error 1 << 16 != 0x00010000
#endif

#if 0x00010000 << -16 != 1
#error 0x00010000 << -16 != 1
#endif

#if 0x10000 >> 16 != 1
#error 0x10000 >> 16 != 1
#endif

#if 1 >> -16 != 0x10000
#error 1 >> -16 != 0x10000
#endif

#if 1 << 32 != 0
#error 1 << 32 != 0
#endif

#if 1 << -32 != 0
#error 1 << -32 != 0
#endif

#if 1 >> 32 != 0
#error 1 >> 32 != 0
#endif

#if 1 >> -32 != 0
#error 1 >> -32 != 0
#endif

#if -1 << 32 != 0
#error -1 << 32 != 0
#endif

#if -1 << -32 != -1
#error -1 << -32 != -1
#endif

#if -1 >> 32 != -1
#error -1 >> 32 != -1
#endif

#if -1 >> -32 != 0
#error -1 >> -32 != 0
#endif

/* NOTE: 2147483648 is an UNSIGNED integer! */
#if -1 << 2147483648 != 0
#error -1 << 2147483648 != 0
#endif

/* NOTE: -2147483648 is also an UNSIGNED integer! */
#if -1 << -2147483648 != 0
#error -1 << -2147483648 != 0
#endif

#if -1 << (-2147483647 - 1) != -1
#error -1 << (-2147483647 - 1) != -1
#endif

/* NOTE: 2147483648 is an UNSIGNED integer! */
#if -1 >> 2147483648 != -1
#error -1 >> 2147483648 != -1
#endif

/* NOTE: -2147483648 is also an UNSIGNED integer! */
#if -1 >> -2147483648 != -1
#error -1 >> -2147483648 != 0
#endif

#if -1 >> (-2147483647 - 1) != 0
#error -1 >> (-2147483647 - 1) != 0
#endif

/* Unsigned lhs */
#if 1U << 16 != 0x00010000
#error 1U << 16 != 0x00010000
#endif

#if 0x80000000U << -16 != 0x8000
#error 0x80000000U << -16 != 0x8000
#endif

#if 0x80000000U >> 16 != 0x8000
#error 0x80000000U >> 16 != 0x8000
#endif

#if 1U >> -16 != 0x10000
#error 1U >> -16 != 0x10000
#endif

#if -1U << 32 != 0
#error -1U << 32 != 0
#endif

#if -1U << -32 != 0
#error -1U << -32 != 0
#endif

#if -1U >> 32 != 0
#error -1U >> 32 != 0
#endif

#if -1U >> -32 != 0
#error -1U >> -32 != 0
#endif

int main(void)
{
    return 0;
}
