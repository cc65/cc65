/* bug #1348, wrongly optimized integer promotion in comparison */

#include <stdio.h>

static const int notrandtab[] = {
    0xffff, 0x7fff, 0x3fff, 0x1fff,
    0x0fff, 0x07ff, 0x03ff, 0x01ff,
    0x00ff, 0x007f, 0x003f, 0x001f,
    0x000f, 0x0007, 0x0003, 0x0001
};

static unsigned char notrandcount = 0;

static int notrand(void)
{
    return notrandtab[notrandcount & 0x0f];
}

static unsigned char n1, n2;
static unsigned char i, ii, s;
static unsigned char err = 0;

static const unsigned char cmptab[] = {
    0xff, 0x7f, 0x3f, 0x1f,
    0x0f, 0x07, 0x03, 0x01,
    0x80, 0x40, 0x20, 0x10,
    0x08, 0x04, 0x02, 0x01
};

int main(void)
{
    for (ii = 0; ii < 16; ++ii) {
        s = cmptab[ii];
        for (i = 0; i < 16; ++i) {
            n1 = n2 = 0;
            if ((notrand() & 0xff) > s) {
                n1 = 1;
            }
            if ((notrand() & 0xffu) > s) {
                n2 = 1;
            }
            printf("%5d > %3d %u(%02x) %u(%02x)  %s\n",
                   notrandtab[i], s,
                   n1, (notrand() & 0xff),
                   n2, (notrand() & 0xffu),
                   n1 == n2 ? "=" : "!=");
            if (n1 != n2) {
                err = 1;
            }
            notrandcount++;
        }
    }
    return err;
}
