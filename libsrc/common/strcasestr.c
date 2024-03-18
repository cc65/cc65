/*
** strcasestr.c
**
** Colin Leroy-Mira, 2024
*/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/



char* __fastcall__ strcasestr(const char *str, const char *substr) {
  size_t len_a = strlen(str);
  size_t len_b = strlen(substr);
  const char *end_str;

  if (len_a < len_b)
    return NULL;

  len_a -= len_b;

  for (end_str = str + len_a + 1; str < end_str; str++) {
    if (!strncasecmp(str, substr, len_b))
      return (char *)str;
  }
  return NULL;
}
