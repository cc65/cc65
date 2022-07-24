/*
 * conio API test program
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

#if defined(__GAMATE__)
/* there is not enough screen space to show all 256 characters at the bottom */
#define NUMCHARS        128
#define NUMCOLS           4
#else
#define NUMCHARS        256
#define NUMCOLS          16
#endif

static char grid[5][5] = {
    {CH_ULCORNER, CH_HLINE, CH_TTEE,  CH_HLINE, CH_URCORNER},
    {CH_VLINE,    ' ',      CH_VLINE, ' ',      CH_VLINE   },
    {CH_LTEE,     CH_HLINE, CH_CROSS, CH_HLINE, CH_RTEE    },
    {CH_VLINE,    ' ',      CH_VLINE, ' ',      CH_VLINE   },
    {CH_LLCORNER, CH_HLINE, CH_BTEE,  CH_HLINE, CH_LRCORNER}
};

void main(void)
{
        unsigned int i, j, n;
        unsigned char xsize, ysize, tcol, bgcol, bcol, inpos = 0;
#if defined(__NES__) || defined(__PCE__) || defined(__GAMATE__)
        unsigned char joy;

        joy_install(joy_static_stddrv);
#endif
        clrscr();
        screensize(&xsize, &ysize);
        cputs("cc65 conio test\n\r");
        cputs("Input:[        ]");      /* 8 spaces */

        tcol = textcolor(0);    /* memorize original textcolor */
        bgcol = bgcolor(0);     /* memorize original background color */
        bcol = bordercolor(0);  /* memorize original border color */
        (void)bordercolor(bcol);
        (void)bgcolor(bgcol);

        cputsxy(0, 2, "Colors:" );
        for (i = 0; i < 3; ++i) {
                gotoxy(i, 3 + i);
                for (j = 0; j < NUMCOLS; ++j) {
                        (void)textcolor(j);
                        cputc('X');
                }
        }
        (void)textcolor(tcol);

        cprintf("\n\n\r Screensize: %ux%u", xsize, ysize);

        chlinexy(0, 6, xsize);
        cvlinexy(0, 6, 3);
        chlinexy(0, 8, xsize);
        cvlinexy(xsize - 1, 6, 3);
        cputcxy(0, 6, CH_ULCORNER);
        cputcxy(xsize - 1, 6, CH_URCORNER);
        cputcxy(0, 8, CH_LLCORNER);
        cputcxy(xsize - 1, 8, CH_LRCORNER);

        for (i = 0; i < 5; ++i) {
                gotoxy(xsize - 5, i);
                for (j = 0; j < 5; ++j) {
                        cputc(grid[i][j]);
                }
        }

        gotoxy(0, ysize - 2 - ((NUMCHARS + xsize) / xsize));
        revers(1);
        for (i = 0; i < xsize; ++i) {
                cputc('0' + i % 10);
        }
        revers(0);
        for (i = 0; i < NUMCHARS; ++i) {
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
                /* do the "rvs" blinking */
                i = textcolor(COLOR_BLACK);
                gotoxy(8, 2);
                j = (++n / 16) & 1;
                revers(j);
                cputc(j ? 'R' : ' ');
                revers(j ^ 1);
                cputs(" rvs");
                revers(0);
                (void)textcolor(i);

                gotoxy(7 + inpos, 1);

#if defined(__NES__) || defined(__PCE__) || defined(__GAMATE__)
                /* not all targets have waitvsync */
                waitvsync();
                /* for targets that do not have a keyboard, read the first
                   joystick */
                joy = joy_read(JOY_1);
                cprintf("%02x", joy);
#else
                i = cgetc();
                switch (i) {
                    case CH_ENTER:
                        clrscr();
                        return;
                    case CH_CURS_LEFT:
                        inpos = (inpos - 1) % 8;
                        break;
                    case '0': case '1': case '2': case '3': case '4':
                    case '5': case '6': case '7': case '8': case '9':
                        (void)textcolor(i - '0');
                        break;
#ifdef CH_F5
                    case CH_F5:
                        bcol = (bcol + 1) & 0x0f;
                        (void)bordercolor(bcol);
                        break;
#endif
#ifdef CH_F6
                    case CH_F6:
                        bcol = (bcol - 1) & 0x0f;
                        (void)bordercolor(bcol);
                        break;
#endif
#ifdef CH_F7
                    case CH_F7:
                        bgcol = (bgcol + 1) & 0x0f;
                        (void)bgcolor(bgcol);
                        break;
#endif
#ifdef CH_F8
                    case CH_F8:
                        bgcol = (bgcol - 1) & 0x0f;
                        (void)bgcolor(bgcol);
                        break;
#endif
                    default:
                        cputc(i);
                        /* fallthrough */
                    case CH_CURS_RIGHT:
                        inpos = (inpos + 1) % 8;
                }
#endif
        }
}
