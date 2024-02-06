/* Example illustrating scandisplay() and getkey() functions. */

#include <stdio.h>
#include <kim1.h>

int main (void)
{
    int i, j, k, l;
    int last = 15;

    printf("\nKIM-1 Demo\n");

    for (i = 0; i < 16; i++) {
        for (j = 0; j < 16; j++) {
            for (k = 0; k < 16; k++) {
                scandisplay(i, j, k);

                l = getkey();

                if (l != last) {
                    switch (l) {
                    case 0x0: case 0x1: case 0x2: case 0x3:
                    case 0x4: case 0x5: case 0x6: case 0x7:
                    case 0x8: case 0x9: case 0xa: case 0xb:
                    case 0xc: case 0xd: case 0xe: case 0xf:
                        printf("Key pressed: %X\n", l);
                        break;
                    case 0x10:
                        printf("Key pressed: AD\n");
                        break;
                    case 0x11:
                        printf("Key pressed: DA\n");
                        break;
                    case 0x12:
                        printf("Key pressed: +\n");
                        break;
                    case 0x13:
                        printf("Key pressed: GO\n");
                        break;
                    case 0x14:
                        printf("Key pressed: PC\n");
                        break;
                    }

                    last = l;
                }
            }
        }
    }

    return 0;
}
