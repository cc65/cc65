/*
** testprogram for includes "atari_screen_charmap.h" and "atari_atascii_charmap.h"
**
** 19-Aug-2016, Christian Krueger
*/

#include <conio.h>
#include <atari.h>
#include <peekpoke.h>
#include <string.h>


char pcDefaultMappingString[] = "Hello Atari!";

#include <atari_screen_charmap.h>
char pcScreenMappingString[] = "Hello Atari!";

#include <atari_atascii_charmap.h>
char pcAtasciiMappingString[] = "Hello Atari!";

/* THIS WON'T work due to string merging/collection problems!
char* pcDefaultMappingString = "Hello Atari!";

#include <atari_screen_charmap.h>
char* pcScreenMappingString = "Hello Atari!";

#include <atari_atascii_charmap.h>
char* pcAtasciiMappingString = "Hello Atari!";
*/

int
main(void)
{
    static unsigned char expectedAtasciiValues[] = { 40,101,108,108,111,0,33,116,97,114,105,1};

    int returnValue = 0;
    unsigned char* screen = (unsigned char*) PEEKW(88);

    // check default (=atascii)
    clrscr();
    cputs(pcDefaultMappingString);
    returnValue |= memcmp(screen, expectedAtasciiValues, sizeof(expectedAtasciiValues));

    clrscr();
    memcpy(screen, pcScreenMappingString, sizeof(expectedAtasciiValues));
    returnValue |= memcmp(screen, expectedAtasciiValues, sizeof(expectedAtasciiValues));

    clrscr();
    cputs(pcAtasciiMappingString);
    returnValue |= memcmp(screen, expectedAtasciiValues, sizeof(expectedAtasciiValues));

    clrscr();
    if (returnValue)
        cputs("Test FAILED!");
    else
        cputs("Test passed.");

    cputs("\n\rHit any key to exit...");
    cgetc();

    return returnValue;
}

