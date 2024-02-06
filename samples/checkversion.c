
/* Until 2.19 the __CC65__ macro was defined as (VER_MAJOR * 0x100) + (VER_MINOR * 0x10),
 * which caused it to contain broken values in compiler releases 2.16 to 2.19. The
 * macro was fixed some time after 2.19 to (VER_MAJOR * 0x100) + VER_MINOR.
 *
 * The following strategy can be used to still compare for less or greater versions,
 * should this really be necessary or wanted - it is not recommended afterall.
 */

#include <stdio.h>
#include <stdlib.h>

#if ((__CC65__ & 0xff00) > 3) || ((__CC65__ & 0x000f) > 0)
/* compiler version is 2.19-git or higher */
# define VER_MAJOR       ((__CC65__ >> 8) & 0xff)
# define VER_MINOR       (__CC65__ & 0xff)
#elif ((__CC65__ & 0xff00) == 3)
/* broken values in version 2.16 - 2.19-git before the bug was fixed */
# define VER_MAJOR       2
# define VER_MINOR       (((__CC65__ >> 4) & 0x0f) + 16)
#else
/* values before 2.16 */
# define VER_MAJOR       ((__CC65__ >> 8) & 0xff)
# define VER_MINOR       ((__CC65__ >> 4) & 0x0f)
#endif

/* define a new value that will work for comparing all versions */
#define VERSION ((VER_MAJOR << 8) + VER_MINOR)

int main(void)
{
    printf("__CC65__ defined as %04x\n", __CC65__);
    printf("compiler version is %u.%u\n", VER_MAJOR, VER_MINOR);
    if (__CC65__ == VERSION) {
        printf("__CC65__ is defined correctly as (VER_MAJOR * 0x100) + VER_MINOR\n");
        return EXIT_SUCCESS;
    }
    printf("__CC65__ is incorrectly defined as (VER_MAJOR * 0x100) + (VER_MINOR * 0x10)\n");
    return EXIT_FAILURE;
}
