#include <stdio.h>
#include <string.h>
#include <conio.h>


int main (void)
{
    unsigned char error = 0;
    unsigned char line = 0;
    unsigned char maxx, maxy;
    screensize (&maxx, &maxy);
    do {
        printf ("%2d: %s\n", error, _stroserror (error));
        if (line == maxy-3) {
            printf ("Press any key...\n");
            if (cgetc () == 'q') {
                return 0;
            }
            clrscr ();
            line = 0;
        } else {
            ++line;
        }
        ++error;
    } while (error != 0);

    return 0;
}



