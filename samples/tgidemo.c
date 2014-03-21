#include <stdio.h>
#include <cc65.h>
#include <conio.h>
#include <ctype.h>
#include <modload.h>
#include <tgi/tgi-kernel.h>

#include <stdlib.h>
#include <unistd.h>
#include <tgi.h>

static void showinfo()
{
    char buf[6];
    unsigned char x, y, nextline;

    tgi_clear();
    tgi_setcolor(TGI_COLOR_WHITE);
    x = 10;
    y = 10;

    // Find lineheight
    if (tgi_gettextheight("0") > 6) {
        nextline = tgi_gettextheight("0") + 2;
    } else {
        nextline = 8 + 2;
    }

    // Number of pixels
    tgi_outtextxy(x, y, "Screen ");
    utoa(tgi_getxres(), buf, 10);
    tgi_outtext(buf);
    tgi_outtext("x");
    utoa(tgi_getyres(), buf, 10);
    tgi_outtext(buf);
    y += nextline;

    tgi_outtextxy(x, y, "Index  ");
    utoa(tgi_getmaxx(), buf, 10);
    tgi_outtext(buf);
    tgi_outtext(" ");
    utoa(tgi_getmaxy(), buf, 10);
    tgi_outtext(buf);
    y += nextline;

    // Text properties
    tgi_outtextxy(x, y, "Text   ");
    utoa(tgi_gettextwidth("0"), buf, 10);
    tgi_outtext(buf);
    tgi_outtext("x");
    utoa(tgi_gettextheight("0"), buf, 10);
    tgi_outtext(buf);
    y += nextline;

    // Colors
    tgi_outtextxy(x, y, "Colors ");
    utoa(tgi_getcolorcount(), buf, 10);
    tgi_outtext(buf);
    sleep(5);


}

static void showbar()
{
    tgi_clear();
    tgi_setcolor(TGI_COLOR_WHITE);
    tgi_bar(10, 10, tgi_getmaxx()-10, tgi_getmaxy()-10);
    tgi_outtextxy(2, 1, "Bar");
    sleep(5);
}

static void showcircle()
{
    tgi_clear();
    tgi_setcolor(TGI_COLOR_WHITE);
    tgi_circle(tgi_getxres() / 2, tgi_getyres() / 2, tgi_getxres() < tgi_getyres() ? tgi_getxres() / 2 - 10 : tgi_getyres() / 2 - 10);
    tgi_outtextxy(2, 1, "Circle");
    sleep(5);
}

static void showellipse()
{
    tgi_clear();
    tgi_setcolor(TGI_COLOR_WHITE);
    tgi_ellipse(tgi_getxres() / 2, tgi_getyres() / 2, tgi_getxres() / 2 - 10, tgi_getyres() / 2 - 10);
    tgi_outtextxy(2, 1, "Ellipse");
    sleep(5);
}

static void showline()
{
    tgi_clear();
    tgi_setcolor(TGI_COLOR_WHITE);
    tgi_line(10, tgi_getyres() - 10, tgi_getxres() - 10, 10);
    tgi_outtextxy(2, 1, "Line");
    sleep(5);
}

static void showpixel()
{
    tgi_clear();
    tgi_setcolor(TGI_COLOR_WHITE);
    tgi_setpixel(tgi_getxres() / 2, tgi_getyres() / 2);
    tgi_outtextxy(2, 1, "Pixel");
    sleep(5);
}

static void showarc()
{
    tgi_clear();
    tgi_setcolor(TGI_COLOR_WHITE);
    tgi_arc(tgi_getxres() / 2, tgi_getyres() / 2, tgi_getxres() / 2 - 10, tgi_getyres() / 2 - 10, 10, 70);
    tgi_outtextxy(2, 1, "Arc");
    sleep(5);
}

static void showpie()
{
    tgi_clear();
    tgi_setcolor(TGI_COLOR_WHITE);
    tgi_pieslice(tgi_getxres() / 2, tgi_getyres() / 2, tgi_getxres() / 2 - 10, tgi_getyres() / 2 - 10, 10, 70);
    tgi_outtextxy(2, 1, "Pie");
    sleep(5);
}

static void showpalette()
{
    unsigned char i;
    tgi_clear();
    for (i = 0; i < tgi_getcolorcount(); i++) {
        unsigned char x, y;
        tgi_setcolor(i);
        x = (i % 8) * 10 + 10;
        y = (i / 8) * 10 + 10;
        tgi_bar(x, y, x + 9, y + 9);
    }
    tgi_setcolor(TGI_COLOR_WHITE);
    tgi_outtextxy(2, 1, "Palette");
    sleep(5);
}

static void randomcircles(void)
{
    unsigned char i;

    tgi_clear ();
    for (i = 0; i < 10; i++) {
        tgi_setcolor(rand() % tgi_getcolorcount());
        tgi_circle(
            rand() % (tgi_getxres() / 3) + tgi_getxres() / 3,
            rand() % (tgi_getyres() / 3) + tgi_getyres() / 3,
            rand() % (tgi_getxres() / 6));
    }
    tgi_setcolor(TGI_COLOR_WHITE);
    tgi_outtextxy(2, 1, "Random circles");
    sleep(5);
}

static void randomboxes(void)
{
    unsigned char i;

    tgi_clear ();
    for (i = 0; i < 10; i++) {
        unsigned char x, y;
        tgi_setcolor(rand() % tgi_getcolorcount());
        x = rand() % (tgi_getxres() / 2) + 10;
        y = rand() % (tgi_getyres() / 2) + 10;
        tgi_bar(
            x,
            y,
            x + rand() % (tgi_getxres() / 4),
            y + rand() % (tgi_getyres() / 4));
    }
    tgi_setcolor(TGI_COLOR_WHITE);
    tgi_outtextxy(2, 1, "Random boxes");
    sleep(5);
}

static void checkerboard(void)
{
    unsigned char x, y;

    tgi_clear ();
    for (y = 10; y < tgi_getmaxy() - 20; y += 10) {
        for (x = 10; x < tgi_getmaxx() - 20; x += 10) {
            tgi_setcolor(((x/10) & 1) ^ ((y/10) & 1) ? TGI_COLOR_BLACK : TGI_COLOR_WHITE);
            tgi_bar(x, y, x + 9, y + 9);
        }
    }
    tgi_setcolor(TGI_COLOR_WHITE);
    tgi_outtextxy(2, 1, "Checkerboard");
    sleep(5);
}

static void diagram()
{
    int XOrigin, YOrigin;
    int Amp;
    int X, Y;
    unsigned I;

    tgi_clear ();
    tgi_setcolor(TGI_COLOR_WHITE);

    /* Determine zero and aplitude */
    YOrigin = (tgi_getmaxy() - 19) / 2 + 10;
    XOrigin = 10;
    Amp     = (tgi_getmaxy() - 19) / 2;

    /* Y axis */
    tgi_line (XOrigin, 10, XOrigin, tgi_getmaxy()-10);
    tgi_line (XOrigin-2, 12, XOrigin, 10);
    tgi_lineto (XOrigin+2, 12);

    /* X axis */
    tgi_line (XOrigin, YOrigin, tgi_getmaxx()-10, YOrigin);
    tgi_line (tgi_getmaxx()-12, YOrigin-2, tgi_getmaxx()-10, YOrigin);
    tgi_lineto (tgi_getmaxx()-12, YOrigin+2);

    /* Sine */
    tgi_gotoxy (XOrigin, YOrigin);
    for (I = 0; I <= 360; I += 5) {

        /* Calculate the next points */
        X = (int) (((long) (tgi_getmaxx() - 19) * I) / 360);
        Y = (int) (((long) Amp * -cc65_sin (I)) / 256);

        /* Draw the line */
        tgi_lineto (XOrigin + X, YOrigin + Y);
    }
    tgi_setcolor(TGI_COLOR_WHITE);
    tgi_outtextxy(2, 1, "Diagram");
    sleep(5);
}

int main (void)
{
    /* Install the driver */
    tgi_install(tgi_static_stddrv);
    if (tgi_geterror() != TGI_ERR_OK) {
        return EXIT_FAILURE;
    }

    /* Initialize the driver and clear the screen */
    tgi_init();
    if (tgi_geterror() != TGI_ERR_OK) {
        return EXIT_FAILURE;
    }

    showinfo();

    showbar();

    showcircle();

    showellipse();

    showline();

    showpixel();

    showarc();

    showpie();

    showpalette();

    randomcircles();

    randomboxes();

    checkerboard();

    diagram();

    /* Uninstall the driver */
    tgi_uninstall ();

    /* Done */
    return EXIT_SUCCESS;
}
