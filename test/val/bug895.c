/** This test is related to GitHub issue 895
 ** https://github.com/cc65/cc65/issues/895
 **
 ** The OptCmp8 optimization attempts to eliminate an unnecessary
 ** comparison and branch when the operands of the comparison are
 ** known to be constant at compile time.
 **
 ** For 8-bit types it worked well, but for larger types it failed
 ** to generate correct code. The bug manifest as a branch on an
 ** uninitialized carry flag.
 **
 ** This does four tests for each type tested:
 **   1: < with carry clear
 **   2: >= with carry clear
 **   3: < with carry set
 **   4: >= with carry set
 */

#include "unittest.h"

signed char   sca, scb;
signed int    sia, sib;
signed long   sla, slb;

unsigned char uca, ucb;
unsigned int  uia, uib;
unsigned long ula, ulb;

#define OPTCMP8TEST(vara,varb,startb,starta,cmpa,setb,printterm,typename,name) \
    void name ## 1(void) { \
    varb = startb; \
    asm("clc"); \
    vara = starta; \
    if (vara < cmpa) varb = setb; \
    ASSERT_AreEqual(startb, varb, printterm, "Incorrect optimization of " typename " comparison (1: < clc)."); \
    } \
    void name ## 2(void) { \
    varb = startb; \
    asm("sec"); \
    vara = starta; \
    if (vara < cmpa) varb = setb; \
    ASSERT_AreEqual(startb, varb, printterm, "Incorrect optimization of " typename " comparison (2: < sec)."); \
    } \
    void name ## 3(void) { \
    varb = startb; \
    asm("clc"); \
    vara = starta; \
    if (vara >= cmpa) varb = setb; \
    ASSERT_AreEqual(setb, varb, printterm, "Incorrect optimization of " typename " comparison (3: >= clc)."); \
    } \
    void name ## 4(void) { \
    varb = startb; \
    asm("sec"); \
    vara = starta; \
    if (vara >= cmpa) varb = setb; \
    ASSERT_AreEqual(setb, varb, printterm, "Incorrect optimization of " typename " comparison (4: >= sec)."); \
    }

#define RUNOPTCMP8TEST(name) \
    name ## 1(); \
    name ## 2(); \
    name ## 3(); \
    name ## 4();

OPTCMP8TEST(sca,scb,-20,100,50,5,"%d","signed char",signed_char);
OPTCMP8TEST(uca,ucb,20,100,50,5,"%u","unsigned char",unsigned_char);
OPTCMP8TEST(sia,sib,-2000,1000,500,50,"%d","signed int",signed_int);
OPTCMP8TEST(uia,uib,2000,1000,500,50,"%u","unsigned int",unsigned_int);
OPTCMP8TEST(sla,slb,-200000L,100000L,50000L,5000L,"%d","signed long",signed_long);
OPTCMP8TEST(ula,ulb,200000UL,100000UL,50000UL,5000UL,"%u","unsigned long",unsigned_long);

TEST
{
    RUNOPTCMP8TEST(signed_char);
    RUNOPTCMP8TEST(unsigned_char);
    RUNOPTCMP8TEST(signed_int);
    RUNOPTCMP8TEST(unsigned_int);
    RUNOPTCMP8TEST(signed_long);
    RUNOPTCMP8TEST(unsigned_long);
}
ENDTEST
