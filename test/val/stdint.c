/* Test definitions from stdint.h */

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <limits.h>
#include <signal.h>

/* All macros from stdint.h must be evaluatable by the preprocessor */
#if INT8_MIN
#endif
#if INT8_MAX
#endif
#if INT16_MIN
#endif
#if INT16_MAX
#endif
#if INT32_MIN
#endif
#if INT32_MAX
#endif
#if UINT8_MAX
#endif
#if UINT16_MAX
#endif
#if UINT32_MAX
#endif
#if INT_LEAST8_MIN
#endif
#if INT_LEAST8_MAX
#endif
#if INT_LEAST16_MIN
#endif
#if INT_LEAST16_MAX
#endif
#if INT_LEAST32_MIN
#endif
#if INT_LEAST32_MAX
#endif
#if UINT_LEAST8_MAX
#endif
#if UINT_LEAST16_MAX
#endif
#if UINT_LEAST32_MAX
#endif
#if INT_FAST8_MIN
#endif
#if INT_FAST8_MAX
#endif
#if INT_FAST16_MIN
#endif
#if INT_FAST16_MAX
#endif
#if INT_FAST32_MIN
#endif
#if INT_FAST32_MAX
#endif
#if UINT_FAST8_MAX
#endif
#if UINT_FAST16_MAX
#endif
#if UINT_FAST32_MAX
#endif
#if INTPTR_MIN
#endif
#if INTPTR_MAX
#endif
#if UINTPTR_MAX
#endif
#if INTMAX_MIN
#endif
#if INTMAX_MAX
#endif
#if UINTMAX_MAX
#endif
#if PTRDIFF_MIN
#endif
#if PTRDIFF_MAX
#endif
#if SIG_ATOMIC_MIN
#endif
#if SIG_ATOMIC_MAX
#endif
#if SIZE_MAX
#endif

#define SMIN(type)      ((type)(1L << (sizeof(type) * CHAR_BIT - 1)))
#define SMAX(type)      ((type)(~SMIN(type)))
#define UMAX(type)      ((type)(~(type)0))

#define SMIN_CHECK(type, val)                                           \
    if (SMIN(type) != val) {                                            \
        ++failures;                                                     \
        printf("Mismatch for %s, minimum (%ld) is not %s (%ld)\n",      \
               #type, (long)SMIN(type), #val, (long)val);               \
    }
#define SMAX_CHECK(type, val)                                           \
    if (SMAX(type) != val) {                                            \
        ++failures;                                                     \
        printf("Mismatch for %s, maximum (%ld) is not %s (%ld)\n",      \
               #type, (long)SMAX(type), #val, (long)val);               \
    }
#define UMAX_CHECK(type, val)                                           \
    if (UMAX(type) != val) {                                            \
        ++failures;                                                     \
        printf("Mismatch for %s, maximum (%lu) is not %s (%lu)\n",      \
               #type, (unsigned long)UMAX(type), #val,                  \
               (unsigned long)val);                                     \
    }

static unsigned failures = 0;

int main()
{
    SMIN_CHECK(int8_t, INT8_MIN);
    SMAX_CHECK(int8_t, INT8_MAX);
    SMIN_CHECK(int16_t, INT16_MIN);
    SMAX_CHECK(int16_t, INT16_MAX);
    SMIN_CHECK(int32_t, INT32_MIN);
    SMAX_CHECK(int32_t, INT32_MAX);
    UMAX_CHECK(uint8_t, UINT8_MAX);
    UMAX_CHECK(uint16_t, UINT16_MAX);
    UMAX_CHECK(uint32_t, UINT32_MAX);

    SMIN_CHECK(int_least8_t, INT_LEAST8_MIN);
    SMAX_CHECK(int_least8_t, INT_LEAST8_MAX);
    SMIN_CHECK(int_least16_t, INT_LEAST16_MIN);
    SMAX_CHECK(int_least16_t, INT_LEAST16_MAX);
    SMIN_CHECK(int_least32_t, INT_LEAST32_MIN);
    SMAX_CHECK(int_least32_t, INT_LEAST32_MAX);
    UMAX_CHECK(uint_least8_t, UINT_LEAST8_MAX);
    UMAX_CHECK(uint_least16_t, UINT_LEAST16_MAX);
    UMAX_CHECK(uint_least32_t, UINT_LEAST32_MAX);

    SMIN_CHECK(int_fast8_t, INT_FAST8_MIN);
    SMAX_CHECK(int_fast8_t, INT_FAST8_MAX);
    SMIN_CHECK(int_fast16_t, INT_FAST16_MIN);
    SMAX_CHECK(int_fast16_t, INT_FAST16_MAX);
    SMIN_CHECK(int_fast32_t, INT_FAST32_MIN);
    SMAX_CHECK(int_fast32_t, INT_FAST32_MAX);
    UMAX_CHECK(uint_fast8_t, UINT_FAST8_MAX);
    UMAX_CHECK(uint_fast16_t, UINT_FAST16_MAX);
    UMAX_CHECK(uint_fast32_t, UINT_FAST32_MAX);

    SMIN_CHECK(intptr_t, INTPTR_MIN);
    SMAX_CHECK(intptr_t, INTPTR_MAX);
    UMAX_CHECK(uintptr_t, UINTPTR_MAX);

    SMIN_CHECK(intmax_t, INTMAX_MIN);
    SMAX_CHECK(intmax_t, INTMAX_MAX);
    UMAX_CHECK(uintmax_t, UINTMAX_MAX);

    SMIN_CHECK(ptrdiff_t, PTRDIFF_MIN);
    SMAX_CHECK(ptrdiff_t, PTRDIFF_MAX);

#if SIG_ATOMIC_MIN < 0
    SMIN_CHECK(sig_atomic_t, SIG_ATOMIC_MIN);
    SMAX_CHECK(sig_atomic_t, SIG_ATOMIC_MAX);
#else
    UMAX_CHECK(sig_atomic_t, SIG_ATOMIC_MAX);
#endif

    UMAX_CHECK(size_t, SIZE_MAX);

    return failures;
}
