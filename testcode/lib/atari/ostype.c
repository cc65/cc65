/*
 * testprogram for get_ostype() function
 *
 * 17-Jul-2000, cpg@aladdin.de
 */

#include <stdio.h>
#include <atari.h>

int main(void)
{
    unsigned int t;
    unsigned char palntsc;
    unsigned char *rev;
    unsigned char minor;
    unsigned char c;

    t = get_ostype();    /* get computer type */

    palntsc = (t & AT_OS_PALNTSC) >> 3;
    minor = (t & AT_OS_TYPE_MINOR) >> 5;
    if (palntsc != AT_OS_PAL) palntsc = 0; /* 1 - PAL; 0 - NTSC */
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
    printf("hit <RETURN> to continure...\n");
    c = getchar();
}
