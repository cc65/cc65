#include <stdio.h>

int main () {
    char a[200] = "xyz";
    int  ctr    = 0;
start:
    a[ctr] = ctr + 65;
    goto second;

    {
        char b[64] = "xxx";
    first:
        b[0] = ctr + 97;
        goto safe;
        b[0] = 'Z';
    safe:
        printf ("%c%c", a[0], b[0]);
        if (ctr++ > 20)
            goto end;
        else
            goto second;
    }
    {
        char c[100] = "aaa";
    second:;
        c[0]  = '1';
        c[99] = '2';
        goto first;
    }
end:
    a[ctr] = '\n';
    printf ("\n%s\n", a);

    return 0;
}
