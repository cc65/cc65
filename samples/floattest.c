#include <stdio.h>
#include <stdlib.h>

#include <_float.h>

char buf[100];

// quick check to see if printing float value works
void constants(void)
{
    printf("-100.0      %15s\n", _ftostr(buf, -100.0f));
    printf(" -10.0      %15s\n", _ftostr(buf, -10.0f));
    printf("  -1.0      %15s\n", _ftostr(buf, -1.0f));
    printf("  -0.1      %15s\n", _ftostr(buf, -0.1f));
    printf("  -0.01     %15s\n", _ftostr(buf, -0.01f));
    printf("  -0.001    %15s\n", _ftostr(buf, -0.001f));
    printf("  -0.0001   %15s\n", _ftostr(buf, -0.0001f));
    printf("  -0.00001  %15s\n", _ftostr(buf, -0.00001f));
    printf("   0.0      %15s\n", _ftostr(buf, 0.0f));
    printf("   0.1      %15s\n", _ftostr(buf, 0.1f));
    printf("   0.01     %15s\n", _ftostr(buf, 0.01f));
    printf("   0.001    %15s\n", _ftostr(buf, 0.001f));
    printf("   0.0001   %15s\n", _ftostr(buf, 0.0001f));
    printf("   0.00001  %15s\n", _ftostr(buf, 0.00001f));
    printf("   1.0      %15s\n", _ftostr(buf, 1.0f));
    printf("  10.0      %15s\n", _ftostr(buf, 10.0f));
    printf(" 100.0      %15s\n", _ftostr(buf, 100.0f));
    printf("<key>\n");
//    cgetc();
}

int main(void)
{
    constants();
    return 0;
}
