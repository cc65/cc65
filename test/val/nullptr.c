/* Bug # - Pointer compared to null pointer constant */

#include <stdio.h>

unsigned failures;

struct S {
    char a[4];
} *p;

#define TEST_NULL(E) \
    do { \
        a = (E) == 0 && !(E); \
        if (!a) \
        { \
            ++failures; \
            printf("failed: " #E " should be null\n"); \
        } \
    } while(0);

#define TEST_NON_NULL(E) \
    do { \
        a = (E) != 0 && !!(E) && (E); \
        if (!a) \
        { \
            ++failures; \
            printf("failed: " #E " should be non-null\n"); \
        } \
    } while(0);

int main()
{
    int a;

    /* Null pointer constant (per ISO C) compared equal to null pointer constant */
    TEST_NULL((void*)0)

    /* Null pointer compared equal to null pointer constant */
    TEST_NULL((char*)0)

    /* Null pointer obtained with -> */
    TEST_NULL(((struct S*)0)->a)

    /* Null pointer obtained with -> */
    TEST_NULL(p->a)

    /* Null pointer obtained with cast and -> */
    TEST_NULL(((struct S*)(a = 0))->a)

    /* Null pointer obtained with cast and -> */
    TEST_NULL((a = 0, ((struct S*)a)->a))

    /* Non-null pointer obtained with cast and -> */
    TEST_NON_NULL(((struct S*)(long)(a = 0x1234))->a)

    /* Non-null pointer obtained with cast and -> */
    TEST_NON_NULL((a = 0x1234, ((struct S*)a)->a))

    /* Non-null pointer obtained with cast and -> */
    TEST_NON_NULL(((struct S*)&a)->a)

    /* Non-null pointer obtained with cast and -> */
    TEST_NON_NULL(((struct S*)&main)->a)

    if (failures != 0)
    {
        printf("failures: %u\n", failures);
    }

    return failures;
}
