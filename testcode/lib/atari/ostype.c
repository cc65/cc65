/*
** testprogram for get_ostype() and get_tv() functions
**
** 09-Jul-2004, chris@groessler.org
*/

#include <stdio.h>
#include <atari.h>

int main(void)
{
    unsigned int t, v;
    unsigned char palntsc;
    unsigned char *rev;
    unsigned char minor;
    unsigned char c;

    t = get_ostype();    /* get computer type */
    v = get_tv();        /* get tv system */

    palntsc = (v == AT_PAL);

    minor = (t & AT_OS_TYPE_MINOR) >> 5;
    switch(t & AT_OS_TYPE_MAIN) {
        case AT_OS_UNKNOWN:
        default:
            printf("unknown system type !!\n");
            break;
        case AT_OS_400800:
            if (minor == 1) rev = "A";
            else rev = "B";
            printf("it's a 400/800, %s, Rev. %s\n",palntsc ? "PAL" : "NTSC",rev);
            break;
        case AT_OS_1200XL:
            if (minor == 1) rev = "10";
            else rev = "11";
            printf("it's a 1200XL, %s, Rev. %s\n",palntsc ? "PAL" : "NTSC",rev);
            break;
        case AT_OS_XLXE:
            printf("is'a a XL/XE, %s, Rev. %d\n",palntsc ? "PAL" : "NTSC",minor);
            break;
    }
    printf("hit <RETURN> to continue...\n");
    c = getchar();
    return 0;
}
