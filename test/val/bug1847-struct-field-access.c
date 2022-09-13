/* Bug #1847 - struct field access */

#include <stdio.h>

struct TestStruct {
    char a;
    char b;
    char c;
};

struct TestStruct s0[2] = { {0xFF, 0, 0xFF}, {0, 0x42, 0xFF} };
struct TestStruct* s0Ptr = s0;

#define TEST_READ_SUB(X, E) \
    if ((X) != (E)) { \
        printf(#X ": 0x%X, expected: 0x%X\n", (X), (E)); \
        ++failures; \
    }

#define TEST_READ(S, I, F, E) \
    TEST_READ_SUB(S[I].F, E) \
    TEST_READ_SUB((&S[I])->F, E) \
    TEST_READ_SUB((&S[I])[0].F, E) \
    TEST_READ_SUB(S##Ptr[I].F, E) \
    TEST_READ_SUB((&S##Ptr[I])->F, E) \
    TEST_READ_SUB((&(S##Ptr[I]))[0].F, E) \
    TEST_READ_SUB((&(*S##Ptr))[I].F, E) \
    TEST_READ_SUB((&(*S##Ptr)+I)->F, E) \
    TEST_READ_SUB((S##Ptr+I)->F, E) \
    TEST_READ_SUB((S##Ptr+I)[0].F, E)

static unsigned failures = 0;

int main(void) {
    struct TestStruct  s1[2] = { {0xFF, 0, 0xFF}, {0, 42, 0xFF} };
    struct TestStruct* s1Ptr = s1;

    TEST_READ(s0, 1, b, 0x42)
    TEST_READ(s1, 1, b, 42)

    if (failures > 0) {
        printf("Failures: %u\n", failures);
    }
    
    return 0;
}
