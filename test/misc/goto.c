#include <stdio.h>
#define false 0
#define true (!false)

int main () {
    int var  = 3;
    int quit = false;

    goto finish;

    while (!quit) {
        var += 1;
        {
            if (var % 2) {
                int var2 = 2;
                int var3 = 4;
                goto safe;
                goto unsafe;
                {
                another:
                    var2 = 0x5599;
                safe:
                    printf ("var2: %d\n", var2);
                }
            } else {
                int var3 = 3;
                int x    = 4;
                goto unsafe;
                goto bad;
            unused:
                printf ("var3: %d\n", var3);
                {
                    int var = 1;
                bad:
                    var++;
                    if (var < 4)
                        goto bad;
                    goto finish;
                }
            unsafe:
                var3 = 4;
                goto another;
            }

            {
                int var = 2;
                goto bad;
            }


            var += 1;
            if (var >= 10)
                goto finish;
        }
    }
finish:
    return var;
}

int function () {
    goto end;

end:
    ;
}
