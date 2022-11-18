#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define XRES 30

float x1, x2, x3;
float y1, y2, y3;
int j, i;
int xpos1, xpos2, xpos3;

#define STEP1   ((M_PI*2) / 15.0f)
#define STEP2   ((M_PI*2) / 17.0f)
//#define STEP3   0.0001f
#define STEP3   0.025f

char buf[100];

unsigned char pix;
              //01234567
char *colors = ".oO#%*=-";

int main(void)
{
    y1 = 0;
    y2 = 0;
    y3 = 0;

    while (1) {

        y1 = y1 + STEP1;
        y2 = y2 + STEP2;
        y3 = y3 + STEP3;

        x1 = (sinf(y1) * (XRES / 2.0f)) + (XRES / 2.0f);
        x2 = (cosf(y2) * (XRES / 2.0f)) + (XRES / 2.0f);
        x3 = tanf(y3) + (XRES / 2.0f);

        xpos1 = x1;
        xpos2 = x2;
        xpos3 = x3;
//        printf("%s\n", _ftostr(buf, xpos2)); continue;
//        printf("%s\n", _ftostr(buf, xpos3)); continue;

        for (i = 0; i < XRES; i++) {
            pix = 0;
            if (i >= xpos1) {
                pix |= 1;
            }
            if (i <= xpos2) {
                pix |= 2;
            }
            if ((i == xpos3) || (i == xpos3 - 1) || (i == xpos3 + 1)) {
                pix |= 4;
            }
            putchar(colors[pix]);
        }
        putchar('\n');
    }
}
