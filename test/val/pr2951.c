
/* bug #2942/#2947: OptStackOps() sometimes removes Lhs loads shared with Rhs.
**
** These are synthetic, extreme reuse test cases: AX unchanged by pushax.
** These do not occur with the current codegen, runtime and optimizers, but
** may occur in the future. If the runtime or codegen change sufficiently to
** break these tests, they can be deleted.
*/

#include <stdio.h>
#include <stdlib.h>

int fail = 0;

#define TRASH_AX    0x55AA

int stat_val;

int stat_add_full(int trash)
{
    (void)trash;

    __AX__ = stat_val;
    /* AX = AX + AX */
    asm("jsr    pushax");
    asm("jsr    tosaddax");
    
    return __AX__;
}

int stat_xor_full(int trash)
{
    (void)trash;

    __AX__ = stat_val;
    /* AX = AX ^ AX */
    asm("jsr    pushax");
    asm("jsr    tosxorax");
    
    return __AX__;
}

int stat_xor_and(int trash)
{
    (void)trash;

    __AX__ = stat_val;
    /* AX = AX ^ (AX & 0xFFF0) */
    asm("jsr    pushax");
    asm("and    #$F0");
    asm("jsr    tosxorax");

    return __AX__;
}

int stat_or_inx(int trash)
{
    (void)trash;

    __AX__ = stat_val;
    /* AX = AX | (AX + 0x100) */
    asm("jsr    pushax");
    asm("inx");
    asm("jsr    tosorax");

    return __AX__;
}

int stat_and_add(int trash)
{
    (void)trash;

    __AX__ = stat_val;
    /* AX = (AX & 0xFFF0) + (AX & 0xFFF0) */
    asm("and    #$F0");
    asm("jsr    pushax");
    asm("jsr    tosaddax");

    return __AX__;
}

int stat_inx_add(int trash)
{
    (void)trash;

    __AX__ = stat_val;
    /* AX = (AX + 0x100) + (AX + 0x100) */
    asm("inx");
    asm("jsr    pushax");
    asm("jsr    tosaddax");

    return __AX__;
}

int stat_inx_or(int trash)
{
    (void)trash;

    __AX__ = stat_val;
    /* AX = (AX + 0x100) | (AX + 0x100) */
    asm("inx");
    asm("jsr    pushax");
    asm("jsr    tosorax");

    return __AX__;
}


int loc_add_full(int val, int trash)
{
    (void)trash;

    __AX__ = val;
    /* AX = AX + AX */
    asm("jsr    pushax");
    asm("jsr    tosaddax");
    
    return __AX__;
}

int loc_xor_full(int val, int trash)
{
    (void)trash;

    __AX__ = val;
    /* AX = AX ^ AX */
    asm("jsr    pushax");
    asm("jsr    tosxorax");
    
    return __AX__;
}

int loc_xor_and(int val, int trash)
{
    (void)trash;

    __AX__ = val;
    /* AX = AX ^ (AX & 0xFFF0) */
    asm("jsr    pushax");
    asm("and    #$F0");
    asm("jsr    tosxorax");

    return __AX__;
}

int loc_or_inx(int val, int trash)
{
    (void)trash;

    __AX__ = val;
    /* AX = AX | (AX + 0x100) */
    asm("jsr    pushax");
    asm("inx");
    asm("jsr    tosorax");

    return __AX__;
}

int loc_and_add(int val, int trash)
{
    (void)trash;

    __AX__ = val;
    /* AX = (AX & 0xFFF0) + (AX & 0xFFF0) */
    asm("and    #$F0");
    asm("jsr    pushax");
    asm("jsr    tosaddax");

    return __AX__;
}

int loc_inx_add(int val, int trash)
{
    (void)trash;

    __AX__ = val;
    /* AX = (AX + 0x100) + (AX + 0x100) */
    asm("inx");
    asm("jsr    pushax");
    asm("jsr    tosaddax");

    return __AX__;
}

int loc_inx_or(int val, int trash)
{
    (void)trash;

    __AX__ = val;
    /* AX = (AX + 0x100) | (AX + 0x100) */
    asm("inx");
    asm("jsr    pushax");
    asm("jsr    tosorax");

    return __AX__;
}


void test_stat(int (*func)(int /*trash*/), int num, int ref, const char* expr)
{
    int res;

    stat_val = num;
    res = func(TRASH_AX);
    if (res != ref) {
        printf("Fail (stat): %s -> 0x%x\n", expr, res);
        ++fail;
    }
}

void test_loc(int (*func)(int, int /*trash*/), int num, int ref, const char* expr)
{
    int res;

    res = func(num, TRASH_AX);
    if (res != ref) {
        printf("Fail (loc): %s -> 0x%x\n", expr, res);
        ++fail;
    }
}

#define TEST_STATIC(func, num, ref)     test_stat(func, num, (ref), #ref)
#define TEST_LOCAL(func, num, ref)      test_loc(func, num, (ref), #ref)

int main(void)
{
    TEST_STATIC(stat_add_full, 0x321, 0x321 + 0x321);
    TEST_STATIC(stat_xor_full, 0x321, 0x321 ^ 0x321);
    TEST_STATIC(stat_xor_and, 0x321, 0x321 ^ (0x321 & 0xFFF0));
    TEST_STATIC(stat_or_inx, 0x321, 0x321 | (0x321 + 0x100));
    TEST_STATIC(stat_and_add, 0x321, (0x321 & 0xFFF0) + (0x321 & 0xFFF0));
    TEST_STATIC(stat_inx_add, 0x321, (0x321 + 0x100) + (0x321 + 0x100));
    TEST_STATIC(stat_inx_or, 0x321, (0x321 + 0x100) | (0x321 + 0x100));

    TEST_LOCAL(loc_add_full, 0x321, 0x321 + 0x321);
    TEST_LOCAL(loc_xor_full, 0x321, 0x321 ^ 0x321);
    TEST_LOCAL(loc_xor_and, 0x321, 0x321 ^ (0x321 & 0xFFF0));
    TEST_LOCAL(loc_or_inx, 0x321, 0x321 | (0x321 + 0x100));
    TEST_LOCAL(loc_and_add, 0x321, (0x321 & 0xFFF0) + (0x321 & 0xFFF0));
    TEST_LOCAL(loc_inx_add, 0x321, (0x321 + 0x100) + (0x321 + 0x100));
    TEST_LOCAL(loc_inx_or, 0x321, (0x321 + 0x100) | (0x321 + 0x100));

    return fail == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
