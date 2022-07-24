
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
#define STRING_A "abAB"

extern char mappedA[5];

#pragma charmap(0x61, 0x61)
char notmappedA[5] = "abAB";

#pragma charmap(0x61, 0x42)
int main(void)
{
    char mappedB[5] = STRING_A;
    sprintf(res0, "abAB");       /* expected: BbAB */

#pragma charmap(0x61, 0x61)
    sprintf(res1, mappedA);      /* expected: CbAB */
    sprintf(res2, STRING_A);     /* expected: abAB */
    sprintf(res3, mappedB);      /* expected: BBaB */

#pragma charmap(0x61, 0x43)
    sprintf(res4, notmappedA);   /* expected: abAB */

#pragma charmap(0x61, 0x61)
    puts(res0); if (strcmp(res0, "BbAB") != 0) { puts("expected: BbAB"); err++; }
    puts(res1); if (strcmp(res1, "CbAB") != 0) { puts("expected: CbAB"); err++; }
    puts(res2); if (strcmp(res2, "abAB") != 0) { puts("expected: abAB"); err++; }
    puts(res3); if (strcmp(res3, "BBaB") != 0) { puts("expected: BBaB"); err++; }
    puts(res4); if (strcmp(res4, "abAB") != 0) { puts("expected: abAB"); err++; }

    return err;
}

char mappedA[5] = "abAB"; 
