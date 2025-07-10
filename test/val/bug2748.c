#include "unittest.h"

int func(int expr)
{
   {
      int i = 5;
      return i;
   }
}

static size_t c_sp_before, c_sp_after;

TEST
{
    int a = 11;
    int b;

    __asm__("lda c_sp");
    __asm__("ldx c_sp+1");
    c_sp_before = __AX__;

    b = func(a);

    __asm__("lda c_sp");
    __asm__("ldx c_sp+1");
    c_sp_after = __AX__;

    ASSERT_IsTrue(c_sp_before == c_sp_after, "Unexpected stack pointer");
    ASSERT_IsTrue(b == 5, "Wrong value for b");
    ASSERT_IsTrue(a == 11, "Wrong value for a");
}
ENDTEST
