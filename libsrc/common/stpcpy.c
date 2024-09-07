#include <string.h>

char * __fastcall__ stpcpy (char * dst, const char * src)
{
    strcpy (dst, src);
    return dst + strlen (src);
}
