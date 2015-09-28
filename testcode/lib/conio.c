
#include <conio.h>
#include <string.h>
#include <stdlib.h>


static char grid[5][5] = {
    { CH_ULCORNER, CH_HLINE, CH_TTEE, CH_HLINE, CH_URCORNER },
    { CH_VLINE, ' ', CH_VLINE, ' ', CH_VLINE },
    { CH_LTEE, CH_HLINE, CH_CROSS, CH_HLINE, CH_RTEE },
    { CH_VLINE, ' ', CH_VLINE, ' ', CH_VLINE },
    { CH_LLCORNER, CH_HLINE, CH_BTEE, CH_HLINE, CH_LRCORNER },
};

void main(void)
{
        int i, j, n;
        unsigned char xsize, ysize, tcol;

        clrscr();
        screensize(&xsize, &ysize);
        cputs("cc65 conio test");

        cputsxy(0, 2, "Colors:" );
        tcol = textcolor(0); /* remember original textcolor */
        for (i = 0; i < 3; ++i) {
                gotoxy(i,3 + i);
                for (j = 0; j < 16; ++j) {
                        textcolor(j);
                        cputc('X');
                }
        }
        textcolor(tcol);

        cprintf("\n\n\r Screensize is: %dx%d", xsize, ysize );

        chlinexy(0,6,xsize);
        cvlinexy(0,6,3);
        chlinexy(0,8,xsize);
        cvlinexy(xsize-1,6,3);
        cputcxy(0,6,CH_ULCORNER);
        cputcxy(xsize-1,6,CH_URCORNER);
        cputcxy(0,8,CH_LLCORNER);
        cputcxy(xsize-1,8,CH_LRCORNER);

        for (i = 0; i < 5; ++i) {
                gotoxy(xsize - 5,i);
                for (j = 0; j < 5; ++j) {
                        cputc(grid[i][j]);
                }
        }

        gotoxy(0,ysize - 2 - ((256 + xsize) / xsize));
        revers(1);
        for (i = 0; i < xsize; ++i) {
                cputc('0' + i % 10);
        }
        revers(0);
        for (i = 0; i < 256; ++i) {
            if ((i != '\n') && (i != '\r')) {
                    cputc(i);
            }
        }
        while(wherex() > 0) {
                cputc('#');
        }
        revers(1);
        for (i = 0; i < xsize; ++i) {
                cputc('0' + i % 10);
        }
        revers(0);

        for(;;) {

                gotoxy(8, 2);
                j = (n >> 5) & 1;
                revers(j);
                cputc(j ? 'R' : ' ');
                cputs(" revers");
                revers(0);

                ++n;
        }

        for(;;);
}
