/*
 * perror.c
 *
 * Maciej 'YTM/Elysium' Witkowiak, 15.07.2001
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <geos.h>

void perror(const char* msg)
{

    DlgBoxOk((char*)msg,strerror(errno));

}
