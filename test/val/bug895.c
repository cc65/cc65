/** This test is related to GitHub issue 895
 ** https://github.com/cc65/cc65/issues/895
 **
 ** The OptCmp8 optimization attempted to eliminate an unnecessary
 ** comparison and branch when the operands of the comparison are
 ** known to be constant at compile time.
 **
 ** For 8-bit types it worked well, but for 16-bit types it failed
 ** to generate correct code for some cases. The bug manifest as a
 ** branch on an uninitialized carry flag.
 */

#include "unittest.h"

signed char   sca, scb;
signed int    sia, sib;
signed long   sla, slb;

unsigned char uca, ucb;
unsigned int  uia, uib;
unsigned long ula, ulb;

#define OPTCMP8TEST_SINGLE(num,cmpop,asmprefix,vara,varb,b0,b1,a0,a1,typename,name) \
    typename name ## _ ## num(void) { \
        varb = b0; \
        asm( asmprefix ); \
        vara = a0; \
        if (vara cmpop a1) varb = b1; \
        return varb; \
    }

#define OPTCMP8TEST_VERIFY(num,b,desc,printterm,name) \
    ASSERT_AreEqual(name ## _ ## num(),b,printterm,"Incorrect optimization of const comparison (" #name "_" #num ": " desc ").");

/* Generates a set of comparison tests for one type and set of test values.
**     name = a name for this test (no spaces)
**     typename = the type used
**     b0 = result if comparison is false
**     b1 = result if comparison is true
**     a0 = a low value to use for the comparison tests (a0 < a1)
**     a1 = a high value to use for the comparison tests (a0 < a1)
**     vara = temporary variable of the type to be examined
**     varb = temporary variable of the type to be examined
**     printterm = printf term to display the variable type
*/
#define OPTCMP8TEST(name,typename,b0,b1,a0,a1,vara,varb,printterm) \
    OPTCMP8TEST_SINGLE(1,<,"clc",vara,varb,b0,b1,a0,a1,typename,name); \
    OPTCMP8TEST_SINGLE(2,<,"sec",vara,varb,b0,b1,a0,a1,typename,name); \
    OPTCMP8TEST_SINGLE(3,<,"clc",vara,varb,b0,b1,a1,a0,typename,name); \
    OPTCMP8TEST_SINGLE(4,<,"sec",vara,varb,b0,b1,a1,a0,typename,name); \
    OPTCMP8TEST_SINGLE(5,>,"clc",vara,varb,b0,b1,a0,a1,typename,name); \
    OPTCMP8TEST_SINGLE(6,>,"sec",vara,varb,b0,b1,a0,a1,typename,name); \
    OPTCMP8TEST_SINGLE(7,>,"clc",vara,varb,b0,b1,a1,a0,typename,name); \
    OPTCMP8TEST_SINGLE(8,>,"sec",vara,varb,b0,b1,a1,a0,typename,name); \
    OPTCMP8TEST_SINGLE(9,<=,"clc",vara,varb,b0,b1,a0,a1,typename,name); \
    OPTCMP8TEST_SINGLE(10,<=,"sec",vara,varb,b0,b1,a0,a1,typename,name); \
    OPTCMP8TEST_SINGLE(11,<=,"clc",vara,varb,b0,b1,a1,a0,typename,name); \
    OPTCMP8TEST_SINGLE(12,<=,"sec",vara,varb,b0,b1,a1,a0,typename,name); \
    OPTCMP8TEST_SINGLE(13,>=,"clc",vara,varb,b0,b1,a0,a1,typename,name); \
    OPTCMP8TEST_SINGLE(14,>=,"sec",vara,varb,b0,b1,a0,a1,typename,name); \
    OPTCMP8TEST_SINGLE(15,>=,"clc",vara,varb,b0,b1,a1,a0,typename,name); \
    OPTCMP8TEST_SINGLE(16,>=,"sec",vara,varb,b0,b1,a1,a0,typename,name); \
    OPTCMP8TEST_SINGLE(17,==,"nop",vara,varb,b0,b1,a0,a1,typename,name); \
    OPTCMP8TEST_SINGLE(18,==,"nop",vara,varb,b0,b1,a1,a1,typename,name); \
    OPTCMP8TEST_SINGLE(19,!=,"nop",vara,varb,b0,b1,a0,a1,typename,name); \
    OPTCMP8TEST_SINGLE(20,!=,"nop",vara,varb,b0,b1,a1,a1,typename,name); \
    void name ## _ ## test(void) { \
        OPTCMP8TEST_VERIFY(1,b1,"low < high, clc",printterm,name); \
        OPTCMP8TEST_VERIFY(2,b1,"low < high, sec",printterm,name); \
        OPTCMP8TEST_VERIFY(3,b0,"high < low, clc",printterm,name); \
        OPTCMP8TEST_VERIFY(4,b0,"high < low, sec",printterm,name); \
        OPTCMP8TEST_VERIFY(5,b0,"low > high, clc",printterm,name); \
        OPTCMP8TEST_VERIFY(6,b0,"low > high, sec",printterm,name); \
        OPTCMP8TEST_VERIFY(7,b1,"high > low, clc",printterm,name); \
        OPTCMP8TEST_VERIFY(8,b1,"high > low, sec",printterm,name); \
        OPTCMP8TEST_VERIFY(9,b1,"low <= high, clc",printterm,name); \
        OPTCMP8TEST_VERIFY(10,b1,"low <= high, sec",printterm,name); \
        OPTCMP8TEST_VERIFY(11,b0,"high <= low, clc",printterm,name); \
        OPTCMP8TEST_VERIFY(12,b0,"high <= low, sec",printterm,name); \
        OPTCMP8TEST_VERIFY(13,b0,"low >= high, clc",printterm,name); \
        OPTCMP8TEST_VERIFY(14,b0,"low >= high, sec",printterm,name); \
        OPTCMP8TEST_VERIFY(15,b1,"high >= low, clc",printterm,name); \
        OPTCMP8TEST_VERIFY(16,b1,"high >= low, sec",printterm,name); \
        OPTCMP8TEST_VERIFY(17,b0,"low == high, nop",printterm,name); \
        OPTCMP8TEST_VERIFY(18,b1,"high == high, nop",printterm,name); \
        OPTCMP8TEST_VERIFY(19,b1,"low != high, nop",printterm,name); \
        OPTCMP8TEST_VERIFY(20,b0,"high != high, nop",printterm,name); \
    }

OPTCMP8TEST(signed_char,signed char,-20,5,60,100,sca,scb,"%d");
OPTCMP8TEST(unsigned_char,unsigned char,20,5,60,100,uca,ucb,"%u");
OPTCMP8TEST(signed_int,signed int,-2000,50,600,1000,sia,sib,"%d");
OPTCMP8TEST(unsigned_int,unsigned int,2000,50,600,1000,uia,uib,"%u");
OPTCMP8TEST(signed_long,signed long,-200000L,5000L,60000L,100000L,sla,slb,"%d");
OPTCMP8TEST(unsigned_long,unsigned long,200000UL,5000UL,60000UL,100000UL,ula,ulb,"%u");

TEST
{
    signed_char_test();
    unsigned_char_test();
    signed_int_test();
    unsigned_int_test();
    signed_long_test();
    unsigned_long_test();
}
ENDTEST
