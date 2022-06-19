
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

float _fneg(float f)
{
    return f * -1.0f;
}

float _fand(float f1, float f2)
{
    return ((unsigned)f1) & ((unsigned)f2);
}

char buffer[32];

char *_ftostr(char *d, float s)
{
    if (d == NULL) {
        d = &buffer[0];
    }
    sprintf(d, "%f", (double)s);
    return d;
}

char *_ftoa(char *d, float s) 
{
    if (d == NULL) {
        d = &buffer[0];
    }
    sprintf(d, "%f", (double)s);
    return d;
}

float _ctof(char c)
{
    return (float)c;
}

float _utof(unsigned int c)
{
    return (float)c;
}

float _stof(signed int c)
{
    return (float)c;
}
