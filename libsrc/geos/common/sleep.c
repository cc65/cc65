/*
 * sleep.c
 *
 * Maciej 'YTM/Elysium' Witkowiak, 15.08.2003
 *
 */

#include <geos.h>

unsigned __fastcall__ sleep (unsigned wait)
{
    char typ;

    if ( (get_tv()) & TV_NTSC ) {
	typ = 60;
    } else {
	typ = 50;
    }

    Sleep(wait*typ);

    return 0;
}
