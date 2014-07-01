/*
** _poserror.c
**
** Maciej 'YTM/Elysium' Witkowiak, 25.04.2003
*/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <geos.h>

void __fastcall__ _poserror (const char* msg)
{
    const char *errmsg = _stroserror(_oserror);

    ExitTurbo();
    if (msg && *msg) {
        DlgBoxOk(msg, errmsg);
    } else {
        DlgBoxOk("", errmsg);
    }
}
