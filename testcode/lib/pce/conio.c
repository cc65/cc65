#include <conio.h>
#include <time.h>
#include <joystick.h>
#include <string.h>
#include <stdlib.h>

static int datavar = 10;

void main(void)
{
        int stackvar = 42;
        int i, j;
        clock_t clk;
        char* p;
        unsigned char xsize, ysize, n, nn;

        joy_install(&joy_static_stddrv);

        clrscr();
        screensize(&xsize, &ysize);

        cputs("hello world");
        cputsxy(0, 2, "colors:" );
        for (i = 0; i < 16; ++i) {
                textcolor(i);
                cputc('X');
        }
        textcolor(1);

        gotoxy(0,4);
        cprintf("datavar:  %02x\n\r", datavar);
        cprintf("stackvar: %02x\n\r", stackvar);

        j = joy_count();
        gotoxy(0,9);
        cprintf("Found %d Joysticks.", j);

        for (i = 0; i < 4; ++i) {
                gotoxy(0, 16 + i);
                p = malloc(16);
                memcpy(p, "0123456789abcdef", 16);
                cprintf("alloc'ed at: %04p - %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c", p,
                        p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7],
                        p[8],p[9],p[10],p[11],p[12],p[13],p[14],p[15]
                );
        }
        memcpy(p, main, 0);     /* test that a zero length doesn't copy 64K */

        gotoxy(0,ysize - 1);
        for (i = 0; i < xsize; ++i) {
                cputc('0' + i % 10);
        }

        gotoxy(0,ysize - 2 - ((256 + xsize) / xsize));
        for (i = 0; i < xsize; ++i) {
                cputc('0' + i % 10);
        }
        for (i = 0; i < (xsize * 5); ++i) {
                cputc('#');
        }
        gotoxy(0,ysize - 1 - ((256 + xsize) / xsize));
        for (i = 0; i < 256; ++i) {
                if ((i != '\n') && (i != '\r')) {
                        cputc(i);
                }
        }

        i = get_tv();
        gotoxy(30,0);
        cputs("TV Mode: ");
        switch(i) {
            case TV_NTSC:
                    cputs("NTSC");
                    break;
            case TV_PAL:
                    cputs("PAL");
                    break;
            case TV_OTHER:
                    cputs("OTHER");
                    break;
        }
        cprintf(" %dx%d", xsize, ysize);

        for(;;) {
                gotoxy(13,4);
                cprintf("%02x", datavar);
                gotoxy(13,5);
                cprintf("%02x", stackvar);
                ++datavar; ++stackvar;

                gotoxy(0,7);
                clk = clock();
                cprintf("clock: %08lx", clk);

                for (i = 0; i < 4; ++i) {
                        gotoxy(0, 11 + i);
                        j = joy_read (i);
                        cprintf ("pad %d: %02x %-6s%-6s%-6s%-6s%-6s%-6s%-6s%-6s",
                                i, j,
                                (j & joy_masks[JOY_UP])?    "  up  " : " ---- ",
                                (j & joy_masks[JOY_DOWN])?  " down " : " ---- ",
                                (j & joy_masks[JOY_LEFT])?  " left " : " ---- ",
                                (j & joy_masks[JOY_RIGHT])? "right " : " ---- ",
                                (j & joy_masks[JOY_FIRE])?  " fire " : " ---- ",
                                (j & joy_masks[JOY_FIRE2])? "fire2 " : " ---- ",
                                (j & joy_masks[JOY_SELECT])? "select" : " ---- ",
                                (j & joy_masks[JOY_RUN])?   " run  " : " ---- ");
                }

                gotoxy(xsize - 10, 3);
                nn = (n >> 5) & 1;
                revers(nn);
                cputc(nn ? 'R' : ' ');
                cputs(" revers");
                revers(0);

                if ((n & 0x1f) == 0x00) {
                        nn = p[15];
                        ((char*)memmove(p + 1, p, 15))[-1] = nn;
                        gotoxy(22, 19);
                        cprintf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",
                                p[0],p[1],p[ 2],p[ 3],p[ 4],p[ 5],p[ 6],p[ 7],
                                p[8],p[9],p[10],p[11],p[12],p[13],p[14],p[15]);
                }

                waitvblank();
                ++n;
        }
}
