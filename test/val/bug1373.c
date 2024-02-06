
/* #1373 - #pragma charmap works in unexpected ways */

#include <stdio.h>
#include <string.h>

char res0[10];
char res1[10];
char res2[10];
char res3[10];
char res4[10];

int err = 0;

#pragma charmap(0x61, 0x44)
#define STRING_A "abABa"

extern char mappedA[10];

#pragma charmap(0x61, 0x61)
char notmappedA[10] = "abABa";

void test(void);

#pragma charmap(0x61, 0x42)
int main(void)
{
    char mappedB[10] = STRING_A;
    sprintf(res0, "abABa");      /* expected: BbABB */

#pragma charmap(0x61, 0x61)
    sprintf(res1, mappedA);      /* expected: CbABC */
    sprintf(res2, STRING_A);     /* expected: abABa */
    sprintf(res3, mappedB);      /* expected: BbABB */

#pragma charmap(0x61, 0x43)
    sprintf(res4, notmappedA);   /* expected: abABa */

    test();

    return err;
}

char mappedA[10] = "abABa";

#pragma charmap(0x61, 0x61)
void test(void)
{
    puts(res0); if (strcmp(res0, "BbABB") != 0) { puts("expected: BbABB"); err++; }
    puts(res1); if (strcmp(res1, "CbABC") != 0) { puts("expected: CbABC"); err++; }
    puts(res2); if (strcmp(res2, "abABa") != 0) { puts("expected: abABa"); err++; }
    puts(res3); if (strcmp(res3, "BbABB") != 0) { puts("expected: BbABB"); err++; }
    puts(res4); if (strcmp(res4, "abABa") != 0) { puts("expected: abABa"); err++; }
}
