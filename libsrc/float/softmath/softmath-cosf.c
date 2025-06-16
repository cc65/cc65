
/* nicked from https://github.com/AZHenley/cosine/blob/master/cosine.c */

/*
MIT License

Copyright (c) 2020 Austin Henley

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <math.h>

#define RUNTERMS    8
float cosf(float x)
{
    int i;
    int div;
    float num;
    float result = 1.0f;
    float inter = 1.0f;
    float comp;
    float den;

    div = (int)(x / M_PI);

    x = x - (div * M_PI);

    num = x * x;
    for (i = 1; i <= RUNTERMS; i++) {
        comp = 2.0f * i;
        den = comp * (comp - 1.0f);
        inter *= num / den;
        if (i % 2 == 0) {
            result += inter;
        } else {
            result -= inter;
        }
    }

    if (div % 2 != 0) {
        return result * -1.0f;
    }
    return result;
}

#if 0
float cosf(float x) {
    return sinf(x + (M_PI / 2.0));
}
#endif
