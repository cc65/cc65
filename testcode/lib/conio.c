/*
 * conio api test program
 *
 * keys:
 *
 * 1...0        change text color
 * F5/F6        change border color
 * F7/F8        change background color
 *
 */


#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <joystick.h>

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
        unsigned char xsize, ysize, tcol, bgcol, bcol, inpos = 0;

        clrscr();
        screensize(&xsize, &ysize);
        cputs("cc65 conio test\n\rInput: [        ]");

        cputsxy(0, 2, "Colors:" );
        tcol = textcolor(0); /* remember original textcolor */
        bgcol = bgcolor(0); /* remember original background color */
        bcol = bordercolor(0); /* remember original border color */
        bgcolor(bgcol);bordercolor(bcol);
        for (i = 0; i < 3; ++i) {
                gotoxy(i,3 + i);
                for (j = 0; j < 16; ++j) {
                        textcolor(j);
                        cputc('X');
                }
        }
        textcolor(tcol);

        cprintf("\n\n\r Screensize is: %dx%d", xsize, ysize);

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
            } else {
                    cputc(' ');
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

        cursor(1);
        for (;;) {

                gotoxy(8, 2);
                j = n & 1;
                revers(j);
                cputc(j ? 'R' : ' ');
                revers(j ^ 1);
                cputs(" revers");
                revers(0);

#if defined(__NES__) || defined(__PCE__)

                joy_install(joy_static_stddrv);
                while (!joy_read(JOY_1)) ;
                joy_uninstall();

#else

                gotoxy(8 + inpos,1);
                i = cgetc();
                if ((i >= '0') && (i<='9')) {
                    textcolor(i - '0');
                } else if (i == CH_CURS_LEFT) {
                    inpos = (inpos - 1) & 7;
                } else if (i == CH_CURS_RIGHT) {
                    inpos = (inpos + 1) & 7;
                } else if (i == CH_F5) {
                    bgcol = (bgcol + 1) & 0x0f;
                    bordercolor(bgcol);
                } else if (i == CH_F6) {
                    bgcol = (bgcol - 1) & 0x0f;
                    bordercolor(bgcol);
                } else if (i == CH_F7) {
                    bgcol = (bgcol + 1) & 0x0f;
                    bgcolor(bgcol);
                } else if (i == CH_F8) {
                    bgcol = (bgcol - 1) & 0x0f;
                    bgcolor(bgcol);
                } else {
                    cputc(i);
                    inpos = (inpos + 1) & 7;
                }

#endif

                ++n;
        }
}
