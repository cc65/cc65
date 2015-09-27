
#include <conio.h>
#include <string.h>
#include <stdlib.h>

void main(void)
{
        int i, j;
        unsigned char xsize, ysize, n;

        clrscr();
        screensize(&xsize, &ysize);

        cputs("cc65 conio test");
        cputsxy(0, 2, "colors:" );
        for (i = 3; i < 6; ++i) {
                gotoxy(i,i);
                for (j = 0; j < 16; ++j) {
                        textcolor(j);
                        cputc('X');
                }
        }
        textcolor(1);

        cprintf("\n\n\rscreensize is: %dx%d", xsize, ysize );

        chlinexy(0,10,xsize);
        cvlinexy(0,10,3);
        chlinexy(0,12,xsize);
        cvlinexy(xsize-1,10,3);
        cputcxy(0,10,CH_ULCORNER);
        cputcxy(xsize-1,10,CH_URCORNER);
        cputcxy(0,12,CH_LLCORNER);
        cputcxy(xsize-1,12,CH_LRCORNER);

        gotoxy(0,ysize - 2 - ((256 + xsize) / xsize));
        for (i = 0; i < xsize; ++i) {
                cputc('0' + i % 10);
        }
        for (i = 0; i < 256; ++i) {
            if ((i != '\n') && (i != '\r')) {
                    cputc(i);
            }
        }
        while(wherex() > 0) {
                cputc('#');
        }
        for (i = 0; i < xsize; ++i) {
                cputc('0' + i % 10);
        }

        for(;;) {

                gotoxy(xsize - 10, 3);
                j = (n >> 5) & 1;
                revers(j);
                cputc(j ? 'R' : ' ');
                cputs(" revers");
                revers(0);

                ++n;
        }

        for(;;);
}
