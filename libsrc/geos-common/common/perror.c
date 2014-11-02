/*
** perror.c
**
** Maciej 'YTM/Elysium' Witkowiak, 15.07.2001
*/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <geos.h>

void __fastcall__ perror(const char* msg)
{
    const char *errmsg = strerror(errno);

    ExitTurbo();
    if (msg && *msg) {
        DlgBoxOk(msg, errmsg);
    } else {
        DlgBoxOk("", errmsg);
    }
}
