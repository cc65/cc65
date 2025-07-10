/* bug #1643, macro expansion in #include */

#define STDIO_H     <stdio.h>
#include STDIO_H

#ifdef string
#undef string
#endif

#define string      0!%^&*/_=
#include <string.h>

#define BUG1643_RESULT 0
