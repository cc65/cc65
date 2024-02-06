#include <geos.h>
#include <conio.h>

// Let's define the window we're operating
struct window wholeScreen = {0, SC_PIX_HEIGHT-1, 0, SC_PIX_WIDTH-1};


void main (void)
{
    unsigned char os = get_ostype();
    char *machine = NULL;
    char *version = NULL;
    unsigned char good = 1;

    SetPattern(0);
    InitDrawWindow(&wholeScreen);
    Rectangle();
    gotoxy(0, 4);
    if (os == GEOS4) {
        machine = "plus4";
        version = "GEOS v3.5";
    } else {
        if ((os & GEOS128) == GEOS128) {
            machine = "c128";
        } else {
            machine = "c64";
        }
        os &= 0x7f;
        if (os == GEOS_V10) {
            version = "GEOS v1.0";
        } else if (os == GEOS_V11) {
            version = "GEOS v1.1";
        } else if (os == GEOS_V12) {
            version = "GEOS v1.2";
        } else if (os == GEOS_V13) {
            version = "GEOS v1.3";
        } else if (os == GEOS_V15) {
            version = "GEOS v1.5";
        } else if (os == GEOS_V20) {
            version = "GEOS v2.0";
        } else if (os == MEGAPATCH3) {
            version = "MegaPatch 3";
        } else if (os == GATEWAY) {
            version = "GateWay";
        } else if ((os & WHEELS) == WHEELS) {
            version = "Wheels";
        } else {
            version = "Unknown GEOS version";
            good = 0;
        }
    }

    if (good) {
        cprintf("%s (%s)", version, machine);
    } else {
        cprintf("%s (%s) (%d)", version, machine, os);
    }

    Sleep(10*50);

    return;
}
