
#include <stdlib.h>
#include <stdio.h>

char *_ftostr(char *buffer, float f) {
    signed long intpart = (signed long)f;
    float fracpart;
    fracpart = f - (float)(intpart);
    sprintf(buffer, "<0x%08lx:%ld.%ld>", *((unsigned long*)(&f)), intpart, fracpart * 1000);
    return &buffer[0];
}
