/* Bug #2327 - Calling conventions and address size specifiers of functions in type names and parameter types are not parsed correctly */

#include <stdint.h>
#include <stdio.h>

unsigned failures;
int need_header = 1;
const char* test_header;

/* Helpers */
void set_header(const char* name)
{
    if (need_header == 0)
    {
        printf("\n");
    }
    test_header = name;
    need_header = 1;
}

void print_header(void)
{
    if (need_header)
    {
        need_header = 0;
        printf("<%s test>\n", test_header);
    }
}

#define CHECK(R, E) \
    do { \
        if ((R) != (E)) { \
            ++failures; \
            print_header(); \
            printf("    fail: %s = %d\n", #R, (R)); \
        } \
    } while (0);

#define CHECK_RV()  CHECK(rv, 42)
#define CHECK_SP()  CHECK(x - (intptr_t)&x, 0)

#define FUNC_QUAL __cdecl__ __near__

typedef int hoo_t(int __far__ __cdecl__ ());
typedef int hoo_t(int __far__ (__cdecl__)());       /* Question: should this be rejected? */
typedef int hoo_t(int __far__ (__cdecl__ ()));
typedef int hoo_t(int __far__ (__cdecl__ *)());
typedef int hoo_t(int __far__ (__cdecl__ (*)()));
typedef int hoo_t(int __far__ ((__cdecl__ *)()));

typedef int hoo_t(int __cdecl__ __far__ ());
typedef int hoo_t(int (__cdecl__ __far__)());       /* Question: should this be rejected? */
typedef int hoo_t(int (__cdecl__ __far__ ()));
typedef int hoo_t(int (__cdecl__ __far__ *)());
typedef int hoo_t(int (__cdecl__ (__far__ *)()));
typedef int hoo_t(int ((__cdecl__ __far__ *)()));

typedef int (FUNC_QUAL foo_t)(int, int);
typedef int (FUNC_QUAL *pfoo_t)(int, int);

int FUNC_QUAL foo(int a, int b)
{
    return a * b;
}

int (FUNC_QUAL * const pfoo)() = (int (FUNC_QUAL *)())foo;

/* Incompatible and not working for cc65 if used as-is */
int (*qfoo)(int, ...) = foo;

int main(void)
{
    int rv;
    intptr_t x;

    set_header("init");
    x = (intptr_t)&x;
    CHECK_SP()

    set_header("foo");
    rv = foo((int8_t)-3, (int32_t)-14);
    CHECK_RV()
    CHECK_SP()

    set_header("pfoo");
#if 0
    /* This would fail */
    rv = pfoo((int8_t)-6, (int32_t)-7);
#else
    rv = ((pfoo_t)pfoo)((int8_t)-6, (int32_t)-7);
#endif
    CHECK_RV()
    CHECK_SP()

    set_header("qfoo");
#if 0
    /* This would fail */
    rv = (qfoo)((int32_t)-6, (int8_t)-7);
#else
    rv = ((foo_t *)qfoo)((int32_t)-6, (int8_t)-7);
#endif
    CHECK_RV()
    CHECK_SP()

    if (failures > 0)
    {
        printf("\nfailures: %u\n", failures);
    }

    return failures;
}
