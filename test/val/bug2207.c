#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>



/********* Macros *********/

#define DEBUG_DETAIL 0

/* TODO: enable these when they can be compiled */
#define SKIP_VOID_RETURN_VALUE_TESTS        1
#define SKIP_INLINED_ARG_SIDE_EFFECT_TESTS  1

#define CHECK(C) \
    if (!(C)) { \
        ++failures; \
        print_header(); \
        printf("    failed: expected %s\n", #C); \
    }

#define CHECK_RANGE(L, R, D, N) \
    index = my_memcmp(L, R, D, N); \
    if (index >= 0) { \
        ++failures; \
        print_header(); \
        printf("    failed: %s vs %s dismatch at [%d]\n", #L, #R, index); \
    }

#define MEM_SIZE    512
#define STACK_SIZE  160
#define ZP_SIZE     8
#define MAGIC_SIZE  129

#define BROKEN_STR      "hello\0!"
#define BROKEN_STR_SIZE 6
#define BROKEN_STR_LEN  5



/********* Data *********/

unsigned failures;
int need_header = 1;
const char* test_header;

static int x;
static int y;
static int z;
static int index;

static char mem_dst[MEM_SIZE];
static char mem_src[MEM_SIZE];
static char mem_ori[MEM_SIZE];

#pragma data-name(push, "ZEROPAGE", "zp")
#pragma bss-name(push, "ZEROPAGE", "zp")
static char zp_src[ZP_SIZE];
static char zp_dst[ZP_SIZE];
static char zp_ori[ZP_SIZE];
static char* p_zp_src;
static char* p_zp_dst;
static char* p_zp_ori;
#pragma bss-name(pop)
#pragma data-name(pop)



/********* Helpers *********/

void my_memfill(void *dst, int init, size_t size)
{
    unsigned i;

    for (i = 0; i < size; ++i)
    {
        ((char*)dst)[i] = init;
        init += 3;
    }
}

void my_memset(void *dst, int val, size_t size)
{
    unsigned i;

    for (i = 0; i < size; ++i)
    {
        ((unsigned char *)dst)[i] = val;
    }
}

void my_memcpy(void *dst, const void *src, size_t size)
{
    unsigned i;

    for (i = 0; i < size; ++i)
    {
        ((char*)dst)[i] = ((char*)src)[i];
    }
}

int my_memcmp(const void *dst, const void *src, int diff, size_t size)
{
    unsigned i;

    diff %= 256;
    for (i = 0; i < size; ++i)
    {
        if ((unsigned char)(((unsigned char*)dst)[i] - ((unsigned char*)src)[i]) != diff)
        {
#if DEBUG_DETAIL
            /* DEBUG */
            printf("%d vs %d\n", (unsigned char)(((unsigned char*)dst)[i] - ((unsigned char*)src)[i]), diff);
#endif
            return i;
        }
    }

    return -1;
}

void* mul_two(void* p, int* px)
{
    *px *= 2;
    return p;
}

void* add_one(void* p, int* px)
{
    *px += 1;
    return p;
}

void* negate(void* p, int* px)
{
    *px = -*px;
    return p;
}

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
        printf("%s test\n", test_header);
    }
}



/********* Tests *********/

/* memcpy */
void test_memcpy(void)
{
    const char *name = 0;
    unsigned size = 0;
    void *p;

    /* init */
    my_memfill(mem_ori, 1000, sizeof mem_ori);
    my_memfill(zp_ori, 1000, sizeof zp_ori);
    p_zp_src = zp_src;
    p_zp_dst = zp_dst;
    p_zp_ori = zp_ori;

#if !SKIP_INLINED_ARG_SIDE_EFFECT_TESTS
    /* arg3 == 0 */
    set_header("p = memcpy(arg1, arg2, 0)");
    size = MEM_SIZE;
    my_memfill(mem_dst, 3000, size);
    my_memfill(mem_src, 2000, size);
    x = 42;
    y = -42;
    z = 36;
    p = memcpy(mul_two(mem_dst, &x), add_one(mem_src, &y), (negate(0, &z), 0));
    CHECK(p == mem_dst);
    CHECK(x == 84);
    CHECK(y == -41);
    CHECK(z == -36);
    CHECK_RANGE(mem_dst, mem_ori, 2000, size);
    CHECK_RANGE(mem_src, mem_ori, 1000, size);
#endif

#if !SKIP_INLINED_ARG_SIDE_EFFECT_TESTS
    /* Check if the arguments are still generated if the return value is unused.
    ** We have this question since the first argument could be directly used as
    ** the return value when this function gets inlined.
    */
#if !SKIP_VOID_RETURN_VALUE_TESTS
    set_header("(void)memcpy(arg1, arg2, 0)");
#else
    set_header("memcpy(arg1, arg2, 0)");
#endif
    size = MEM_SIZE;
    my_memfill(mem_dst, 3000, size);
    my_memfill(mem_src, 2000, size);
    x = 42;
    y = -42;
    z = 36;
#if !SKIP_VOID_RETURN_VALUE_TESTS
    (void)
#endif
    memcpy(mul_two(mem_dst, &x), add_one(mem_src, &y), (negate(0, &z), 0));
    CHECK(x == 84);
    CHECK(y == -41);
    CHECK(z == -36);
    CHECK_RANGE(mem_dst, mem_ori, 2000, size);
    CHECK_RANGE(mem_src, mem_ori, 1000, size);
#endif

    /* The memcpy inliner will give up with further optimizations if any of
    ** the arguments have side effects.
    */

    /* arg1: const addr, arg2: const addr, arg3 <= 129 */
    set_header("memcpy(const_addr_1, const_addr_2, 129) w/ side-effects");
    size = MEM_SIZE;
    my_memfill(mem_dst, 3000, size);
    my_memfill(mem_src, 2000, size);
    x = 42;
    y = -42;
    z = 36;
    /* memcpy size here must be an integer constant to allow the optimization */
    p = memcpy(mul_two(mem_dst, &x), add_one(mem_src, &y), (negate(0, &z), MAGIC_SIZE));
    size = MAGIC_SIZE;
    CHECK(p == mem_dst);
    CHECK(x == 84);
    CHECK(y == -41);
    CHECK(z == -36);
    CHECK_RANGE(mem_dst, mem_ori, 1000, size);
    CHECK_RANGE(mem_dst + size, mem_ori + size, 2000, MEM_SIZE - size);

    /* arg1: (void*)ptr, arg2: const_addr_2 */
    set_header("memcpy((void*)ptr, const_addr_2, 129)");
    {
        void *ptr = mem_dst;
        size = MEM_SIZE;
        my_memfill(mem_dst, 3000, size);
        my_memfill(mem_src, 2000, size);
        /* memcpy size here must be an integer constant to allow the optimization */
        p = memcpy((void*)ptr, mem_src, MAGIC_SIZE);
        size = MAGIC_SIZE;
        CHECK(p == mem_dst);
        CHECK_RANGE(mem_dst, mem_ori, 1000, size);
        CHECK_RANGE(mem_dst + size, mem_ori + size, 2000, MEM_SIZE - size);
    }

    /* arg1: const addr, arg2: const addr, arg3 <= 129 */
    set_header("memcpy(const_addr_1, const_addr_2, 129)");
    size = MEM_SIZE;
    my_memfill(mem_dst, 3000, size);
    my_memfill(mem_src, 2000, size);
    /* memcpy size here must be an integer constant to allow the optimization */
    p = memcpy(mem_dst, mem_src, MAGIC_SIZE);
    size = MAGIC_SIZE;
    CHECK(p == mem_dst);
    CHECK_RANGE(mem_dst, mem_ori, 1000, size);
    CHECK_RANGE(mem_dst + size, mem_ori + size, 2000, MEM_SIZE - size);

    /* arg1: const addr, arg2: const addr, arg3 <= 256 */
    set_header("memcpy(const_addr_1, const_addr_2, 256)");
    size = MEM_SIZE;
    my_memfill(mem_dst, 3000, size);
    my_memfill(mem_src, 2000, size);
    /* memcpy size here must be an integer constant to allow the optimization */
    p = memcpy(mem_dst, mem_src, 256);
    size = 256;
    CHECK(p == mem_dst);
    CHECK_RANGE(mem_dst, mem_ori, 1000, size);
    CHECK_RANGE(mem_dst + size, mem_ori + size, 2000, size - size);

    /* arg1: ptr on zp, arg2: ptr on zp, arg3 <= 256 */
    set_header("memcpy(p_on_zp_1, p_on_zp_2, 4)");
    /* We cannot allocate 256 bytes on the zeropage, unfortunately */
    my_memfill(zp_dst, 3000, ZP_SIZE);
    my_memfill(zp_src, 2000, ZP_SIZE);
    /* memcpy size here must be an integer constant to allow the optimization */
    p = memcpy(p_zp_dst, p_zp_src, ZP_SIZE / 2);
    size = ZP_SIZE / 2;
    CHECK(p == zp_dst);
    CHECK_RANGE(zp_dst, zp_ori, 1000, size);
    CHECK_RANGE(zp_dst + size, zp_ori + size, 2000, ZP_SIZE - size);

    /* arg1: on stack, arg2: const addr, arg3 <= 129 */
    set_header("memcpy(on_stack_1, const_addr_2, 129)");
    {
        char sp_dst[STACK_SIZE];
        my_memfill(sp_dst, 3000, STACK_SIZE);
        my_memfill(mem_src, 2000, STACK_SIZE);
        /* memcpy size here must be an integer constant to allow the optimization */
        p = memcpy(sp_dst, mem_src, MAGIC_SIZE);
        size = MAGIC_SIZE;
        CHECK(p == sp_dst);
        CHECK_RANGE(sp_dst, mem_ori, 1000, size);
        CHECK_RANGE(sp_dst + size, mem_ori + size, 2000, STACK_SIZE - size);
    }

    /* arg1: on stack, arg2: const addr, arg3 <= 256 */
    set_header("memcpy(on_stack_1, const_addr_2, 144)");
    {
        char sp_dst[STACK_SIZE];
        /* We cannot allocate 256 bytes on the stack, unfortunately */
        my_memfill(sp_dst, 3000, STACK_SIZE);
        my_memfill(mem_src, 2000, STACK_SIZE);
        /* memcpy size here must be an integer constant to allow the optimization */
        p = memcpy(sp_dst, mem_src, 144);
        size = 144;
        CHECK(p == sp_dst);
        CHECK_RANGE(sp_dst, mem_ori, 1000, size);
        CHECK_RANGE(sp_dst + size, mem_ori + size, 2000, STACK_SIZE - size);
    }

    /* arg1: const addr, arg2: on stack, arg3 <= 129 */
    set_header("memcpy(const_addr_1, on_stack_2, 129)");
    {
        char sp_src[STACK_SIZE];
        /* We cannot allocate 256 bytes on the stack, unfortunately */
        my_memfill(mem_dst, 3000, STACK_SIZE);
        my_memfill(sp_src, 2000, STACK_SIZE);
        /* memcpy size here must be an integer constant to allow the optimization */
        p = memcpy(mem_dst, sp_src, 129);
        size = 129;
        CHECK(p == mem_dst);
        CHECK_RANGE(mem_dst, mem_ori, 1000, size);
        CHECK_RANGE(mem_dst + size, mem_ori + size, 2000, STACK_SIZE - size);
    }

    /* arg1: const addr, arg2: on stack, arg3 <= 256 */
    set_header("memcpy(const_addr_1, on_stack_2, 144)");
    {
        char sp_src[STACK_SIZE];
        /* We cannot allocate 256 bytes on the stack, unfortunately */
        my_memfill(mem_dst, 3000, STACK_SIZE);
        my_memfill(sp_src, 2000, STACK_SIZE);
        /* memcpy size here must be an integer constant to allow the optimization */
        p = memcpy(mem_dst, sp_src, 144);
        size = 144;
        CHECK(p == mem_dst);
        CHECK_RANGE(mem_dst, mem_ori, 1000, size);
        CHECK_RANGE(mem_dst + size, mem_ori + size, 2000, STACK_SIZE - size);
    }
}

/* memset */
void test_memset(void)
{
    const char *name = 0;
    unsigned size = 0;
    void *p;

    /* init */
    my_memfill(mem_ori, 1000, sizeof mem_ori);
    my_memfill(zp_ori, 1000, sizeof zp_ori);
    p_zp_dst = zp_dst;
    p_zp_ori = zp_ori;

#if !SKIP_INLINED_ARG_SIDE_EFFECT_TESTS
    /* arg3 == 0 */
    set_header("p = memset(arg1, arg2, 0)");
    size = MEM_SIZE;
    my_memfill(mem_dst, 3000, size);
    x = 42;
    y = -42;
    z = 36;
    p = memset(mul_two(mem_dst, &x), (add_one(0, &y), 42), (negate(0, &z), 0));
    CHECK(p == mem_dst);
    CHECK(x == 84);
    CHECK(y == -41);
    CHECK(z == -36);
    CHECK_RANGE(mem_dst, mem_ori, 2000, size);
#endif

#if !SKIP_INLINED_ARG_SIDE_EFFECT_TESTS
    /* Check if the arguments are still generated if the return value is unused.
    ** We have this question since the first argument could be directly used as
    ** the return value when this function gets inlined.
    */
#if !SKIP_VOID_RETURN_VALUE_TESTS
    set_header("(void)memset(arg1, arg2, 0)");
#else
    set_header("memset(arg1, arg2, 0)");
#endif
    size = MEM_SIZE;
    my_memfill(mem_dst, 3000, size);
    x = 42;
    y = -42;
    z = 36;
#if !SKIP_VOID_RETURN_VALUE_TESTS
    (void)
#endif
    memset(mul_two(mem_dst, &x), (add_one(0, &y), 42), (negate(0, &z), 0));
    CHECK(x == 84);
    CHECK(y == -41);
    CHECK(z == -36);
    CHECK_RANGE(mem_dst, mem_ori, 2000, size);
#endif
}

/* strcmp */
void test_strcmp(void)
{
    const char *name = 0;
    unsigned size = 0;
    int res = 0;

    /* init */
    p_zp_dst = zp_dst;

    /* Compared to zero-length C string literal */
    set_header("strcmp(arg1, \"\\0Z\")");
    {
        char str[] = "AA";
        size = sizeof str;
        my_memcpy(zp_dst, str, size);
        x = 42;
        y = -42;

        res = strcmp(zp_dst, "\0Z");
        CHECK(res > 0);

#if !SKIP_INLINED_ARG_SIDE_EFFECT_TESTS
        res = strcmp(mul_two(p_zp_dst, &x), (add_one(0, &y), "\0Z"));
        CHECK(res > 0);
        CHECK(x == 84);
        CHECK(y == -41);
#endif
    }
}

/* strcpy */
void test_strcpy(void)
{
    const char *name = 0;
    unsigned size = 0;
    void *p;
    char stack_dst[ZP_SIZE];
    char stack_src[ZP_SIZE];

    /* init */
    {
        char str[] = BROKEN_STR;
        size = BROKEN_STR_LEN + 1;
        my_memfill(mem_src, 1000, ZP_SIZE);
        my_memfill(stack_src, 1000, ZP_SIZE);
        my_memfill(zp_src, 1000, ZP_SIZE);
        my_memcpy(mem_src, str, size);
        my_memcpy(stack_src, str, size);
        my_memcpy(zp_src, str, size);
        p_zp_src = zp_src;
        p_zp_dst = zp_dst;
        p_zp_dst = zp_dst;
    }

    /* arg1: const addr, arg2: const addr */
    set_header("strcpy(const_addr_1, const_addr_2)");
    my_memfill(mem_dst, 2000, ZP_SIZE);
    size = BROKEN_STR_LEN + 1;
    p = strcpy(mem_dst, mem_src);
    CHECK(p == mem_dst);
    CHECK_RANGE(mem_dst, mem_src, 0, size);
    CHECK_RANGE(mem_dst + size, mem_src + size, 1000, ZP_SIZE - size);

    /* arg1: ptr on zp, arg2: ptr on zp */
    set_header("strcpy(p_on_zp_1, p_on_zp_2)");
    my_memfill(zp_dst, 2000, ZP_SIZE);
    size = BROKEN_STR_LEN + 1;
    p = strcpy(zp_dst, zp_src);
    CHECK(p == zp_dst);
    CHECK_RANGE(zp_dst, zp_src, 0, size);
    CHECK_RANGE(zp_dst + size, zp_src + size, 1000, ZP_SIZE - size);

    /* arg1: on stack, arg2: on stack */
    set_header("strcpy(on_stack_1, on_stack_2)");
    my_memfill(stack_dst, 2000, ZP_SIZE);
    size = BROKEN_STR_LEN + 1;
    p = strcpy(stack_dst, stack_src);
    CHECK(p == stack_dst);
    CHECK_RANGE(stack_dst, stack_src, 0, size);
    CHECK_RANGE(stack_dst + size, stack_src + size, 1000, ZP_SIZE - size);

    /* TODO: args side-effects tests */
}

/* strlen */
void test_strlen(void)
{
    const char *name = 0;
    size_t len;

    /* init */
    {
        char str[] = BROKEN_STR;
        my_memcpy(mem_ori, str, ZP_SIZE);
        //my_memcpy(stack_ori, str, ZP_SIZE);
        my_memcpy(zp_ori, str, ZP_SIZE);
        p_zp_ori = zp_ori;
    }

    /* arg1: string_literal */
    set_header("strlen(\"hello\\0!\")");
    x = 42;
    y = -42;
    len = strlen((mul_two(0, &x), BROKEN_STR));
    (void)strlen((add_one(0, &y), BROKEN_STR));
    CHECK(len == BROKEN_STR_LEN);
    CHECK(x == 84);
    CHECK(y == -41);

    /* arg1: array with const addr */
    set_header("strlen(array_const_addr[8])");
    x = 42;
    y = -42;
    len = strlen((mul_two(0, &x), zp_ori));
    (void)strlen((add_one(0, &y), zp_ori));
    CHECK(len == BROKEN_STR_LEN);
    CHECK(x == 84);
    CHECK(y == -41);

    /* arg1: array on stack */
    set_header("strlen(array_on_stack[8])");
    {
        char p_on_stack[] = BROKEN_STR;
        x = 42;
        y = -42;
        len = strlen((mul_two(0, &x), p_on_stack));
        strlen((add_one(0, &y), p_on_stack));
        CHECK(sizeof p_on_stack == 8);
        CHECK(len == BROKEN_STR_LEN);
        CHECK(x == 84);
        CHECK(y == -41);
    }

    /* arg1: ptr on zp */
    set_header("strlen(ptr_on_zp)");
    x = 42;
    y = -42;
    len = strlen((mul_two(0, &x), p_zp_ori));
    (void)strlen((add_one(0, &y), p_zp_ori));
    CHECK(len == BROKEN_STR_LEN);
    CHECK(x == 84);
    CHECK(y == -41);
}

int main(void)
{
    test_memcpy();
    test_memset();
    test_strcmp();
    test_strcpy();
    test_strlen();

    if (failures > 0)
    {
        printf("failed items: %u\n", failures);
    }
    return failures;
}
