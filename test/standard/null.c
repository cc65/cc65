

/* test headers which should define NULL */

#include <locale.h>
#ifndef NULL
#error "NULL should be defined in locale.h"
#endif
#undef NULL

#include <stdlib.h>
#ifndef NULL
#error "NULL should be defined in stdlib.h"
#endif
#undef NULL

#include <string.h>
#ifndef NULL
#error "NULL should be defined in string.h"
#endif
#undef NULL

#include <stddef.h>
#ifndef NULL
#error "NULL should be defined in stddef.h"
#endif
#undef NULL

#include <stdio.h>
#ifndef NULL
#error "NULL should be defined in stdio.h"
#endif
#undef NULL

#include <time.h>
#ifndef NULL
#error "NULL should be defined in time.h"
#endif
#undef NULL

/* does not exist in cc65 (yet)
#include <wchar.h>
#ifndef NULL
#error "NULL should be defined in wchar.h"
#endif */
#undef NULL


/* test headers which should NOT define NULL */

#include <assert.h>
#ifdef NULL
#error "NULL should NOT be defined in assert.h"
#undef NULL
#endif

/* does not exist in cc65 (yet)
#include <complex.h>
#ifdef NULL
#error "NULL should NOT be defined in complex.h"
#undef NULL
#endif */

#include <ctype.h>
#ifdef NULL
#error "NULL should NOT be defined in ctype.h"
#undef NULL
#endif

#include <errno.h>
#ifdef NULL
#error "NULL should NOT be defined in errno.h"
#undef NULL
#endif

/* does not exist in cc65 (yet)
#include <fenv.h>
#ifdef NULL
#error "NULL should NOT be defined in fenv.h"
#undef NULL
#endif */

/* does not exist in cc65 (yet)
#include <float.h>
#ifdef NULL
#error "NULL should NOT be defined in float.h"
#undef NULL
#endif */

#include <inttypes.h>
#ifdef NULL
#error "NULL should NOT be defined in inttypes.h"
#undef NULL
#endif

#include <iso646.h>
#ifdef NULL
#error "NULL should NOT be defined in iso646.h"
#undef NULL
#endif

#include <limits.h>
#ifdef NULL
#error "NULL should NOT be defined in limits.h"
#undef NULL
#endif

/* does not exist in cc65 (yet)
#include <math.h>
#ifdef NULL
#error "NULL should NOT be defined in math.h"
#undef NULL
#endif */

#include <setjmp.h>
#ifdef NULL
#error "NULL should NOT be defined in setjmp.h"
#undef NULL
#endif

#include <signal.h>
#ifdef NULL
#error "NULL should NOT be defined in signal.h"
#undef NULL
#endif

#include <stdarg.h>
#ifdef NULL
#error "NULL should NOT be defined in stdarg.h"
#undef NULL
#endif

#include <stdbool.h>
#ifdef NULL
#error "NULL should NOT be defined in stdbool.h"
#undef NULL
#endif

#include <stdint.h>
#ifdef NULL
#error "NULL should NOT be defined in stdint.h"
#undef NULL
#endif

/* does not exist in cc65 (yet)
#include <tgmath.h>
#ifdef NULL
#error "NULL should NOT be defined in tgmath.h"
#undef NULL
#endif */

/* does not exist in cc65 (yet)
#include <wctype.h>
#ifdef NULL
#error "NULL should NOT be defined in wctype.h"
#undef NULL
#endif */

int main(void)
{
    return 0;
}
